#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <algorithm>

#include "../include/solvers/HMinHeuristic.h"

#include "../include/solvers/Solver.h"
#include "../include/solvers/LAOStarSolver.h"

#include "../include/util/flags.h"
#include "../include/util/general.h"
#include "../include/util/graph.h"

#include "../include/domains/rocksample/GUSSPRockSampleProblem.h"
#include "../include/domains/rocksample/RSDetHeuristicGUSSP.h"

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

void setupRockSample()
{
    string rocksample = flag_value("rocksample");
    string goal_dist = flag_value("goal-dist");
    bool all_directions = flag_is_registered("sr-all-dir");
    problem = new GUSSPRockSampleProblem(rocksample.c_str(), 1.0, 100.0, all_directions, goal_dist);

    if(flag_value("heuristic") == "domainGUSSP")
        heuristic = new RSDetHeuristicGUSSP((GUSSPRockSampleProblem*) problem);

    problem->ProblemName("GUSSPRockSample");
}


///////////////////////////////////////////////////////////////////////////////
//                                  SOLVER SETUP                             //
///////////////////////////////////////////////////////////////////////////////

// Initializes the given solver according to the given algorithm.
void initSolver(string algorithm, Solver*& solver)
{
    double tol = 1.0e-3;
    bool adjustCost = false;
    int det_choice = 0;

    assert(flag_is_registered_with_value("algorithm"));
    algorithm = flag_value("algorithm");

    int horizon = 0, expansions = 1, trials = 10000;
    if (flag_is_registered_with_value("horizon"))
        horizon = stoi(flag_value("horizon"));

    if (flag_is_registered_with_value("tol"))
        tol = stof(flag_value("tol"));

    if (algorithm == "lao") {
        solver = new LAOStarSolver(problem, tol, 1000000);
    }

    else{
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

bool OptimalSolver(string algorithm)
{
    if(algorithm == "vi" || algorithm == "lao")
        return true;
    return false;
}


void experimentLogs()
{
//    std::cout << "Potential goals:" << std::endl;
    GUSSPRockSampleProblem* rsp = static_cast<GUSSPRockSampleProblem*> (problem);
    std::vector<std::pair<int,int>> pg = rsp->potential_goals;
    for(int i = 0; i < pg.size(); i ++){
        std::pair<int,int> pgpos = pg.at(i);
        std::cout << "(" << pgpos.first << " " << pgpos.second << ")" << std::endl;
    }
    for (State* s : problem->states()){
        GUSSPRockSampleState* rss =  static_cast<GUSSPRockSampleState*> (s);
        if(s->bestAction() != nullptr){
           std::vector<std::pair<std::pair<int, int>,double>> goalpos = rss->goalPos();
            std::cout <<  rss->x() << " " << rss->y() << " [" ;

             for(int i = 0; i < pg.size(); i ++){
                std::pair<int,int> pgpos = pg.at(i);
                for(int j = 0; j < goalpos.size(); j++){
                   std::pair<std::pair<int,int>,double> pgval = goalpos.at(j);
                   std::pair<int,int> pos = pgval.first;
                   if(pgpos.first == pos.first && pgpos.second == pos.second){
                        int val = (pgval.second > 0)? 1 : 0;
                        std::cout << val ;
                        if(j != goalpos.size())
                            std::cout < ",";
                   }

                }
            }
            std::cout << "] " << s->bestAction() <<std::endl;
        }
     }

}
mlcore::State* getSSPState(std::pair<int,int> loc, std::pair<int,int> pg)
{
    for (State* s : problem->states()){
        GUSSPRockSampleState* rss =  static_cast<GUSSPRockSampleState*> (s);
        std::pair<int,int> pos (rss->x(), rss->y());
        if(pos == loc){
             std::vector<std::pair<std::pair<int, int>,double>> goalPos = rss->getGoalBelief();
             for (int j = 0; j < goalPos.size(); j++){
                std::pair<std::pair<int,int>,double> pgbel = goalPos.at(j);
                if(pgbel.first == pg)
                    return s;
             }

        }
    }
}
double getCost(std::pair<std::pair<int,int>,std::pair<int,int>> loc1, Action *a, Problem* problem){
    State* s = getSSPState(loc1.first, loc1.second);
    return problem->cost(s,a);
}

double getTransition(std::pair<std::pair<int,int>,std::pair<int,int>> loc1,
                    std::pair<std::pair<int,int>,std::pair<int,int>> loc2, Action* a, Problem* problem){

     State* s = getSSPState(loc1.first, loc1.second);
     double trans = 0;
     if(loc1.second != loc2.second)  // g!=g'; trans = 0
        return 0;
    if(problem->applicable(s,a)){
     for (auto const & sccr : problem->transition(s, a)){
        GUSSPRockSampleState* rsj = static_cast<GUSSPRockSampleState*> (sccr.su_state);
        if(std::make_pair(rsj->x(),rsj->y()) == loc2.first)
            trans += sccr.su_prob;
        }
    }
    return trans;
}
int getGoalId(Problem* problem, std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> pomdp_states)
{
    GUSSPRockSampleProblem* rsp = static_cast<GUSSPRockSampleProblem*> (problem);
    PairDoubleMap* goalmap = rsp->getGoals();
    for (auto it = goalmap->begin(); it!= goalmap->end(); ++it)
    {
        std::pair<int,int> TrueGoalpos =  it->first;
        for (int j = 0; j < pomdp_states.size(); j++)
        {
            std::pair<std::pair<int,int>,std::pair<int,int>> loc  = pomdp_states.at(j);
            if(loc.first == loc.second && loc.second == TrueGoalpos)
                return j;
        }
    }
    return -1;
}

void generateRawPOMDP(Problem* problem)
{
    std::vector<std::pair<int,int>> map_coord;
    std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> pomdp_states;

    GUSSPRockSampleProblem* rsp = static_cast<GUSSPRockSampleProblem*> (problem);
    std::vector<std::pair<int,int>> pg = rsp->potential_goals;

    for (State* s : problem->states()){
        GUSSPRockSampleState* rss =  static_cast<GUSSPRockSampleState*> (s);
        std::pair<int,int> pos (rss->x(), rss->y());
        std::vector<std::pair<std::pair<int, int>,double>> goalPos = rss->getGoalBelief();

        bool found = (std::find(map_coord.begin(), map_coord.end(), pos) != map_coord.end());
        if(!found && pos.first!= -1){
            map_coord.push_back(pos);
            }
        }

      //gathering pomdp states
      int numbel = 0;
      for (int m = 0; m <  map_coord.size(); m++){
        std::pair<int,int> pos = map_coord.at(m);
        for(int i = 0; i < pg.size(); i ++){
            std::pair<int,int> pgpos = pg.at(i);
            pomdp_states.push_back(std::make_pair(pos,pgpos));
            for (State* s : problem->states()){
                GUSSPRockSampleState* rss =  static_cast<GUSSPRockSampleState*> (s);
                std::vector<std::pair<std::pair<int, int>,double>> goalPos = rss->getGoalBelief();
                if(std::make_pair(rss->x(),rss->y()) == pos){
                    for(int pt = 0; pt < goalPos.size(); pt++){
                        std::pair<std::pair<int,int>,double> belval = goalPos.at(pt);
                        if(belval.first == pgpos && rss->sampledRocks() == 0){
                            if (belval.second >  0.0)
                                numbel++;
                            std::cout << pos.first << "," << pos.second << ",(" << pgpos.first
                                    << ","<< pgpos.second  << ")" << belval.second << std::endl;
                        }
                    }
                }
              }
               std::cout << "*******************************" << std::endl;
            }
        }

    int n = pomdp_states.size(); //number of states
    int ns = 0; //max num of successors for any (s,a)
    int m = problem->actions().size(); //number of actions
    int z = 2; //number of observations - yes and no.
    int rz = pg.size(); // max number of non-zero beliefs.
    int r = pow(2,pg.size())-1;// max number of unique beliefs
    double gamma = 1;
    int horizon = 100;
    int s0;

    //generating transitions:
    double T[pomdp_states.size()][problem->actions().size()][pomdp_states.size()];
    double C[pomdp_states.size()][problem->actions().size()];
    for (int i = 0; i < pomdp_states.size(); i++)
    {
        std::pair<std::pair<int,int>,std::pair<int,int>> loc1  = pomdp_states.at(i);
        for(Action* a: problem->actions()){
            double cost = getCost(loc1,a,problem);
            if(cost > 0)
                C[i][a->hashValue()] = -1 * cost;
            else
                C[i][a->hashValue()] = cost;
            for (int j = 0; j < pomdp_states.size(); j++){
                std::pair<std::pair<int,int>,std::pair<int,int>> loc2  = pomdp_states.at(j);
                double trans = getTransition(loc1, loc2, a, problem);
                T[i][a->hashValue()][j] = trans;
            }
        }
    }
    int ns = 0;
    for (int i = 0; i < pomdp_states.size(); i++)
    {
      for (int a = 0 ; a < problem->actions().size(); a++)
       {
          int check = 0;
          for( int j = 0; j < pomdp_states.size(); j++)
          {
            if(T[i][a][j] > 0)
                check++;
          }
          if(check > ns_check)
            ns = check;
        }
    }
    std::cout << ns_check << std::endl;

    int trueGoalId = getGoalId(problem, pomdp_states);
    std::cout << "Total pomdp states = " << pomdp_states.size() << std::endl;
    std::cout << "num bel = " << numbel << std::endl;
    std::cout << " true goal at: " << trueGoalId << std::endl;
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
    double initPlanTime = 0.0;
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
           if (algorithm != "greedy") {
                solver->solve(problem->initialState());
            }
            endTime = clock();
            double planTime = (double(endTime - startTime) / CLOCKS_PER_SEC);
            totalTime += planTime;
            initPlanTime += planTime;
            longestTime = std::max(longestTime, planTime);
            numDecisions++;
        }

        State* tmp = problem->initialState();
        if(verbosity >= 10){
            experimentLogs();
            std::cout << "generated ROS compatible policy " << std::endl;
        }


       double costTrial = 0.0;
        int plausTrial = 0;
        while (!problem->goal(tmp)) {
            statesSeen.insert(tmp);
            Action* a;
         if (a == nullptr) {
                startTime = clock();
                a = solver->solve(tmp);
                endTime = clock();
                totalTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
                }
          else
                a = greedyAction(problem, tmp);

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
            cnt++;
            updateStatistics(costTrial, cnt, expectedCost, variance);
    }

    /** Adding time taken for heuristic computation to planning time-- if heuristic
    ** was pre-computed as in hmin-solve-all
    **/
//        totalTime += heuristic_time;
    if (verbosity >= 1) {
        cout << "Estimated cost " << problem->initialState()->cost() << " ";
        cout << "Avg. Exec cost " << expectedCost << " ";
        cout << "Std. Dev. " << sqrt(variance / (cnt - 1)) << " ";
        cout << "Plan time(w/o replan) " << (initPlanTime) << " " ; // time in seconds
        if(!OptimalSolver(algorithm)) //optimal solver computes plan only once.
            totalTime = totalTime/cnt;

        cout << "Total time " << (totalTime + heuristic_time) << " " << endl; // time in seconds
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

     if (flag_is_registered_with_value("rocksample")) {
        setupRockSample();
    }
    if (!flag_is_registered("dont-generate"))
        problem->generateAll();

    if(verbosity >= 10)
        cout << "Start State: " << problem->initialState() << std::endl;
    cout << " states = " << problem->states().size() << "  actions = " <<  problem->actions().size() << endl;

    cout << "****************Trans:" << endl;

                                                                                                        generateRawPOMDP(problem);

     if (flag_is_registered_with_value("heuristic")) {
        if (flag_value("heuristic") == "hmin" || flag_value("heuristic") == "hmin-solve-all" ) {
            clock_t startTime = clock();
       //     bool solveAll = flag_is_registered("hmin-solve-all");
             bool solveAll = (flag_value("heuristic") == "hmin-solve-all")? true: false;
             heuristic = new HMinHeuristic(problem, solveAll);

           clock_t endTime = clock();
           if (solveAll)
                heuristic_time =  (double(endTime - startTime) / CLOCKS_PER_SEC);

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
                    simulate(solver, alg_item, nsims, heuristic_time, t, perReplan);
                updateStatistics(results[0], i, avgCost, M2Cost);
                updateStatistics(results[2], i, avgTime, M2Time);
                }

            if (maxTime == -1)
                break;
        }
        delete solver;
    }
    delete problem;
    delete heuristic;
 }


