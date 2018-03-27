#ifndef MDPLIB_COSTADJUSTED_DETERMINISTICSOLVER_H
#define MDPLIB_COSTADJUSTED_DETERMINISTICSOLVER_H

//#include "Solver.h"
#include "DeterministicSolver.h"
//#include "PRM_LAOStarSolver.h"
namespace mlsolvers
{
  //  const int det_most_likely = 0;

/**
 * Implements a deterministic solver based on the A* algorithm. It simplifies
 * the MDP's transition function by picking a single outcome, resulting in
 * a determinisitic version of the original problem.
 *
 * The choice of determinisitc outcome can be specified in the constructor.
 * Also, use3s cost adjustments, which can be specified by setting the value to "true" in the constructor.
 */
class CostAdjusted_DeterministicSolver : public DeterministicSolver
{
private:

    /*
     * Indicates the choice of deterministic outcome
     * (e.g. most-likely, least-likely).
     */
    int choice_ = det_most_likely;

    /*
     * The heuristic to use.
     */
    mlcore::Heuristic* heuristic_ = nullptr;

    /* The problem to solve. */
    mlcore::Problem* problem_ = nullptr;

    /* True if adjusted costs should be used. */
    bool costadjusted_ = false;

public:

    CostAdjusted_DeterministicSolver() {}

    /**
     * Creates a deterministic solver with the given choice for determinization
     * and heuristic to use for A*.
     *
     * Available options the determinization are:
     *
     *      - mlsolvers::det_most_likely (Most likely outcome)
     */
    CostAdjusted_DeterministicSolver(mlcore::Problem* problem, bool costadjusted, int choice = det_most_likely,
                        mlcore::Heuristic* heuristic = nullptr)
        : problem_(problem), costadjusted_(costadjusted), choice_(choice), heuristic_(heuristic) {}


    virtual ~CostAdjusted_DeterministicSolver() {}

    /**
     * Solves the associated problem using the LAO* algorithm.
     *
     * @param s0 The state to start the search at.
     */
    virtual mlcore::Action* solve(mlcore::State* s0);

    bool Iscostadjusted() {return costadjusted_;}

    /** returns adjusted cost **/
    virtual double getAdjustedCost(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);
};

}
#endif // MDPLIB_DETERMINISTICSOLVER_H

