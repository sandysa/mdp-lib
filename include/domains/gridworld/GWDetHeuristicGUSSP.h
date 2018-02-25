#ifndef MDPLIB_GWDETHEURGUSSP_H
#define MDPLIB_GWDETHEURGUSSP_H

#include "../../util/general.h"

#include "../../Heuristic.h"
#include "../../State.h"

#include "GridWorldProblem.h"
#include "GridWorldState.h"


class GWDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    GridWorldProblem* problem_;

public:
     GWDetHeuristicGUSSP(GridWorldProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        GridWorldState* gws = (GridWorldState*) s;
        if (gws->x() == -1) // absorbing dummy state
            return 0;

        double cost_ = mdplib::dead_end_cost;
        for (auto const& element : problem_->potential_goals_prob) {
            std::pair<int,int> goal = element.first;
            double value = 0.0; // all goals have value 0.
            if (gws->x() == goal.first && gws->y() == goal.second)
                return value;
            double md =
                abs(gws->x() - goal.first) + abs(gws->y() - goal.second);

                /** multiply cost by the probability of it being a goal**/
            double goalCost = (element.second) * (problem_->getactioncost() * md + value);
            if (goalCost < cost_)
                cost_ = goalCost;
        }
//    std::cout << " h for state : " << gws << "  = " << cost_ << std::endl;
    return cost_;

    }
};

#endif // MDPLIB_GWMANHATTANHEUR_H
