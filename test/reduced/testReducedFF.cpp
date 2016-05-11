#include <cassert>
#include <ctime>
#include <iostream>
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

#include "../../include/reduced/PPDDLTaggedReduction.h"
#include "../../include/reduced/ReducedHeuristicWrapper.h"
#include "../../include/reduced/ReducedModel.h"
#include "../../include/reduced/ReducedState.h"
#include "../../include/reduced/ReducedTransition.h"

#include "../../include/solvers/FFReducedModelSolver.h"
#include "../../include/solvers/LAOStarSolver.h"

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

string ffExec = "/home/lpineda/Desktop/planning-code/FF-v2.3/ff";
string ffDomain = "-o /home/lpineda/Desktop/domain.pddl";
string ffProblem = "-f /home/lpineda/Desktop/problem.pddl";
string ffCommand = ffExec + " " + ffDomain + " " + ffProblem;


/*
 * Parses the given PPDDL file, and returns true on success.
 */
static bool read_file( const char* ppddlFileName )
{
    yyin = fopen( ppddlFileName, "r" );
    if( yyin == NULL ) {
        cout << "parser:" << ppddlFileName <<
            ": " << strerror( errno ) << endl;
        return( false );
    }
    else {
        current_file = ppddlFileName;
        bool success;
        try {
            success = (yyparse() == 0);
        }
        catch( Exception exception ) {
            fclose( yyin );
            cout << exception << endl;
            return( false );
        }
        fclose( yyin );
        return( success );
    }
}


bool initPPDDL(string ppddlArgs)
{
    size_t pos_equals = ppddlArgs.find(":");
    assert(pos_equals != string::npos);
    string file = ppddlArgs.substr(0, pos_equals);
    string prob =
        ppddlArgs.substr(pos_equals + 1, ppddlArgs.size() - pos_equals);

    pair<state_t *,Rational> *initial = nullptr;

    if( !read_file( file.c_str() ) ) {
        cerr << "<main>: ERROR: couldn't read problem file `" << file << endl;
        return false;
    }
    problem_t* internalPPDDLProblem =
        (problem_t *)(problem_t::find(prob.c_str()));
    if( !internalPPDDLProblem ) {
        cerr << "<main>: ERROR: problem `" << prob <<
            "' is not defined in file '" << file << "'" << endl;
        return false;
    }

    problem = new PPDDLProblem(internalPPDDLProblem);
    heuristic = new mlppddl::PPDDLHeuristic(static_cast<PPDDLProblem*>(problem),
                                            mlppddl::FF);
                                                                                heuristic = nullptr;
    problem->setHeuristic(heuristic);
}


/**
 * This program creates a reduced model for a given PPDDL file and solves
 * using LAO* and FF.
 *
 * The type of model is a Mk1 reduction (at most one primary outcome),
 * which implies that after k exceptions a deterministic model is used. LAO*
 * will solve this model and use FF on states with k-exceptions so that they
 * can be treated as tip nodes with a fixed cost.
 *
 * The reduction to use is specified by a set of command line arguments:
 *
 *    --det_problem: the path to a PDDL file describing the deterministic
 *        model to be passed to FF after k-exceptions.
 *    --det_descriptor: a configuration file describing the determinization
 *        in a simple format. The user of testReducedFF is responsible for
 *        ensuring consistency between the files det_problem and
 *        det_descriptor. The format of this file will be one action name per
 *        line followed by a number specifying which of the outcomes is primary.
 *    --k: The maximum number of exceptions.
 *
 * Other required command line arguments are:
 *    --problem: the PPDDL domain/problem to solve. The format is
 *        ppddlFilename:problemName
 *    --dir: the directory to use for FF. This directory must contain the file
 *        at "det_problem" as well as a file called "p01.pddl" containing only
 *        the PPDDL problem description (i.e., init and goal, not the domain).
 *        The init state must be described in a single line.
 *        The user of testReducedFF is responsible for ensuring consistency
 *        between p01.pddl and the file passed in the "problem" flag.
 */
int main(int argc, char* args[])
{
    mdplib_debug = false;

    register_flags(argc, args);

    // Reading flags.
    register_flags(argc, args);

    // The PPDDL problem to solve.
    assert(flag_is_registered_with_value("problem"));;
    string ppddlArgs = flag_value("problem");

    // The PDDL determinization to use.
    assert(flag_is_registered_with_value("det_problem"));
    string detProblem = flag_value("det_problem");

    // The configuration file for this determinization.
    assert(flag_is_registered_with_value("det_descriptor"));
    string detDescriptor = flag_value("det_descriptor");

    //The directory where the determinization is stored.
    assert(flag_is_registered_with_value("dir"));
    string directory = flag_value("dir");

    //The maximum number of exceptions.
    assert(flag_is_registered_with_value("k"));
    int k = stoi(flag_value("k"));

    // The verbosity level.
    if (flag_is_registered_with_value("v"))
        verbosity = stoi(flag_value("v"));

    // The number of simulations for the experiments.
    int nsims = 100;
    if (flag_is_registered_with_value("n"))
        nsims = stoi(flag_value("n"));

    initPPDDL(ppddlArgs);

    // Creating the reduced model.
    ReducedTransition* reduction =
        new PPDDLTaggedReduction(problem, detDescriptor);
    reducedModel = new ReducedModel(problem, reduction, k);
    reducedHeuristic = new ReducedHeuristicWrapper(heuristic);
    reducedModel->setHeuristic(reducedHeuristic);

    // We will now use the wrapper for the pro-active re-planning approach. It
    // will allow us to plan in advance for the set of successors of a
    // state-action.
    wrapperProblem = new WrapperProblem(reducedModel);

    // Solving reduced model using LAO* + FF.
    double totalPlanningTime = 0.0;
    clock_t startTime = clock();
    FFReducedModelSolver solver(wrapperProblem,
                                ffExec,
                                directory + "/" + detProblem,
                                directory + "/p01.pddl",
                                k);
    solver.solve(wrapperProblem->initialState());
    clock_t endTime = clock();
    totalPlanningTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
    cout << "cost " << wrapperProblem->initialState()->cost() <<
        " time " << totalPlanningTime << endl;


    // Running a trial of the continual planning approach.
    double expectedCost = 0.0;
    for (int i = 0; i < nsims; i++) {
        pair<double, double> costAndTime =
            reducedModel->trial(solver, wrapperProblem);
                                                                                cerr << costAndTime.first << endl;
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

