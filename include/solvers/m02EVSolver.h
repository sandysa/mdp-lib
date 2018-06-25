#ifndef MDPLIB_m02EVSOLVER_H
#define MDPLIB_m02EVSOLVER_H


#include <ctime>
#include <unordered_map>
#include <unordered_set>

#include "Solver.h"
namespace mlsolvers
{

/**
 * A SSP solver using the LAO* algorithm with Mkl reduction for EV only! For other domains, refer test/testReduced.cpp
 * k=0, l= 2.
 *
 */
class m02EVSolver : public Solver
{
private:
    mlcore::Problem* problem_;
    mlcore::StateSet visited;

    /* Error tolerance */
    double epsilon_ = 1.0e-6;

    /* Weight for the Bellman backup */
    double weight_ = 1.0;

    /* Time limit for LAO* in milliseconds */
    int timeLimit_ = 1000000;

    /*
     * Expands the BPSG rooted at state s and returns the
     * number of states expanded.
     */
    int expand(mlcore::State* s);

    /* Test if the BPSG rooted at state s has converged */
    double testConvergence(mlcore::State* s);

      /* returns the modified transitions with 2 primary outcomes*/
    std::list<mlcore::Successor> getTransition(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);


public:
    /**
     * Creates a m02* solver for the given problem.
     *
     * @param problem The problem to be solved.
     * @param epsilon The error tolerance wanted for the solution.
     * @param timeLimit The maximum time allowed for running the algorithm.
     * @param weight The weight for the Bellman backup.
     */
    m02EVSolver(mlcore::Problem* problem,  double epsilon = 1.0e-6,
                  int timeLimit = 1000000, double weight = 1.0)
        : problem_(problem), epsilon_(epsilon),
          timeLimit_(timeLimit), weight_(weight) { }

    /**
     * Solves the associated problem using the LAO* algorithm.
     *
     * @param s0 The state to start the search at.
     */
    virtual mlcore::Action* solve(mlcore::State* s0);

        /**
        * Over rides methods from solver
        */

    virtual mlcore::Action* m02_greedyAction(mlcore::Problem* problem, mlcore::State* s);


    virtual double m02_qvalue(mlcore::Problem* problem, mlcore::State* s, mlcore::Action* a);

    virtual std::pair<double, mlcore::Action*> m02_bellmanBackup(mlcore::Problem* problem,
                                                 mlcore::State* s);

    virtual double m02_bellmanUpdate(mlcore::Problem* problem, mlcore::State* s);
};

}

#endif // MDPLIB_m02EVSOLVER_H
