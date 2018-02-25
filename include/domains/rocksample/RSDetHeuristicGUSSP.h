#ifndef MDPLIB_RSDETHEURGUSSP_H
#define MDPLIB_RSDETHEURGUSSP_H

#include "../../util/general.h"

#include "../../Heuristic.h"
#include "../../State.h"

#include "RockSampleProblem.h"
#include "RockSampleState.h"


class RSDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    RockSampleProblem* problem_;

public:
     RSDetHeuristicGUSSP(RockSampleProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        RockSampleState* rss = (RockSampleState*) s;
        std::pair<int,int> pos (rss->x(),rss->y());
        if (rss->x() == -1) // absorbing dummy state
            return 0;
      //check if it saved all victims
      if (rss->sampledRocks() == problem_->maxRocks())
            return 0.0;

        double cost_ = mdplib::dead_end_cost;
        for (auto const& element : problem_->potential_goals_prob) {
            std::pair<int,int> goal = element.first;
            double value = 0.0; // all goals have value 0.
            double md =
                abs(rss->x() - goal.first) + abs(rss->y() - goal.second);

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

