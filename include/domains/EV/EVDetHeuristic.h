#ifndef MDPLIB_EVDETHEURISTIC_H
#define MDPLIB_EVDETHEURISTIC_H

#include "../../Heuristic.h"

#include "EVProblem.h"

/**
 * A deterministic heuristic for the EV domain.
 * The heuristic assumes that for any action, the intended outcome
 * will occur with probability 1.0; this results in an admissible heuristic.
 */
class EVDetHeuristic : public mlcore::Heuristic
{
private:
    EVProblem* detProblem_;
public:
    EVDetHeuristic(int start_soc, int end_soc, int start_time, int end_time, int reward); //fixing reward case to 1
    virtual ~EVDetHeuristic()
    {
        delete detProblem_;
    }

    virtual double cost(const mlcore::State* s);
};

#endif // MDPLIB_RTRACKDETHEURISTIC_H
