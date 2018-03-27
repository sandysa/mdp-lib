#include <queue>

#include "../../include/solvers/CostAdjusted_DeterministicSolver.h"
#include "../../include/util/general.h"
#include "../../include/domains/racetrack/RacetrackProblem.h"
#include "../../include/domains/sailing/SailingProblem.h"
#include "../../include/domains/EV/EVProblem.h"
using namespace rtrack;

namespace mlsolvers
{
double CostAdjusted_DeterministicSolver::getAdjustedCost(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem)
{
   if(problem->getProblemName() == "racetrack")
   {
        RacetrackState* rts = static_cast<RacetrackState*>(s);
        RacetrackState* next = static_cast<RacetrackState*>(mostLikelyOutcome(problem, s, a));
        std::vector<std::vector <char> > track = ((RacetrackProblem*) problem)->track();

        if (track[next->x()][next->y()] == rtrack::wall || track[next->x()][next->y()] == rtrack::pothole )
           return 11;

        if(next->hValue() > s->hValue())
            return ((next->hValue() - s->hValue())*.5);
    }
    if(problem->getProblemName() == "sailing")
    {
        SailingState* state = static_cast<SailingState*> (s);
        SailingAction* action = static_cast<SailingAction*> (a);
       SailingState* next = static_cast<SailingState*>(mostLikelyOutcome(problem, state, action));

        if (((SailingProblem*) problem)->get_tack(state, action) != 4) //"INTO " in the problem is equal to 4.
              return 5;

        if(next->hValue() < s->hValue())
            return 0;
    }
    if(problem->getProblemName() == "ev")
    {
        if(problem->goal(s))
            return 0;

        EVState* evs = static_cast<EVState*>(s);
        EVAction*eva = static_cast<EVAction*>(a);
        EVProblem* evp = static_cast<EVProblem*>(problem);

       if(evs->exit_comm() == 3){
//                 /** encourage discharging in peak hour */
//               if(EV::horizon_- evs->timestep() >= 4 && evp->isPeak(evs->timestep()) && eva->level() == 4)
//                    return problem->cost(s,a)-2;
//
//               if(evs->timestep() >= EV::horizon_-2 && eva->level() == 5 && evs->soc() - EV::SPEED_L2 >= evp->end_soc())
//                     return -8;
//
//              if(evs->soc() - EV::SPEED_L1 >= evp->end_soc() && eva->level() == 4)// have enough charge to discharge at high speed.
//                   return -7;
//
              if(EV::horizon_- evs->timestep() >= 4 && eva->level() >= 1 && evp->isPeak(evs->timestep()))
                    return 10;
        }
        if(evs->exit_comm() <= 2){

                  if(eva->level() == 5 && evs->soc() - EV::SPEED_L2 >= evp->end_soc())
                        return problem->cost(s,a)-5;
                  if(evs->soc() - EV::SPEED_L1 >= evp->end_soc() && eva->level() == 4)// have enough charge to discharge at high speed.
                        return problem->cost(s,a)-2;
        }

    }
    return problem->cost(s,a);
}


mlcore::Action* CostAdjusted_DeterministicSolver::solve(mlcore::State* s0)
{
    NodeComparer comp();
    std::priority_queue<Node*, std::vector<Node*>, NodeComparer> frontier(comp);
    Node* init = new Node(nullptr, s0, nullptr, 0.0, heuristic_);
    frontier.push(init);
    std::list<Node*> allNodes;  // for memory clean-up later
    allNodes.push_back(init);
    Node* final = nullptr;
    while (!frontier.empty()) {
        Node* node = frontier.top();
        frontier.pop();

        if (node->state()->checkBits(mdplib::VISITED_ASTAR))
            continue;   // valid because this is using path-max
        node->state()->setBits(mdplib::VISITED_ASTAR);

        if (problem_->goal(node->state())) {
            final = node;
            break;
        }

        for (mlcore::Action* a : problem_->actions()) {
            if (!problem_->applicable(node->state(), a))
                continue;

            mlcore::State* nextState = nullptr;
            if (choice_ == det_most_likely)
                nextState = mostLikelyOutcome(problem_, node->state(), a);

            double cost = 0.0;
            if(Iscostadjusted())
                cost =  getAdjustedCost(node->state(), a, problem_);
            else

               cost = problem_->cost(node->state(), a);

            Node* next = new Node(node, nextState, a, cost, heuristic_, true);
            frontier.push(next);
            allNodes.push_back(next);
        }
    }

    mlcore::Action* optimal = nullptr;
    while (final->parent() != nullptr) {
        optimal = final->action();
        final = final->parent();
    }

    for (Node* node : allNodes) {
        node->state()->clearBits(mdplib::VISITED_ASTAR);
        delete node;
    }

    return optimal;
}

}
