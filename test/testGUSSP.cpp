#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "../include/solvers/DeterministicSolver.h"
#include "../include/solvers/HDPSolver.h"
#include "../include/solvers/HMinHeuristic.h"
#include "../include/solvers/LAOStarSolver.h"
#include "../include/solvers/PRM_LAOStarSolver.h"
#include "../include/solvers/LRTDPSolver.h"
#include "../include/solvers/FLARESSolver.h"
#include "../include/solvers/SoftFLARESSolver.h"
#include "../include/solvers/Solver.h"
#include "../include/solvers/SSiPPSolver.h"
#include "../include/solvers/UCTSolver.h"
#include "../include/solvers/VISolver.h"
#include "../include/solvers/CostAdjusted_DeterministicSolver.h"
#include "../include/solvers/BoundedRTDPSolver.h"
#include "../include/solvers/VPIRTDPSolver.h"
#include "../include/solvers/DeterministicGUSSPSolver.h"


#include "../include/util/flags.h"
#include "../include/util/general.h"
#include "../include/util/graph.h"

#include "../include/domains/ctp/CTPOptimisticHeuristic.h"
#include "../include/domains/ctp/CTPProblem.h"
#include "../include/domains/ctp/CTPState.h"

#include "../include/domains/gridworld/GridWorldProblem.h"
#include "../include/domains/gridworld/GWManhattanHeuristic.h"
#include "../include/domains/gridworld/GWDetHeuristicGUSSP.h"
#include "../include/domains/gridworld/GUSSPGridWorldProblem.h"

#include "../include/domains/SearchRescue/GUSSPSearchRescueProblem.h"
#include "../include/domains/SearchRescue/SRDetHeuristicGUSSP.h"

#include "../include/domains/rocksample/GUSSPRockSampleProblem.h"
#include "../include/domains/rocksample/RSDetHeuristicGUSSP.h"

#include "../include/domains/EV/GUSSPEVProblem.h"
#include "../include/domains/EV/EVDetHeuristicGUSSP.h"

using namespace mdplib;
using namespace mlcore;
using namespace mlsolvers;
using namespace std;

Problem* problem = nullptr;
Heuristic* heuristic = nullptr;
Solver* solver = nullptr;
string algorithm = "greedy";

bool useUpperBound = false;
int verbosity = 0;
bool useOnline = false;

void setupGridWorld()
{
    string grid = flag_value("grid");
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");
    if (verbosity > 100)
        cout << "Setting up grid world " << grid << endl;
    bool all_directions = flag_is_registered("gw-all-dir");
    problem = new GUSSPGridWorldProblem(grid.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);
    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new GWDetHeuristicGUSSP((GUSSPGridWorldProblem*) problem);
    else if (!flag_is_registered("heuristic"))
        heuristic = nullptr;

    problem->ProblemName("GUSSPGridWorld");
}
void setupSearchRescue()
{
    string searchproblem = flag_value("searchrescue");
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");
    bool all_directions = flag_is_registered("sr-all-dir");
    problem = new GUSSPSearchRescueProblem(searchproblem.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);

    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new SRDetHeuristicGUSSP((GUSSPSearchRescueProblem*) problem);
    problem->ProblemName("GUSSPSearchRescue");
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
    int exit_range = atoi(flag_value("exit-range").c_str());
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");

    problem = new GUSSPEVProblem(startSOC,endSOC,startTime,rewardCase, exit_range, uniform_goal_dist);

    if (flag_value("heuristic") == "domainGUSSP")
        heuristic = new EVDetHeuristicGUSSP((GUSSPEVProblem*) problem);

    problem->ProblemName("GUSSPEV");
 }

void setupRockSample()
{
    string rocksample = flag_value("rocksample");
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");
    bool all_directions = flag_is_registered("sr-all-dir");
    problem = new GUSSPRockSampleProblem(rocksample.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);

    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new RSDetHeuristicGUSSP((GUSSPRockSampleProblem*) problem);

    problem->ProblemName("GUSSPRockSample");
}
void setupProblem()
{
    if (verbosity > 100)
        cout << "Setting up problem" << endl;
    if (flag_is_registered_with_value("grid")) {
        setupGridWorld();
    }
    else if (flag_is_registered_with_value("searchrescue")) {
        setupSearchRescue();
    }
    else if (flag_is_registered_with_value("rocksample")) {
        setupRockSample();
    }
    else if (flag_is_registered_with_value("start-soc")) {
        setupEV();
   }
    else {
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
    double tol = 1.0e-3;
    bool adjustCost = false;
    assert(flag_is_registered_with_value("algorithm"));
    algorithm = flag_value("algorithm");

    int horizon = 0, expansions = 1, trials = 10000;
    if (flag_is_registered_with_value("horizon"))
        horizon = stoi(flag_value("horizon"));
    if (flag_is_registered_with_value("expansions"))
        expansions = stoi(flag_value("expansions"));
    if (flag_is_registered_with_value("trials"))
        trials = stoi(flag_value("trials"));
    if (flag_is_registered_with_value("tol"))
        tol = stof(flag_value("tol"));

    if (algorithm == "lao") {
        solver = new LAOStarSolver(problem, tol, 1000000);
    }
    else if (algorithm == "detGUSSP"){
        solver = new DeterministicGUSSPSolver(problem, 1);
    }
     else if (algorithm == "lrtdp") {
        solver = new LRTDPSolver(problem, trials, tol);
    } else if (algorithm == "rtdp-ub") {
        // RTDP with upper bound action selection
        // is just VPI-RTDP with vanillaSample set to true
        solver = new VPIRTDPSolver(problem, tol, trials,
                                   0.0, 0.0, 0.0,
                                   mdplib::dead_end_cost + 10.0,
                                   true);
        useUpperBound = true;
    } else if (algorithm == "vpi-rtdp") {
        double alpha = 1.0;
        double beta = 0.95 * mdplib::dead_end_cost;
        double tau = 100;
        if (flag_is_registered_with_value("beta"))
            beta = stof(flag_value("beta"));
        if (flag_is_registered_with_value("alpha"))
            alpha = stof(flag_value("alpha"));
        solver = new VPIRTDPSolver(problem,
                                   tol, trials,
                                   alpha, beta, tau,
                                   mdplib::dead_end_cost);
        if (flag_is_registered("vpi-delta"))
            static_cast<VPIRTDPSolver*>(solver)->sampleVPIDelta();
        else if (flag_is_registered("vpi-old"))
            static_cast<VPIRTDPSolver*>(solver)->sampleVPIOld();
        useUpperBound = true;
    } else if (algorithm == "flares") {
        bool optimal = flag_is_registered("optimal");
        bool useProbsDepth = flag_is_registered("use-prob-depth");
        double depth = horizon;
        if (flag_is_registered("prob")) {
            depth = stof(flag_value("prob"));
        }
        solver = new FLARESSolver(problem,
                                  trials,
                                  tol,
                                  depth,
                                  optimal,
                                  useProbsDepth); // max time set to 10000 ms in Softflares header file.
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
    }  else if (algorithm == "vi") {
        solver = new VISolver(problem, 1000000000, tol);
    }  else if (algorithm == "det") {
        solver = new DeterministicSolver(problem,
                                         mlsolvers::det_most_likely,
                                         heuristic);
    } else if (algorithm != "greedy") {
        cerr << "Unknown algorithm: " << algorithm << endl;
        exit(-1);
    }
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
  if (algorithm == "soft-flares") {
      if (s->checkBits(mdplib::SOLVED))
          return false;
      SoftFLARESSolver* flares = static_cast<SoftFLARESSolver*>(solver);
      return !flares->labeledSolved(s);
    }
  if (algorithm == "hdp") {
      if (flag_is_registered("i")) {
          int j = INT_MAX;
          if (flag_is_registered_with_value("j")) {
              j = stoi(flag_value("j"));
          }
          if (plausTrial >= j) {
              static_cast<HDPSolver*>(solver)->clearLabels();
              return true;
          }
      }
  }
  if (algorithm == "ssipp" || algorithm == "uct") {
        return true;
  }
  if(algorithm == "prm" || algorithm == "det")
    return true;

  if(algorithm == "detGUSSP"){
    if(s->bestAction() == nullptr)
        return true;
  }

  return false;
}
void experimentLogs()
{
//    std::cout << "Potential goals:" << std::endl;
//    GUSSPRockSampleProblem* rsp = static_cast<GUSSPRockSampleProblem*> (problem);
//    std::vector<std::pair<int,int>> pg = rsp->potential_goals;
//    for(int i = 0; i < pg.size(); i ++){
//        std::pair<int,int> pgpos = pg.at(i);
//        std::cout << "(" << pgpos.first << " " << pgpos.second << ")" << std::endl;
//    }
    for (State* s : problem->states()){
            if(s->bestAction() != nullptr){
             std::cout << s << " -> " <<  s->bestAction() << " , " << problem->cost(s, s->bestAction()) << std::endl;
//              for (auto const & sccr : problem->transition(s, s->bestAction()))
//                   std::cout << "\t " << sccr.su_state << ", " << sccr.su_prob << std::endl;
            }

//        GUSSPRockSampleState* rss =  static_cast<GUSSPRockSampleState*> (s);
//        if(s->bestAction() != nullptr){
//           std::vector<std::pair<std::pair<int, int>,double>> goalpos = rss->goalPos();
//            std::cout <<  rss->x() << " " << rss->y() << " " << rss->sampledRocks() <<" [" ;
//
//             for(int i = 0; i < pg.size(); i ++){
//                std::pair<int,int> pgpos = pg.at(i);
//                for(int j = 0; j < goalpos.size(); j++){
//                   std::pair<std::pair<int,int>,double> pgval = goalpos.at(j);
//                   std::pair<int,int> pos = pgval.first;
//                   if(pgpos.first == pos.first && pgpos.second == pos.second){
//                        int val = (pgval.second > 0)? 1 : 0;
//                        std::cout << val <<" ";
//                   }
//
//                }
//            }
//            std::cout << "] -> " << s->bestAction() <<std::endl;
//        }
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
                        double heuristic_time,
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
        if (verbosity >= 10)
            cout << " ********* Simulation Starts ********* " << endl;
        clock_t startTime, endTime;
        if (i == 0 && !flag_is_registered("no-initial-plan")) {
            for (State* s : problem->states())
                s->reset();
            if (maxTime > 0) {
                solver->maxPlanningTime(maxTime);
            }
            startTime = clock();
            if (algorithm == "uct") {
                static_cast<UCTSolver*>(solver)->reset();
            } else if (algorithm != "greedy") {
                solver->solve(problem->initialState());
                                                                                        if (verbosity >= 10)
                                                                                            cout << " problem solved " << endl;

                if(algorithm == "prm" || algorithm == "acarm"){
                        double max_hval = 0.0;
                        double model_size = 0.0;
                        double num_fullModel = 0.0;
                        for (State* s : problem->states()){
                            if(s->hashValue() > max_hval)
                                max_hval = s->hashValue();
                            for (Action* a : problem->actions()){
                                if(problem->applicable(s,a))
                                        model_size++;
                            }
                            }
                           std::cout << "max h value = " << max_hval << std::endl;
                          std::cout << "full mode usage count = " << solver->isFullModel_.size() << " % full model use  = "
                                                                  << (solver->isFullModel_.size()/model_size)*100 << std::endl;
                    }
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
                                                                                            experimentLogs();
       double costTrial = 0.0;
        int plausTrial = 0;
        while (!problem->goal(tmp)) {
                                                                                if(verbosity >= 10 && tmp->bestAction() != nullptr)
                                                                                    std::cout << " tmp = " << tmp << " ba = " << tmp->bestAction() << std::endl;
            statesSeen.insert(tmp);
            Action* a;
         if (a == nullptr && (algorithm == "prm" || algorithm == "det" || algorithm == "acarm" || algorithm == "alld" )) {
                startTime = clock();
                a = solver->solve(tmp);
                endTime = clock();
                totalTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
                }
          else {
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
                                                                                        if (verbosity >= 10)
                                                                                            cout << " in mustreplan " << endl;
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
            if (algorithm == "hdp") {
                double maxProb = 0.0;
                for (auto const & sccr : problem->transition(tmp, a))
                    maxProb = std::max(maxProb, sccr.su_prob);
                plausTrial +=
                    static_cast<HDPSolver*>(solver)->kappa(prob, maxProb);
            }
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

    /** Adding time taken for heuristic computation to planning time-- if heuristic
    ** was pre-computed as in hmin-solve-all
    **/
//        totalTime += heuristic_time;
    if (verbosity >= 1) {
        cout << "Estimated cost " << problem->initialState()->cost() << " ";
        cout << "Avg. Exec cost " << expectedCost << " ";
        cout << "Std. Dev. " << sqrt(variance / (cnt - 1)) << " ";
        cout << "Total time " << ((totalTime / cnt)+ heuristic_time) << " " << endl; // time in seconds
        cout << "States seen " << statesSeen.size() << endl;
        cout << "Avg. time per decision "
             << totalTime / numDecisions << endl
             << "Longest planning time " << longestTime << endl;
        cout << "Num. decisions " << numDecisions << endl;
    } else if (verbosity >= 0 && verbosity < 1) {
        cout << problem->initialState()->cost() << " ";
        cout << expectedCost << " " << sqrt(variance / (cnt - 1)) << " " <<
            ((totalTime / cnt)+ heuristic_time)<< " " << totalTime / numDecisions << endl;
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
    double heuristic_time = 0; // in seconds
    verbosity = 0;
    if (flag_is_registered_with_value("v"))
        verbosity = stoi(flag_value("v"));
    if (flag_is_registered("debug"))
        mdplib_debug = true;
    if (flag_is_registered_with_value("dead-end-cost"))
        mdplib::dead_end_cost = stof(flag_value("dead-end-cost"));

    setupProblem();
                                                                                           if(verbosity >= 500)
                                                                                            cerr << "problem setup complete" << endl;
    if (!flag_is_registered("dont-generate"))
        problem->generateAll();
                                                                                            if(verbosity >= 100)
                                                                                            cerr << "generated all states" << endl;
    cout << "Start State: " << problem->initialState() << std::endl;
    cout << " states = " << problem->states().size() << "  actions = " <<  problem->actions().size() << endl;

     if (flag_is_registered_with_value("heuristic")) {
        if (flag_value("heuristic") == "hmin" || flag_value("heuristic") == "hmin-solve-all" ) {
            clock_t startTime = clock();
       //     bool solveAll = flag_is_registered("hmin-solve-all");
             bool solveAll = (flag_value("heuristic") == "hmin-solve-all")? true: false;
             heuristic = new HMinHeuristic(problem, solveAll);

           clock_t endTime = clock();
           if (solveAll)
                heuristic_time =  (double(endTime - startTime) / CLOCKS_PER_SEC);

            if (verbosity >= 10) {
                cout << "Heuristic took "
                    << heuristic_time <<
                    " seconds."  << endl;
             }

        } else if (flag_value("heuristic") == "zero")
            heuristic = nullptr;
    }

    problem->setHeuristic(heuristic);

    int nsims = 100;
    if (flag_is_registered_with_value("n"))
        nsims = stoi(flag_value("n"));
    int numReps = 1;
    if (flag_is_registered_with_value("reps"))
        numReps = stoi(flag_value("reps"));

    // Running simulations to evaluate each algorithm's performance
    string algorithm = flag_value("algorithm");
    stringstream ss(algorithm);
    string alg_item;
    while (getline(ss, alg_item, ',')) {
        // cout << setw(10) << alg_item << ": ";
        Solver* solver = nullptr;
        std::vector<std::pair<std::pair<int,int>,double>> pg;
        initSolver(alg_item, solver);
        // Maximum planning time per simulation in milliseconds
        int maxTime = -1;
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
                    simulate(solver, alg_item, nsims, heuristic_time, t, perReplan);
                updateStatistics(results[0], i, avgCost, M2Cost);
                updateStatistics(results[2], i, avgTime, M2Time);
                }
//            cout << t << " "
//                << avgCost << " "
//                << sqrt(M2Cost / (numReps * (numReps - 1))) << " "
//                << avgTime << " "
//                << sqrt(M2Time / (numReps * (numReps - 1))) << endl;
            if (maxTime == -1)
                break;
        }
        delete solver;
    }
    delete problem;
    delete heuristic;
 }

