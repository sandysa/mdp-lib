#include "../../include/solvers/Solver.h"
#include "../../include/Heuristic.h"
#include "../../include/solvers/m02EVSolver.h"

#include "../../include/domains/EV/EVProblem.h"

#include "../../include/util/general.h"

#include <ctime>
#include <queue>
#include<algorithm>

using namespace EV;
using namespace mlcore;
namespace mlsolvers
{

std::list<mlcore::Successor> m02EVSolver::getTransition(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem)
{
   std::list<mlcore::Successor> successors;
   if (!problem->applicable(s, a))
         return successors;

    successors = problem->transition(s,a);
    double maxprob = 0.0;
    int first_hash  = -1;
    int second_hash = -1;


    std::list<mlcore::Successor> primarysuccessors;
    for (const mlcore::Successor& su : successors ) {
        if(su.su_prob > maxprob)
        {
            maxprob = su.su_prob;
            first_hash = su.su_state->hashValue();
            primarysuccessors.clear();
            primarysuccessors.push_back(mlcore::Successor(su.su_state, 0.5));
        }
    }
    if(successors.size() < 2)
        return primarysuccessors;

    maxprob = 0.0;
    mlcore::State* secondarystate;
    for (const mlcore::Successor& su : successors) {
        if(su.su_prob > maxprob && su.su_state->hashValue() != first_hash)
        {
            maxprob = su.su_prob;
            second_hash = su.su_state->hashValue();
            secondarystate = su.su_state;
        }
    }
     primarysuccessors.push_back(mlcore::Successor(secondarystate, 0.5));
    return primarysuccessors;
}



double m02EVSolver::m02_qvalue(mlcore::Problem* problem, mlcore::State* s, mlcore::Action* a)
{
    double qAction = 0.0;
    for (const mlcore::Successor& su : getTransition(s, a,problem)) {
        qAction += su.su_prob * su.su_state->cost();
    }
    qAction = (qAction * problem->gamma()) + problem->cost(s, a);
    return qAction;
}

std::pair<double, mlcore::Action*> m02EVSolver::m02_bellmanBackup(mlcore::Problem* problem,
                                                 mlcore::State* s)
{
    double bestQ = problem->goal(s) ? 0.0 : mdplib::dead_end_cost;
    bool hasAction = false;
    mlcore::Action* bestAction = nullptr;
    for (mlcore::Action* a : problem->actions()) {
        if (!problem->applicable(s, a))
             continue;
        hasAction = true;
        double qAction = std::min(mdplib::dead_end_cost, m02_qvalue(problem, s, a));
        if (qAction <= bestQ) {
            bestQ = qAction;
            bestAction = a;
        }
    }

    if (!hasAction && bestQ >= mdplib::dead_end_cost)
        s->markDeadEnd();

     return std::make_pair(bestQ, bestAction);
}
double m02EVSolver::m02_bellmanUpdate(mlcore::Problem* problem, mlcore::State* s)
{
    std::pair<double, mlcore::Action*> best = m02_bellmanBackup(problem, s);
    double residual = s->cost() - best.bb_cost;
    bellman_mutex.lock();
    s->setCost(best.bb_cost);
    s->setBestAction(best.bb_action);
    bellman_mutex.unlock();
    return fabs(residual);
}

mlcore::Action* m02EVSolver::solve(mlcore::State* s0)
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

int m02EVSolver::expand(mlcore::State* s)
{
    if (!visited.insert(s).second)  // state was already visited.
        return 0;
    if (s->deadEnd() || problem_->goal(s))
        return 0;

    int cnt = 0;
    if (s->bestAction() == nullptr) {
        // state has not been expanded.
        m02_bellmanUpdate(problem_, s);
        return 1;
    } else {
        mlcore::Action* a = s->bestAction();
     //   for (mlcore::Successor sccr : problem_->transition(s, a))
            for(mlcore::Successor sccr : getTransition(s,a,problem_))
            cnt += expand(sccr.su_state);
    }
    m02_bellmanUpdate(problem_, s);
    return cnt;
}
double m02EVSolver::testConvergence(mlcore::State* s)
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

    error = std::max(error, m02_bellmanUpdate(problem_, s));
    if (prevAction == s->bestAction())
        return error;
    // it hasn't converged because the best action changed.
    return mdplib::dead_end_cost + 1;
}
mlcore::Action* m02EVSolver::m02_greedyAction(mlcore::Problem* problem, mlcore::State* s)
{
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
        double qAction = std::min(mdplib::dead_end_cost, m02_qvalue(problem, s, a));
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
