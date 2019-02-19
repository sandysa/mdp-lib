#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <algorithm>

#include "../include/solvers/BoundedRTDPSolver.h"
#include "../include/solvers/DeterministicSolver.h"
#include "../include/solvers/HMinHeuristic.h"
#include "../include/solvers/LAOStarSolver.h"
#include "../include/solvers/LRTDPSolver.h"
#include "../include/solvers/FLARESSolver.h"
#include "../include/solvers/SoftFLARESSolver.h"
#include "../include/solvers/Solver.h"
#include "../include/solvers/VISolver.h"
#include "../include/solvers/CostAdjusted_DeterministicSolver.h"
#include "../include/solvers/PRM_LAOStarSolver.h"
#include "../include/solvers/m02EVSolver.h"

#include "../include/util/flags.h"
#include "../include/util/general.h"
#include "../include/util/graph.h"

#include "../include/domains/borderexit/BorderExitProblem.h"

#include "../include/domains/ctp/CTPOptimisticHeuristic.h"
#include "../include/domains/ctp/CTPProblem.h"
#include "../include/domains/ctp/CTPState.h"

#include "../include/domains/gridworld/GridWorldProblem.h"
#include "../include/domains/gridworld/GWManhattanHeuristic.h"

#include "../include/domains/racetrack/RacetrackProblem.h"
#include "../include/domains/racetrack/RTrackDetHeuristic.h"

#include "../include/domains/sailing/SailingNoWindHeuristic.h"
#include "../include/domains/sailing/SailingProblem.h"

#include "../include/domains/EV/EVProblem.h"
#include "../include/domains/EV/EVDetHeuristic.h"

#include "../include/domains/corridor/CorridorProblem.h"

using namespace mdplib;
using namespace mlcore;
using namespace mlsolvers;
using namespace std;


Problem* problem = nullptr;
Heuristic* heuristic = nullptr;
bool useUpperBound = false;

int verbosity = 0;
bool useOnline = false;
bool print_decisions = false;
double tol = 1.0e-3;

std::vector<std::pair<std::pair<State*, Action*>,double>> state_ca;

bool sortCosts(std::pair<std::pair<State*, Action*>,double> s1, std::pair<std::pair<State*, Action*>,double> s2) { return (s1.second > s2.second); }

///////////////////////////////////////////////////////////////////////////////
//                              PROBLEM SETUP                                //
///////////////////////////////////////////////////////////////////////////////
void setupRacetrack()
{
    string trackName = flag_value("track");
    if (verbosity > 100)
        cout << "Setting up racetrack " << trackName << endl;
    int mds = -1;
    if (flag_is_registered_with_value("mds"))
        mds = stoi(flag_value("mds"));
    double perror = 0.05;
    if (flag_is_registered_with_value("perror"))
        perror = stod(flag_value("perror"));
    double pslip = 0.10;
    if (flag_is_registered_with_value("pslip"))
        pslip = stod(flag_value("pslip"));
    problem = new RacetrackProblem(trackName.c_str());
    ((RacetrackProblem*) problem)->pError(perror);
    ((RacetrackProblem*) problem)->pSlip(pslip);
    ((RacetrackProblem*) problem)->mds(mds);
    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic = new RTrackDetHeuristic(trackName.c_str());

    problem->ProblemName("racetrack");
 }

void setupCorridor()
{
    string trackName = flag_value("corridor");
    if (verbosity > 100)
        cout << "Setting up corridor " << trackName << endl;
    problem = new CorridorProblem(trackName.c_str());
     problem->ProblemName("corridor");
 }


void setupBorderExitProblem()
{
    problem = new BorderExitProblem();
}


void setupEV()
{
    if (verbosity >= 100)
        cout << "Setting up EV " << endl;

    int startSOC = atoi(flag_value("start-soc").c_str());
    int endSOC = atoi(flag_value("end-soc").c_str());
    double startTime = atof(flag_value("start-time").c_str());
    double endTime = atof(flag_value("end-time").c_str());
    int rewardCase =  atoi(flag_value("reward").c_str());

    problem = new EVProblem(startSOC,endSOC,startTime,endTime,rewardCase);

    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic = new EVDetHeuristic(startSOC,endSOC,startTime,endTime,1); //setting reward case = 1 for heursitic calculation
 }

void setupGridWorld()
{
    string grid = flag_value("grid");
    if (verbosity > 100)
        cout << "Setting up grid world " << grid << endl;
    bool all_directions = flag_is_registered("gw-all-dir");
    problem = new GridWorldProblem(grid.c_str(), 1.0, 50.0, all_directions);
    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic = new GWManhattanHeuristic((GridWorldProblem*) problem);
}


void setupSailingDomain()
{
    static vector<double> costs;
    costs.push_back(1);
    costs.push_back(2);
    costs.push_back(5);
    costs.push_back(10);
    costs.push_back(mdplib::dead_end_cost + 1);

    static double windTransition[] = {
        0.20, 0.20, 0.20, 0.00, 0.00, 0.00, 0.20, 0.20,
        0.20, 0.20, 0.20, 0.20, 0.00, 0.00, 0.00, 0.20,
        0.20, 0.20, 0.20, 0.20, 0.20, 0.00, 0.00, 0.00,
        0.00, 0.20, 0.20, 0.20, 0.20, 0.20, 0.00, 0.00,
        0.00, 0.00, 0.20, 0.20, 0.20, 0.20, 0.20, 0.00,
        0.00, 0.00, 0.00, 0.20, 0.20, 0.20, 0.20, 0.20,
        0.20, 0.00, 0.00, 0.00, 0.20, 0.20, 0.20, 0.20,
        0.20, 0.20, 0.00, 0.00, 0.00, 0.20, 0.20, 0.20};

    if (!flag_is_registered_with_value("sailing-goal")) {
        cerr << "Must specify sailing-goal argument flag" << endl;
        exit(-1);
    }

    int sizeSailing = atoi(flag_value("sailing-size").c_str());
    int goalSailing = atoi(flag_value("sailing-goal").c_str());

    if (verbosity > 100)
        cout << "Setting up sailing domain with size " << sizeSailing <<
            " with goal " << goalSailing << endl;

    problem =
        new SailingProblem(0, 0, 0,
                           goalSailing, goalSailing,
                           sizeSailing, sizeSailing,
                           costs,
                           windTransition);

    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic =
            new SailingNoWindHeuristic(static_cast<SailingProblem*>(problem));

     problem->ProblemName("sailing");
}


void setupCTP()
{
    if (verbosity > 100) {
        cout << "Setting up Canadian Traveler Problem " <<
            flag_value("ctp") << endl;
    }
    problem = new CTPProblem(flag_value("ctp").c_str());
    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic =
            new CTPOptimisticHeuristic(static_cast<CTPProblem*> (problem));
}


void setupProblem()
{
    if (verbosity > 100)
        cout << "Setting up problem" << endl;
    if (flag_is_registered_with_value("track")) {
        setupRacetrack();
    } else if (flag_is_registered_with_value("grid")) {
        setupGridWorld();
    } else if (flag_is_registered_with_value("sailing-size")) {
        setupSailingDomain();
    } else if (flag_is_registered_with_value("ctp")) {
        setupCTP();
    } else if (flag_is_registered("border-problem")) {
        setupBorderExitProblem();
    } else if (flag_is_registered_with_value("start-soc")) {
        setupEV();
    } else if (flag_is_registered_with_value("corridor")){
        setupCorridor();
    }else {
        cerr << "Invalid problem." << endl;
        exit(-1);
    }
}


///////////////////////////////////////////////////////////////////////////////
//                                  SOLVER SETUP                             //
///////////////////////////////////////////////////////////////////////////////

// Initializes the given solver according to the given algorithm.
void initSolver(string algorithm, Solver*& solver)
{
    bool adjustCost = false;
    assert(flag_is_registered_with_value("algorithm"));

    if (flag_is_registered("dead-end-cost")) {
        mdplib::dead_end_cost = stof(flag_value("dead-end-cost"));
    }

    int horizon = 0, expansions = 1, trials = 1000000;
    if (flag_is_registered_with_value("horizon"))
        horizon = stoi(flag_value("horizon"));
    if (flag_is_registered_with_value("expansions"))
        expansions = stoi(flag_value("expansions"));
    if (flag_is_registered_with_value("trials"))
        trials = stoi(flag_value("trials"));
    if (flag_is_registered_with_value("tol"))
        tol = stof(flag_value("tol"));

     if(algorithm == "acarm" || algorithm == "alld")
        adjustCost = true;

    if (algorithm == "wlao") {
        double weight = 1.0;
        if (flag_is_registered_with_value("weight"))
            weight = stof(flag_value("weight"));
        solver = new LAOStarSolver(problem, tol, 1000000, weight);
    } else if (algorithm == "lao") {
        solver = new LAOStarSolver(problem, tol, 1000000);
    }  else if (algorithm == "prm" || algorithm == "acarm") {
        solver = new PRM_LAOStarSolver(problem, adjustCost, tol, 1000000);
    } else if (algorithm == "lrtdp") {
        solver = new LRTDPSolver(problem, trials, tol);
    } else if (algorithm == "flares") {
        bool optimal = flag_is_registered("optimal");
        bool useProbsDepth = flag_is_registered("use-prob-depth");
        double depth = horizon;
        if (flag_is_registered("prob"))
            depth = stof(flag_value("prob"));
        solver = new FLARESSolver(
            problem, trials, tol, depth, optimal, useProbsDepth);
    } else if (algorithm == "soft-flares") {
        double depth = horizon;
        double alpha = 0.10;
        bool optimal = flag_is_registered("optimal");
        TransitionModifierFunction mod_func = kLogistic;
        DistanceFunction dist_func = kStepDist;
        if (flag_is_registered_with_value("alpha"))
            alpha = stof(flag_value("alpha"));
        // Distance functions
        if (flag_is_registered("dist")) {
            string dist_str = flag_value("dist");
            if (dist_str == "traj") {
                dist_func = kTrajProb;
            } else if (dist_str == "plaus") {
                dist_func = kPlaus;
            } else if (dist_str == "depth") {
                dist_func = kStepDist;
            } else {
                cerr << "Error: unknown distance function." << endl;
                exit(0);
            }
        }
        // Labeling functions
        if (flag_is_registered("labelf")) {
            string labelf_str = flag_value("labelf");
            if (labelf_str == "exp") {
                mod_func = kExponential;
            } else if (labelf_str == "step") {
                mod_func = kStep;
            } else if (labelf_str == "linear") {
                mod_func = kLinear;
            } else if (labelf_str == "logistic") {
                mod_func = kLogistic;
            } else {
                cerr << "Error: unknown labeling function." << endl;
                exit(0);
            }
        }
        solver = new SoftFLARESSolver(
            problem, trials, tol, depth, mod_func, dist_func,
            alpha, false, optimal);
    } else if (algorithm == "vi") {
        solver = new VISolver(problem, 1000000000, tol);
    } else if (algorithm == "det") {
        solver = new DeterministicSolver(problem,
                                         mlsolvers::det_most_likely,
                                         heuristic);
    } else if (algorithm == "alld") {
        solver = new CostAdjusted_DeterministicSolver(problem, adjustCost,
                                         mlsolvers::det_most_likely,
                                         heuristic);
    }else if(algorithm == "m02EV"){
        solver = new m02EVSolver(problem, tol, 1000000);
    }

   else if (algorithm != "greedy") {
        cerr << "Unknown algorithm: " << algorithm << endl;
        exit(-1);
    }

}

/////////////////////////////////////////////////////////////////////////////
//                  PRINTS FULL MODEL USAGE                                //
////////////////////////////////////////////////////////////////////////////
void printfullModelUsage(Solver* solver)
{
    double num_fullModel = 0.0;
//    std::cout << "full mode usage count = " << solver->isFullModel_.size() << " % full model use  = "
//                    << (solver->isFullModel_.size()/double(problem->states().size()))*100 << std::endl;
    PRM_LAOStarSolver* prmsolver = static_cast<PRM_LAOStarSolver*>(solver);
    for ( auto it = prmsolver->FullModelMap_.begin(); it != prmsolver->FullModelMap_.end(); ++it )
    {
       std::list<int> actionset = it->second;
       num_fullModel = num_fullModel + actionset.size();
    }
  std::cout << "fullmodel usage count = " << num_fullModel <<
            " %fullmodel = " << (num_fullModel/double(problem->states().size() * problem->actions().size()))*100 << std::endl;

}

///////////////////////////////////////////////////////////////////////////////
//                              SIMULATION CODE                              //
///////////////////////////////////////////////////////////////////////////////
void updateStatistics(double cost, int n, double& mean, double& M2)
{
    double delta = cost - mean;
    mean += delta / n;
    M2 += delta * (cost - mean);
}

bool mustReplan(Solver* solver, string algorithm, State* s, int plausTrial) {
    if (flag_is_registered("online"))
      return true;
    if (algorithm == "flares") {
      return !s->checkBits(mdplib::SOLVED_FLARES);
    }
    if (algorithm == "lrtdp") {
    return !s->checkBits(mdplib::SOLVED);
    }
    if (algorithm == "soft-flares") {
      if (s->checkBits(mdplib::SOLVED))
          return false;
      SoftFLARESSolver* flares = static_cast<SoftFLARESSolver*>(solver);
      return !flares->labeledSolved(s);
    }
    if(algorithm == "prm" || algorithm == "det" || algorithm == "m02EV")
        return true;

    return false;
}

/***********Calculates optimal cost adjustment in hindsight **********/
void calculateOptCostAdjust(Problem* problem, Solver* solver){
    int count_percent = 0;
    int total_sa = 0;
    for(State* s : problem->states()){
        for(Action* a : problem->actions()){
            if(problem->applicable(s,a)){
                total_sa++;
                State* mlo = mostLikelyOutcome(problem, s,a);
                double cost_adjustment = qvalue(problem,s,a) -  mlo->cost();

                double percent_ca_difference  = (problem->cost(s,a) > 0)? std::abs(cost_adjustment - problem->cost(s,a))/ (problem->cost(s,a)) * 100.0 : 0.0;
                state_ca.push_back(std::make_pair(std::make_pair(s,a), percent_ca_difference));
            }
        }
    }
    if(state_ca.size() > 0 ){
       std::sort (state_ca.begin(), state_ca.end()-1, sortCosts);
    }
}


// Runs [numSims] of the given solver and and returns the results
// (i.e., expectedCost, variance, totalTime, statesSeen).
// Argument [algorithm] is the name of the algorithm implemented by [solver].
// Argument [maxTime], if set to > 0, specifies the maximum time allowed to
// the algorithm to complete all simulations (in milliseconds).
// If [perReplan] is passed, then [maxTime] is used as the maximum time allowed
// per re-planning event.
vector<double> simulate(Solver* solver,
                        string algorithm,
                        int numSims,
                        int maxTime = -1,
                        bool perReplan = false)
{
    double expectedCost = 0.0;
    double variance = 0.0;
    double totalTime = 0.0;
    double longestTime = 0.0;
    StateSet statesSeen;
    int cnt = 0;
    int numDecisions = 0;

    clock_t simulationsStartTime = clock();
    for (int i = 0; i < numSims; i++) {
        if (verbosity >= 100)
            cout << " ********* Simulation Starts ********* " << endl;
        clock_t startTime, endTime;
        if (i == 0 && !flag_is_registered("no-initial-plan")) {
            for (State* s : problem->states())
                s->reset();
            if (maxTime > 0) {
                solver->maxPlanningTime(maxTime);
            }
            startTime = clock();
            if (algorithm != "greedy") {
                solver->solve(problem->initialState());
            }
            endTime = clock();
            double planTime = (double(endTime - startTime) / CLOCKS_PER_SEC);
            totalTime += planTime;
            longestTime = std::max(longestTime, planTime);
            numDecisions++;
        }
        if (verbosity >= 10) {
            cout << "Starting simulation " << i << endl;
          }
        State* tmp = problem->initialState();
        if (verbosity >= 100) {
            cout << "Estimated cost " <<
                problem->initialState()->cost() << endl;
        }

        double costTrial = 0.0;
        int plausTrial = 0;
        while (!problem->goal(tmp)) {
            statesSeen.insert(tmp);
            Action* a;
            if (mustReplan(solver, algorithm, tmp, plausTrial)) {
                startTime = clock();
                int simulationsElapsedTime =
                    std::ceil(1000 * (double(startTime - simulationsStartTime)
                                / CLOCKS_PER_SEC));
                if (maxTime > -1) {
                    int planningTime = perReplan ?
                        maxTime : std::max(0, maxTime - simulationsElapsedTime);
                    solver->maxPlanningTime(planningTime);
                }
                if (algorithm != "greedy")
                    solver->solve(tmp);
                endTime = clock();
                double planTime =
                    (double(endTime - startTime) / CLOCKS_PER_SEC);
                totalTime += planTime;
                longestTime = std::max(longestTime, planTime);
                numDecisions++;
                a = greedyAction(problem, tmp);
            } else {
                if (useUpperBound) {
                    // The algorithms that use upper bounds store the
                    // greedy action with respect to the upper bound
                    // in State::bestAction_
                    a = tmp->bestAction();
                }
                else {
                    a = greedyAction(problem, tmp);
                }
            }

            if (verbosity >= 1000) {
                cout << "State/Action: " << tmp << " " << a << " " << endl;
            }

            costTrial += problem->cost(tmp, a);
            costTrial = std::min(costTrial, mdplib::dead_end_cost);
            if (costTrial >= mdplib::dead_end_cost) {
                break;
            }
            double prob = 0.0;
            State* aux = randomSuccessor(problem, tmp, a, &prob);
            tmp = aux;
        }
        if (flag_is_registered("ctp")) {
            CTPState* ctps = static_cast<CTPState*>(tmp);
            if (!ctps->badWeather()) {
                cnt++;
                updateStatistics(costTrial, cnt, expectedCost, variance);
            }
        } else {
            cnt++;
            updateStatistics(costTrial, cnt, expectedCost, variance);
            if (verbosity >= 10)
                cout << costTrial << endl;
        }
    }
    if(print_decisions)
    {
         for (State* s : problem->states()){
                if(s->bestAction() != nullptr)
                    cout << s << " , bestaction = " << s->bestAction() << endl;
            }
    }
    if(algorithm == "prm" || algorithm == "acarm"){
         printfullModelUsage(solver);
    }
    if (verbosity >= 1) {
        cout << "Estimated cost " << problem->initialState()->cost() << " ";
        cout << "Avg. Exec cost " << expectedCost << " ";
        cout << "Std. Dev. " << sqrt(variance / (cnt - 1)) << " ";
        cout << "Total time " << totalTime / cnt << " " << endl;
        cout << "States seen " << statesSeen.size() << endl;
        cout << "Avg. time per decision "
             << totalTime / numDecisions << endl
             << "Longest planning time " << longestTime << endl;
        cout << "Num. decisions " << numDecisions << endl;
    } else if (verbosity >= 0) {
        cout << problem->initialState()->cost() << " ";
        cout << expectedCost << " " << sqrt(variance / (cnt - 1)) << " " <<
            totalTime / cnt << " " << totalTime / numDecisions << endl;
    }

    double reportedTime = perReplan ?
        totalTime / numDecisions : totalTime;
    double results[] = {expectedCost,
                        variance / (cnt - 1),
                        reportedTime,
                        double(statesSeen.size())};
    return vector<double>(results, results + sizeof(results) / sizeof(double));
}


///////////////////////////////////////////////////////////////////////////////
//                                     MAIN                                  //
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[])
{
    register_flags(argc, args);
    int budget = 10;
    verbosity = 0;
    if (flag_is_registered_with_value("v"))
        verbosity = stoi(flag_value("v"));
    if (flag_is_registered("debug"))
        mdplib_debug = true;
    if (flag_is_registered_with_value("dead-end-cost"))
        mdplib::dead_end_cost = stof(flag_value("dead-end-cost"));
    setupProblem();
    if (!flag_is_registered("dont-generate"))
        problem->generateAll();
    if(verbosity > 100)
        cout << "problem generation complete..." << endl;

   if (flag_is_registered_with_value("budget"))
        budget = stoi(flag_value("budget"));

    if (flag_is_registered_with_value("heuristic")) {
        if (flag_value("heuristic") == "hmin") {
            clock_t startTime = clock();
            bool solveAll = flag_is_registered("hmin-solve-all");
            heuristic = new HMinHeuristic(problem, solveAll);
            clock_t endTime = clock();
            if (verbosity > 100) {
                cout << "Heuristic took " <<
                    (double(endTime - startTime) / CLOCKS_PER_SEC) <<
                    " seconds."  << endl;
                cout << "Cost of initial state "
                    << problem->initialState()->cost() << endl;
            }
        } else if (flag_value("heuristic") == "zero")
            heuristic = nullptr;
    }
    problem->setHeuristic(heuristic);

    if(flag_is_registered_with_value("print-decisions")){
         print_decisions = true;
    }

    if (verbosity > 100)
        cout << problem->states().size() << " states" << endl;

    int numSims = 100;
    if (flag_is_registered_with_value("n"))
        numSims = stoi(flag_value("n"));
    int numReps = 1;
    if (flag_is_registered_with_value("reps"))
        numReps = stoi(flag_value("reps"));

    // Running simulations to evaluate each algorithm's performance
    string algorithm = flag_value("algorithm");
    stringstream ss(algorithm);
    string alg_item;
    int maxTime = -1;
    if (algorithm == "demo")
    {
        Solver* solver = nullptr;
        bool adjustCost = false;
        bool perReplan = false;
        int minTime = maxTime;
        state_ca.clear();
        // Learning from demo is just have the optimal values.
        solver = new LAOStarSolver(problem, tol, 1000000);
        for (int t = minTime; t <= maxTime; t *= 2) {
            double avgCost = 0.0, avgTime = 0.0;
            double M2Cost = 0.0, M2Time = 0.0;
            int trials = 1;
            for (int i = 1; i <= numReps; i++) {
                std::vector<double> results =
                    simulate(solver, alg_item, trials, t, perReplan);
                updateStatistics(results[0], i, avgCost, M2Cost);
                updateStatistics(results[2], i, avgTime, M2Time);
            }
            if (maxTime == -1)
                break;
        }
        calculateOptCostAdjust(problem,solver);
        cout << "state_ca size = " << state_ca.size() << endl;
        delete solver;

//        int budget_allowed[] = {0,1,5,10,15,20,25,30,40,50};
        int budget_allowed[] = {0,5,10,15,20};

        for(int k = 0; k < (sizeof(budget_allowed)/sizeof(*budget_allowed)); k++ ){
        // call PRM with optimal values
            int budget = budget_allowed[k];
            cout << "******************Init PRM solver with budget = " << budget << endl;
            Solver* solver = new PRM_LAOStarSolver(problem, adjustCost, budget, state_ca, tol, 1000000);

            for (int t = minTime; t <= maxTime; t *= 2) {
                double avgCost = 0.0, avgTime = 0.0;
                double M2Cost = 0.0, M2Time = 0.0;
                for (int i = 1; i <= numReps; i++) {
                    std::vector<double> results =
                        simulate(solver, alg_item, numSims, t, perReplan);
                    updateStatistics(results[0], i, avgCost, M2Cost);
                    updateStatistics(results[2], i, avgTime, M2Time);
                }
                if (maxTime == -1)
                    break;
            }
            printfullModelUsage(solver);
            delete solver;
        }
    }


    else{
        while (getline(ss, alg_item, ',')) {
            // cout << setw(10) << alg_item << ": ";
            Solver* solver = nullptr;
            initSolver(alg_item, solver);
            // Maximum planning time per simulation in milliseconds
            if (flag_is_registered_with_value("max_time")) {
                maxTime = stoi(flag_value("max_time"));
            }
            int minTime = maxTime;
            if (maxTime != -1 && flag_is_registered_with_value("min_time")) {
                minTime = stoi(flag_value("min_time"));
            }
            bool perReplan = flag_is_registered("per_replan");
            for (int t = minTime; t <= maxTime; t *= 2) {
                double avgCost = 0.0, avgTime = 0.0;
                double M2Cost = 0.0, M2Time = 0.0;
                for (int i = 1; i <= numReps; i++) {
                    std::vector<double> results =
                        simulate(solver, alg_item, numSims, t, perReplan);
                    updateStatistics(results[0], i, avgCost, M2Cost);
                    updateStatistics(results[2], i, avgTime, M2Time);
                }
                if (maxTime == -1)
                    break;
            }
            delete solver;
        }
    }
    delete problem;
    delete heuristic;
}
