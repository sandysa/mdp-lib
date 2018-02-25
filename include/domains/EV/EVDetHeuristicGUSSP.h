#ifndef MDPLIB_EVDETHEURGUSSP_H
#define MDPLIB_EVDETHEURGUSSP_H

#include "../../util/general.h"

#include "../../Heuristic.h"
#include "../../State.h"

#include "EVProblem.h"
#include "EVState.h"

class EVDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    EVProblem* problem_;

public:
     EVDetHeuristicGUSSP(EVProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        EVState* evs = (EVState*) s;
        std::pair<int,int> pos (evs->x(),evs->y());
        if (evs->x() == -1) // absorbing dummy state
            return 0;
      //check if it saved all victims
      if (evs->sampledRocks() == problem_->maxRocks())
            return 0.0;

        double cost_ = mdplib::dead_end_cost;
        for (auto const& element : problem_->potential_goals_prob) {
            std::pair<int,int> goal = element.first;
            double value = 0.0; // all goals have value 0.
            double md =
                abs(evs->x() - goal.first) + abs(evs->y() - goal.second);

                /** multiply cost by the probability of it being a goal.
                ** always considers equal probability: 0.5 probability of being a goal
                **/
            double goalCost = (1-element.second) * (problem_->getactioncost() * md + value);
            if (goalCost < cost_)
                cost_ = goalCost;
        }
    return cost_;
    }
};

#endif // MDPLIB_RSDETHEURGUSSP_H
