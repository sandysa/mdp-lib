#ifndef MDPLIB_GWDETHEURGUSSP_H
#define MDPLIB_GWDETHEURGUSSP_H

#include <vector>
#include <math.h>
#include "../../util/general.h"

#include "../../Heuristic.h"
#include "../../State.h"

#include "GUSSPGridWorldState.h"
#include "GUSSPGridWorldProblem.h"

class GWDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    GUSSPGridWorldProblem* problem_;

public:
     GWDetHeuristicGUSSP(GUSSPGridWorldProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        GUSSPGridWorldState* gws = (GUSSPGridWorldState*) s;
        if (s == problem_->absorbing)
            return 0;
        double cost_ = mdplib::dead_end_cost;
        double value = 0.0; // all goals have value 0.
        for (auto const& element : gws->goalPos()) {
            std::pair<int,int> loc = element.first;
            double md = abs (loc.first -  gws->x()) +
                        abs(loc.second - gws->y());
             /** multiply cost by the probability of it not being a goal**/
            double goalCost = std::round(1-element.second) * (problem_->getactioncost() * md + value);
            if (goalCost < cost_)
                cost_ = goalCost;
            }
    return cost_;
    }
};

#endif // MDPLIB_GWMANHATTANHEUR_H
