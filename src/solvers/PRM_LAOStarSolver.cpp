#include "../../include/solvers/Solver.h"
#include "../../include/Heuristic.h"
#include "../../include/solvers/PRM_LAOStarSolver.h"
#include "../../include/solvers/CostAdjusted_DeterministicSolver.h"

#include "../../include/domains/racetrack/RacetrackProblem.h"
#include "../../include/domains/sailing/SailingProblem.h"
#include "../../include/domains/EV/EVProblem.h"

#include "../../include/util/general.h"

#include <ctime>
#include <queue>
#include<algorithm>

using namespace EV;
using namespace rtrack;
using namespace mlcore;
namespace mlsolvers
{
double getCostAdjustmentValue(mlcore::State* s, mlcore::Action* a,mlcore::Problem* problem)
{
    if(problem->goal(s))
            return problem->cost(s,a);

    if(problem->getProblemName() == "sailing")
    {
         SailingState* state = static_cast<SailingState*> (s);
         SailingAction* action = static_cast<SailingAction*> (a);
         if (((SailingProblem*) problem)->get_tack(state, action) <= 1)
            return problem->cost(s,a) * 1.3; //1
         if (((SailingProblem*) problem)->get_tack(state, action) < 4)
            return problem->cost(s,a) * 1.2;
         if (((SailingProblem*) problem)->get_tack(state, action) == 4)
            return problem->cost(s,a) * 1.1; //1.3
    }
   else  if(problem->getProblemName() == "racetrack")
     {
        RacetrackState* rts = static_cast<RacetrackState*>(s);
        RacetrackAction* rta = static_cast<RacetrackAction*>(a);
        RacetrackState* next = new RacetrackState(rts->x()+ rta->ax(), rts->y() + rta->ay(),  rta->ax(),  rta->ay(), (RacetrackProblem*) problem);
        std::vector<std::vector <char> > track = ((RacetrackProblem*) problem)->track();
        double newcost = problem->cost(s,a) ;
        if (track[next->x()][next->y()] == rtrack::wall || track[next->x()][next->y()] == rtrack::pothole )
            newcost =  problem->cost(s,a)*2;

        else if (track[rts->x()][rts->y()] == rtrack::wall || track[rts->x()][rts->y()] == rtrack::pothole )
            newcost = problem->cost(s,a)*2;

        delete next;
        return newcost;
    }

   else if(problem->getProblemName() == "ev")
    {

        EVState* evs = static_cast<EVState*>(s);
        EVAction*eva = static_cast<EVAction*>(a);
        EVProblem* evp = static_cast<EVProblem*>(problem);

        /** discharging with low charge when about to leave **/
        if((evs->exit_comm() <= 2 || evs->timestep() >= EV::horizon_-2) && eva->level() > 3 && evs->soc() <= evp->end_soc())
            return 8; //8

        /** encourage discharging in peak hour */
        if(EV::horizon_- evs->timestep() >= 4 && evp->isPeak(evs->timestep()) && eva->level() > 3)
            return -10;
        /** enough charge to discharge **/
        if((evs->exit_comm() <= 2 || evs->timestep() >= EV::horizon_-2) && eva->level() == 5 && evs->soc() - EV::SPEED_L2 >= evp->end_soc())
            return -12;

        if(evs->soc() - EV::SPEED_L1 >= evp->end_soc() && eva->level() == 4)// have enough charge to discharge at high speed.
            return -8;

//        if(evs->exit_comm() <= 2 || evs->timestep() >=  EV::horizon_-3)
//            return problem->cost(s,a) * 1.5;
    }

    return problem->cost(s,a);

}


bool PRM_LAOStarSolver::useFullModel(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem){
    //For racetrack: (CA:100% in pRM comparison.ods)
   bool fullModel = false;
   if(problem->getProblemName() == "racetrack")
   {
    if (s == problem->initialState())
        return true;

    else if (((RacetrackProblem*) problem)->goal(s)  || s == ((RacetrackProblem*) problem)->absorbing())
        return true;
    else if(s->hValue() < 3 )
            return true;
    else if((getCostAdjustmentValue(s,a,problem)/ problem->cost(s,a)) > 1)
            return true;

    return false;
    }

    if(problem->getProblemName() == "sailing")
    {
         if (s == problem->initialState())
            return true;

        if((getCostAdjustmentValue(s,a,problem)/ problem->cost(s,a)) > 1.2) //< 1.2 (chnage costs)for results in AAMAS/PAL paper%
            return true;

        return false;
       }


    if(problem->getProblemName() == "ev")
    {
        EVState* evs = static_cast<EVState*>(s);
        if(evs->exit_comm() <= 2 || evs->timestep() >=  EV::horizon_-3)
            return true;
//    if((abs(getCostAdjustmentValue(s,a,problem))/ problem->cost(s,a)) >= .5)
            return true;
    }

    return false;
}

std::list<mlcore::Successor> PRM_LAOStarSolver::getTransition(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem)
{
   std::list<mlcore::Successor> successors;
   if (problem->applicable(s, a))
        successors = problem->transition(s,a);
    else
        return successors;

       if(PRM_LAOStarSolver::useFullModel(s,a,problem)){
              isFullModel_.insert(s);
              auto it = FullModelMap_.find(s);
              if( it != FullModelMap_.end() ) {
                    std::list<int> actionset = it->second;
                    if(std::find(actionset.begin(), actionset.end(), a->hashValue()) == actionset.end()){
                            actionset.push_back(a->hashValue());
                             it->second = actionset; //update map with new values
                    }
                }
             else {
                std::list<int> actionset;
                actionset.push_back(a->hashValue());
                FullModelMap_.insert(std::make_pair(s,actionset));
                }
              return successors;
        }

     mlcore::State* mostlikely = mostLikelyOutcome(problem, s,a, true);
     std::list<mlcore::Successor> mostlikelySuccessors ;
     mostlikelySuccessors.push_back(mlcore::Successor(mostlikely, 1.0));

     return mostlikelySuccessors;
}


double PRM_LAOStarSolver::getCost(mlcore::State* s, mlcore::Action* a,mlcore::Problem* problem)
{
    if(!costAdjusted_)
        return problem->cost(s,a);

    //if using full model
       auto it = FullModelMap_.find(s);
       if( it != FullModelMap_.end() ) {
           std::list<int> actionset = it->second;
           if(std::find(actionset.begin(), actionset.end(), a->hashValue()) != actionset.end()){
                return problem->cost(s,a);
           }
        }

    //else use adjusted cost
    return getCostAdjustmentValue(s,a,problem);

}

double PRM_LAOStarSolver::PRM_qvalue(mlcore::Problem* problem, mlcore::State* s, mlcore::Action* a)
{
    double qAction = 0.0;
    for (const mlcore::Successor& su : getTransition(s, a,problem)) {
        qAction += su.su_prob * su.su_state->cost();
    }
  //  qAction = (qAction * problem->gamma()) + problem->cost(s, a);
    qAction = (qAction * problem->gamma()) + getCost(s, a, problem);
    return qAction;
}

std::pair<double, mlcore::Action*> PRM_LAOStarSolver::PRM_bellmanBackup(mlcore::Problem* problem,
                                                 mlcore::State* s)
{
    double bestQ = problem->goal(s) ? 0.0 : mdplib::dead_end_cost;
    bool hasAction = false;
    mlcore::Action* bestAction = nullptr;
    for (mlcore::Action* a : problem->actions()) {
        if (!problem->applicable(s, a))
             continue;
        hasAction = true;
        double qAction = std::min(mdplib::dead_end_cost, PRM_qvalue(problem, s, a));
        if (qAction <= bestQ) {
            bestQ = qAction;
            bestAction = a;
        }
    }

    if (!hasAction && bestQ >= mdplib::dead_end_cost)
        s->markDeadEnd();


    return std::make_pair(bestQ, bestAction);
}
double PRM_LAOStarSolver::PRM_bellmanUpdate(mlcore::Problem* problem, mlcore::State* s)
{
    std::pair<double, mlcore::Action*> best = PRM_bellmanBackup(problem, s);
    double residual = s->cost() - best.bb_cost;
    bellman_mutex.lock();
    s->setCost(best.bb_cost);
    s->setBestAction(best.bb_action);
    bellman_mutex.unlock();
    return fabs(residual);
}

mlcore::Action* PRM_LAOStarSolver::solve(mlcore::State* s0)
{
    clock_t startTime = clock();
    int totalExpanded = 0;
    int countExpanded = 0;
    double error = mdplib::dead_end_cost;
    while (true) {
        do {
            visited.clear();
            countExpanded = expand(s0);
            totalExpanded += countExpanded;
            if ((0.001 * (clock() - startTime)) /
                    CLOCKS_PER_SEC > timeLimit_)
                return s0->bestAction();

        } while (countExpanded != 0);

        while (true) {
            if ((0.001 * (clock() - startTime)) /
                    CLOCKS_PER_SEC > timeLimit_)
                return s0->bestAction();

            visited.clear();
            error = testConvergence(s0);
            if (error < epsilon_)
                return s0->bestAction();
            if (error > mdplib::dead_end_cost) {
                break;  // BPSG changed, must expand tip nodes again
            }
        }
    }
}

int PRM_LAOStarSolver::expand(mlcore::State* s)
{
    if (!visited.insert(s).second)  // state was already visited.
        return 0;
    if (s->deadEnd() || problem_->goal(s))
        return 0;

    int cnt = 0;
    if (s->bestAction() == nullptr) {
        // state has not been expanded.
        PRM_bellmanUpdate(problem_, s);
        return 1;
    } else {
        mlcore::Action* a = s->bestAction();
     //   for (mlcore::Successor sccr : problem_->transition(s, a))
            for(mlcore::Successor sccr : getTransition(s,a,problem_))
            cnt += expand(sccr.su_state);
    }
    PRM_bellmanUpdate(problem_, s);
    return cnt;
}
double PRM_LAOStarSolver::testConvergence(mlcore::State* s)
{
    double error = 0.0;

    if (s->deadEnd() || problem_->goal(s))
        return 0.0;

    if (!visited.insert(s).second)
        return 0.0;

    mlcore::Action* prevAction = s->bestAction();
    if (prevAction == nullptr) {
        // if it reaches this point it hasn't converged yet.
        return mdplib::dead_end_cost + 1;
    } else {
       // for (mlcore::Successor sccr : problem_->transition(s, prevAction))
       for (mlcore::Successor sccr : getTransition(s, prevAction,problem_))
            error =  std::max(error, testConvergence(sccr.su_state));
    }

    error = std::max(error, PRM_bellmanUpdate(problem_, s));
    if (prevAction == s->bestAction())
        return error;
    // it hasn't converged because the best action changed.
    return mdplib::dead_end_cost + 1;
}
mlcore::Action* PRM_LAOStarSolver::PRM_greedyAction(mlcore::Problem* problem, mlcore::State* s)
{
    std::cout<< " hey~" << std::endl;
    if (s->bestAction() != nullptr)
        return s->bestAction();

    std::cout<< " best action is a null ptr--------" << std::endl;
    mlcore::Action* bestAction = nullptr;
    double bestQ = mdplib::dead_end_cost;
    bool hasAction = false;
    for (mlcore::Action* a : problem->actions()) {
        if (!problem->applicable(s, a))
      //    if(!isApplicable(s,a,problem))
            continue;
        hasAction = true;
        double qAction = std::min(mdplib::dead_end_cost, PRM_qvalue(problem, s, a));
        if (qAction <= bestQ) {
            bestQ = qAction;
            bestAction = a;
        }
    }
    if (!hasAction)
        s->markDeadEnd();

    return bestAction;
}
}
