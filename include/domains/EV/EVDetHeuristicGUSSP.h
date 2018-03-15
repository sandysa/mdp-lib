#ifndef MDPLIB_EVDETHEURISTICGUSSP_H
#define MDPLIB_EVDETHEURISTICGUSSP_H

#include "../../Heuristic.h"

#include "GUSSPEVProblem.h"

/**
 * A deterministic heuristic for the EV domain.
 * The heuristic assumes that for any action, the intended outcome
 * will occur with probability 1.0; this results in an admissible heuristic.
 */
class EVDetHeuristicGUSSP : public mlcore::Heuristic
{
private:
    GUSSPEVProblem* problem_;
public:
   EVDetHeuristicGUSSP(GUSSPEVProblem* problem)
    {
        problem_ = problem;
    }

  virtual double cost(const mlcore::State* s){

        GUSSPEVState* evs = (GUSSPEVState*) s;
        if (s == problem_->absorbing_)
            return 0;
        double cost_ = mdplib::dead_end_cost;
        std::vector<std::pair<int, double>> goalPos = evs->goalPos();

        double value = 0.0; // all goals have value 0-- assume it reaches the right charge level always
         for(int i = 0; i < goalPos.size(); ++i){
            std::pair<int, double> pos = goalPos[i];
            double md = pos.first - evs->timestep();
            if(md > 0) // consider only upcoming potential exit times.
            {
               /** multiply cost by the probability of it not being a goal**/
             //   double goalCost = std::round((1 - pos.second) * (problem_-> getlowestCost() * md + value));
                double goalCost = (problem_-> getlowestCost() * md + value);
                if (goalCost < cost_)
                    cost_ = goalCost;
            }
            }
    return cost_;
    }
};

#endif // MDPLIB_EVDETHEURISTICGUSSP_H
