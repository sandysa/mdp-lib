#ifndef MDPLIB_SRDETHEURGUSSP_H
#define MDPLIB_SRDETHEURGUSSP_H

#include "../../util/general.h"

#include "../../Heuristic.h"
#include "../../State.h"

#include "SearchRescueProblem.h"
#include "SearchRescueState.h"


class SRDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    SearchRescueProblem* problem_;

public:
     SRDetHeuristicGUSSP(SearchRescueProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        SearchRescueState* srs = (SearchRescueState*) s;
        if (srs->x() == -1) // absorbing dummy state
            return 0;
      //check if it saved all victims
      if (srs->victims() == problem_->maxVictims())
            return 0.0;

        double cost_ = mdplib::dead_end_cost;
        for (auto const& element : problem_->potential_goals_victims) {
            std::pair<int,int> goal = element.first;
            double value = 0.0; // all goals have value 0.
            double md =
                abs(srs->x() - goal.first) + abs(srs->y() - goal.second);

                /** multiply cost by the probability of it being a goal.
                ** always considers equal probability: 0.5 probability of being a goal
                **/
            double goalCost = 0.5 * (problem_->getactioncost() * md + value);
            if (goalCost < cost_)
                cost_ = goalCost;
        }
//    std::cout << " h for state : " << srs << "  = " << cost_ << std::endl;
    return cost_;

    }
};

#endif // MDPLIB_SRDETHEURGUSSP_H

