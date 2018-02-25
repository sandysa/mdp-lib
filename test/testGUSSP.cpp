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

#include "../include/util/flags.h"
#include "../include/util/general.h"
#include "../include/util/graph.h"

#include "../include/domains/ctp/CTPOptimisticHeuristic.h"
#include "../include/domains/ctp/CTPProblem.h"
#include "../include/domains/ctp/CTPState.h"

#include "../include/domains/gridworld/GridWorldProblem.h"
#include "../include/domains/gridworld/GWManhattanHeuristic.h"
#include "../include/domains/gridworld/GWDetHeuristicGUSSP.h"

#include "../include/domains/SearchRescue/SearchRescueProblem.h"
#include "../include/domains/SearchRescue/SRDetHeuristicGUSSP.h"

#include "../include/domains/rocksample/RockSampleProblem.h"
#include "../include/domains/rocksample/RSDetHeuristicGUSSP.h"

#include "../include/domains/EV/EVProblem.h"
#include "../include/domains/EV/EVDetHeuristic.h"

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
    problem = new GridWorldProblem(grid.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);
    if (!flag_is_registered_with_value("heuristic") ||
            flag_value("heuristic") == "domain")
        heuristic = new GWManhattanHeuristic((GridWorldProblem*) problem);
    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new GWDetHeuristicGUSSP((GridWorldProblem*) problem);
}
void setupSearchRescue()
{
    string searchproblem = flag_value("searchrescue");
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");
    bool all_directions = flag_is_registered("sr-all-dir");
    problem = new SearchRescueProblem(searchproblem.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);

    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new SRDetHeuristicGUSSP((SearchRescueProblem*) problem);
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
            flag_value("heuristic") == "domainGUSSP")
        heuristic = new EVDetHeuristic(startSOC,endSOC,startTime,endTime,1); //setting reward case = 1 for heursitic calculation
 }

void setupRockSample()
{
    string rocksample = flag_value("rocksample");
    bool uniform_goal_dist = flag_is_registered("uniform-goal-dist");
    bool all_directions = flag_is_registered("sr-all-dir");
    problem = new RockSampleProblem(rocksample.c_str(), 1.0, 100.0, all_directions, uniform_goal_dist);

    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new RSDetHeuristicGUSSP((RockSampleProblem*) problem);
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
    } else {
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

    if(algorithm == "acarm" || algorithm == "alld")
        adjustCost = true;

    if (algorithm == "wlao") {
        double weight = 1.0;
        if (flag_is_registered_with_value("weight"))
            weight = stof(flag_value("weight"));
        solver = new LAOStarSolver(problem, tol, 1000000, weight);
    } else if (algorithm == "lao") {
        solver = new LAOStarSolver(problem, tol, 1000000);
    } else if (algorithm == "brtdp") {
        // BRTDP is just VPI-RTDP with beta = 0
        double tau = 100;
        solver = new VPIRTDPSolver(problem, tol, trials,
                                   -1.0, 0.0, tau,
                                   mdplib::dead_end_cost + 10.0);
        useUpperBound = true;
    }
    else if (algorithm == "prm" || algorithm == "acarm") {
        solver = new PRM_LAOStarSolver(problem, adjustCost, tol, 1000000);
    }  else if (algorithm == "lrtdp") {
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
                                  useProbsDepth);
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
    } else if (algorithm == "hdp") {
        int plaus;
        if (flag_is_registered_with_value("i"))
            solver = new HDPSolver(problem, tol, stoi(flag_value("i")));
        else
            solver = new HDPSolver(problem, tol);
    } else if (algorithm == "vi") {
        solver = new VISolver(problem, 1000000000, tol);
    } else if (algorithm == "ssipp") {
        solver = new SSiPPSolver(problem, tol, horizon);
    } else if (algorithm == "labeled-ssipp") {
        solver = new SSiPPSolver(problem, tol, horizon, SSiPPAlgo::Labeled);
    } else if (algorithm == "det") {
        solver = new DeterministicSolver(problem,
                                         mlsolvers::det_most_likely,
                                         heuristic);
    } else if (algorithm == "alld") {
        solver = new CostAdjusted_DeterministicSolver(problem, adjustCost,
                                         mlsolvers::det_most_likely,
                                         heuristic);
    }else if (algorithm == "uct") {
        int rollouts = 1000;
        int cutoff = 50;
        int delta = 5;
        double C = 0.0;
        bool use_qvalues_for_c = true;
        if (flag_is_registered_with_value("rollouts"))
            rollouts = stoi(flag_value("rollouts"));
        if (flag_is_registered_with_value("cutoff"))
            cutoff = stoi(flag_value("cutoff"));
        if (flag_is_registered_with_value("delta"))
            delta = stoi(flag_value("delta"));
        if (flag_is_registered("cexp")) {
            C = stod(flag_value("cexp"));
            use_qvalues_for_c = false;
        }

        solver = new UCTSolver(problem,
                               rollouts, cutoff, C,
                               use_qvalues_for_c, delta,
                               true);
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

  return false;
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
            if (algorithm == "uct") {
                static_cast<UCTSolver*>(solver)->reset();
            } else if (algorithm != "greedy") {
                solver->solve(problem->initialState());
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
        double costTrial = 0.0;
        int plausTrial = 0;
        while (!problem->goal(tmp)) {
            statesSeen.insert(tmp);
            Action* a;
            if (a == nullptr && (algorithm == "prm" || algorithm == "det" || algorithm == "acarm" || algorithm == "alld")) {
                startTime = clock();
                a = solver->solve(tmp);
                endTime = clock();
                totalTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
                }

            else
            {

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

 //            /** debugging with VI **/
//        for (State* s : problem->states()){
//            SearchRescueState* srs =  static_cast<SearchRescueState*> (s);
//            SearchRescueProblem* srp = static_cast<SearchRescueProblem*> (problem);
//            if(srp->isPotentialGoal(srs))
//                cout <<  s << ", " << s->bestAction() << " cost = " << problem->cost(s,s->bestAction()) << endl;
//        }


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
    } else if (verbosity >= 0 && verbosity < 1) {
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
    cout << " states = " << problem->states().size() << "  actions = " <<  problem->actions().size() << endl;

     if (flag_is_registered_with_value("heuristic")) {
        if (flag_value("heuristic") == "hmin") {
            clock_t startTime = clock();
            bool solveAll = flag_is_registered("hmin-solve-all");
            heuristic = new HMinHeuristic(problem, solveAll);

           clock_t endTime = clock();
            if (verbosity >= 100) {
                cout << "Heuristic took " <<
                    (double(endTime - startTime) / CLOCKS_PER_SEC) <<
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
                    simulate(solver, alg_item, nsims, t, perReplan);
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

