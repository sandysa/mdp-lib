#ifndef MDPLIB_FLARESSOLVER_H
#define MDPLIB_FLARESSOLVER_H

#include "../Problem.h"
#include "../Heuristic.h"

#include "Solver.h"

namespace mlsolvers
{


/**
 * A SSP solver using the FLARES algorithm.
 */
class FLARESSolver : public Solver
{
private:
    /* The problem to solve. */
    mlcore::Problem* problem_;

    /* The maximum number of trials. */
    int maxTrials_;

    /* The error tolerance */
    double epsilon_;

    /* The max depth for the checkSolved procedure */
    double horizon_;

    /* If true the algorithm returns an optimal policy. */
    bool optimal_;

    /*
     * If true, the depth of states will be the log probability of reaching
     * the state. Otherwise, it is the number of steps.
     */
    bool useProbsForDepth_;

    /* Stores all states that have been solved to the latest horizon. */
    mlcore::StateSet depthSolved_;

    /* Performs a single trial */
    void trial(mlcore::State* s);

    /* Checks if the state has been solved. */
    bool checkSolved(mlcore::State* s);

    /* Checks if the state has already been labeled as solved. */
    bool labeledSolved(mlcore::State* s);

    /* Finds an optimal policy for the problem. */
    mlcore::Action* solveOptimally(mlcore::State* s0);

    /* Finds an approximate policy for the problem. */
    mlcore::Action* solveApproximate(mlcore::State* s0);

    /*
     * Computes the depth of the given successor state for the given depth of
     * its parent state.
     */
    double computeNewDepth(mlcore::Successor& su, double depth);

public:
    /**
     * Creates a FLARES solver for the given problem.
     *
     * @param problem The problem to be solved.
     * @param maxTrials The maximum number of trials to perform.
     * @param epsilon The error tolerance.
     * @param horizon_ The maximum depth for checkSolved.
     */
    FLARESSolver(mlcore::Problem* problem,
                 int maxTrials,
                 double epsilon,
                 double horizon_,
                 bool optimal = false,
                 bool useProbsForDepth = false);

    /**
     * Solves the associated problem using the Labeled RTDP algorithm.
     *
     * @param s0 The state to start the search at.
     */
    virtual mlcore::Action* solve(mlcore::State* s0);

    void cleanup() { depthSolved_.clear(); }
};

}

#endif // MDPLIB_FLARESSOLVER_H
