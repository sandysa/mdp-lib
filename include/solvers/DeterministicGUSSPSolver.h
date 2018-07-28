#ifndef MDPLIB_DETERMINISTICGUSSPSOLVER_H
#define MDPLIB_DETERMINISTICGUSSPSOLVER_H

#include "../Action.h"
#include "../State.h"

#include "Solver.h"

namespace mlsolvers
{

const int det_GUSSP_most_likely = 0;
const int det_GUSSP_closest = 1;

/**
 * Implements a deterministic solver for the GUSSP. It simplifies the problem
 * by considering and planning for one potential goal.
 * The goal selection mechanism (either closest or most likely) is specified in
 * the constructor.
 */
class DeterministicGUSSPSolver : public Solver
{
private:

    /**
     * Indicates the choice of deterministic outcome
     * (e.g. most-likely, closest). If there are ties in
     * most-likely, then ties are resolved in favor of the closest.
     * Similarly, if there are ties in the closest, ties are resolved
     * in favor of the more likely goal.
     * Default is most likely.
     **/
    int choice_ = det_GUSSP_most_likely;

    /*
     * The heuristic to use.
     */
    mlcore::Heuristic* heuristic_ = nullptr;

    /* The problem to solve. */
    mlcore::Problem* problem_ = nullptr;

    mlcore::State* temp_goal;



public:

    DeterministicGUSSPSolver() {}

    /**
     * Creates a deterministic solver with the given choice for determinization
     * and heuristic to use for solving using LAO*.
     *
     * Available options the determinization are:
     *
     *      - mlsolvers::det_GUSSP_most_likely (Most likely potential goal)
     *      - mlsolvers::det_GUSSP_closest (Closest potential goal)
     */
    DeterministicGUSSPSolver(mlcore::Problem* problem, int choice,
                        mlcore::Heuristic* heuristic = nullptr, )
        : problem_(problem), choice_(choice), heuristic_(heuristic) {}

    virtual ~DeterministicGUSSPSolver() {}

    /**
     * Solves the associated problem using the LAO* algorithm.
     *
     * @param s0 The state to start the search at.
     */
    virtual mlcore::Action* solve(mlcore::State* s0);

    void setgoal(mlcore::State* s){temp_goal  = s;}

    mlcore::State* getTempGoal(){return temp_goal;}

    mlcore::State* calculateTempGoal(int choice);


};

}
#endif // MDPLIB_DETERMINISTICGUSSPSOLVER_H
