#include <cassert>
#include <ctime>
#include <list>
#include <sstream>
#include <string>
#include <typeinfo>

#include "../../include/domains/racetrack/RacetrackProblem.h"
#include "../../include/domains/racetrack/RTrackDetHeuristic.h"
#include "../../include/domains/DummyState.h"
#include "../../include/domains/WrapperProblem.h"

#include "../include/ppddl/mini-gpt/states.h"
#include "../include/ppddl/mini-gpt/problems.h"
#include "../include/ppddl/mini-gpt/domains.h"
#include "../include/ppddl/mini-gpt/states.h"
#include "../include/ppddl/mini-gpt/exceptions.h"
#include "../../include/ppddl/PPDDLHeuristic.h"
#include "../../include/ppddl/PPDDLProblem.h"

#include "../../include/reduced/CustomReduction.h"
#include "../../include/reduced/LeastLikelyOutcomeReduction.h"
#include "../../include/reduced/MostLikelyOutcomeReduction.h"
#include "../../include/reduced/RacetrackObviousReduction.h"
#include "../../include/reduced/ReducedHeuristicWrapper.h"
#include "../../include/reduced/ReducedModel.h"
#include "../../include/reduced/ReducedState.h"
#include "../../include/reduced/ReducedTransition.h"

#include "../../include/solvers/LAOStarSolver.h"
#include "../../include/solvers/Solver.h"
#include "../../include/solvers/VISolver.h"

#include "../../include/util/flags.h"
#include "../../include/util/general.h"

#include "../../include/Problem.h"


using namespace std;
using namespace mdplib;
using namespace mlppddl;
using namespace mlreduced;
using namespace mlsolvers;


extern int yyparse();
extern FILE* yyin;
string current_file;
int warning_level = 0;

static int verbosity = 0;
static int k = 0;

mlcore::Problem* problem = nullptr;
mlcore::Heuristic* heuristic = nullptr;
ReducedModel* reducedModel = nullptr;
ReducedHeuristicWrapper* reducedHeuristic = nullptr;
WrapperProblem* wrapperProblem = nullptr;
list<ReducedTransition *> reductions;


void createCustomRacetrackReductions(RacetrackProblem* rtp)
{
    CustomReduction* customReduction = new CustomReduction(rtp);
    vector<bool> primaryIndicators;
    bool first = true;
    for (mlcore::Action* a : rtp->actions()) {
        // Setting primary outcomes for initial state
        if (first) {
            // All action work the same for this state, choose the first
            for (auto const & successor :
                 rtp->transition(rtp->initialState(), a))
                primaryIndicators.push_back(true);
            customReduction->setPrimaryForState(
                rtp->initialState(), primaryIndicators);
            first = false;
        }
        primaryIndicators.clear();
        RacetrackAction* rta = static_cast<RacetrackAction*> (a);
        int numSuccessors = rtp->numSuccessorsAction(rta);
        for (int i = 0; i < numSuccessors; i++)
            primaryIndicators.push_back(true);
        customReduction->setPrimaryForAction(a, primaryIndicators);
    }
}


void initRacetrack(string trackName, int mds)
{
    problem = new RacetrackProblem(trackName.c_str());
    static_cast<RacetrackProblem*>(problem)->pError(0.05);
    static_cast<RacetrackProblem*>(problem)->pSlip(0.10);
    static_cast<RacetrackProblem*>(problem)->mds(mds);
    heuristic = new RTrackDetHeuristic(trackName.c_str());
    problem->generateAll();
    if (verbosity > 100)
        cout << "Generated " << problem->states().size() << " states." << endl;

    reductions.push_back(
        new RacetrackObviousReduction(static_cast<RacetrackProblem*>(problem)));
    reductions.push_back(new LeastLikelyOutcomeReduction(problem));
}


int main(int argc, char* args[])
{
    register_flags(argc, args);
    if (flag_is_registered("debug"))
        mdplib_debug = true;

    // Reading flags.
    assert(flag_is_registered_with_value("domain"));
    string domainName = flag_value("domain");

    assert(flag_is_registered_with_value("problem"));

    if (flag_is_registered_with_value("v"))
        verbosity = stoi(flag_value("v"));

    if (flag_is_registered_with_value("k"))
        k = stoi(flag_value("k"));

    // Creating problem
    if (domainName == "racetrack") {
        int mds = -1;
        if (flag_is_registered_with_value("mds"))
            mds = stoi(flag_value("mds"));
        string trackName = flag_value("problem");
        initRacetrack(trackName, mds);
    }

    bool useFullTransition = flag_is_registered("use_full");

    ReducedTransition* bestReduction = nullptr;
    bestReduction = reductions.front();
    wrapperProblem = new WrapperProblem(problem);

    // Finding the best reduction using Monte Carlo simulations
    double totalPlanningTime = 0.0;
    mlcore::StateSet reachableStates, tipStates;
    getReachableStates(problem, reachableStates, tipStates, 2);
    clock_t startTime = clock();
    for (auto const & reduction : reductions) {
        reducedModel = new ReducedModel(wrapperProblem, reduction, k);
        double result = reducedModel->evaluateMonteCarlo(50);
        cout << result << endl;
    }
    clock_t endTime = clock();
    double timeReductions = double(endTime - startTime) / CLOCKS_PER_SEC;
    totalPlanningTime += timeReductions;
    cout << "time finding reductions " <<
        wrapperProblem->initialState()->cost() <<
        " time " << timeReductions << endl;

    // Setting up the final reduced model to use
    reducedModel = new ReducedModel(problem, bestReduction, k);
    reducedHeuristic = new ReducedHeuristicWrapper(heuristic);
    reducedModel->setHeuristic(reducedHeuristic);
    static_cast<ReducedModel*>(reducedModel)->
        useFullTransition(useFullTransition);

    // We will now use the wrapper for the pro-active re-planning approach. It
    // will allow us to plan in advance for the set of successors of a
    // state-action
    wrapperProblem->setNewProblem(reducedModel);

    // Solving reduced model using LAO*
    startTime = clock();
    LAOStarSolver solver(wrapperProblem);
    solver.solve(wrapperProblem->initialState());
    endTime = clock();
    double timeInitialPlan = (double(endTime - startTime) / CLOCKS_PER_SEC);
    totalPlanningTime += timeInitialPlan;
    cout << "cost " << wrapperProblem->initialState()->cost() <<
        " time " << timeInitialPlan << endl;


    // Running a trial of the continual planning approach.
    double expectedCost = 0.0;
    int nsims = 100;
    for (int i = 0; i < nsims; i++) {
        pair<double, double> costAndTime =
            reducedModel->trial(solver, wrapperProblem);
        expectedCost += costAndTime.first;
    }
    cout << expectedCost / nsims << endl;
    cout << totalPlanningTime << endl;

    // Releasing memory
    for (auto reduction : reductions)
        delete reduction;
    reducedModel->cleanup();
    delete reducedModel;
    wrapperProblem->cleanup();
    delete wrapperProblem;
    delete problem;
    return 0;
}
