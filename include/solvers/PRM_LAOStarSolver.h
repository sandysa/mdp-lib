#ifndef MDPLIB_PRM_LAOSTARSOLVER_H
#define MDPLIB_PRM_LAOSTARSOLVER_H


#include <ctime>
#include <unordered_map>
#include <unordered_set>

#include "Solver.h"
namespace mlsolvers
{

/**
 * A SSPP solver using the LAO* algorithm with 0/1 RM transitions.
 *
 */
class PRM_LAOStarSolver : public Solver
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

    /* Returns true if full model should be used for a (s,a) pair. Returns false otherwise. */
 //   bool isFullModel(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);

    /* returns the full model or deterministic transitions */
    std::list<mlcore::Successor> getTransition(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);

    bool costAdjusted_ = false;


public:
    /**
     * Creates a PRM_LAO* solver for the given problem.
     *
     * @param problem The problem to be solved.
     * @param epsilon The error tolerance wanted for the solution.
     * @param timeLimit The maximum time allowed for running the algorithm.
     * @param weight The weight for the Bellman backup.
     */
    PRM_LAOStarSolver(mlcore::Problem* problem, bool costAdjusted,
                  double epsilon = 1.0e-6,
                  int timeLimit = 1000000, double weight = 1.0)
        : problem_(problem), costAdjusted_(costAdjusted), epsilon_(epsilon),
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

    virtual mlcore::Action* PRM_greedyAction(mlcore::Problem* problem, mlcore::State* s);

     /**
        * Over rides methods from problem.h and returns the cost depending on whether cost adjustment is used
        */

    virtual double getCost(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);
    /* Returns true if full model should be used for a (s,a) pair. Returns false otherwise. */
    virtual bool useFullModel(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem);

    virtual double PRM_qvalue(mlcore::Problem* problem, mlcore::State* s, mlcore::Action* a);

    virtual std::pair<double, mlcore::Action*> PRM_bellmanBackup(mlcore::Problem* problem,
                                                 mlcore::State* s);

    virtual double PRM_bellmanUpdate(mlcore::Problem* problem, mlcore::State* s);

    bool costAdjusted(){return costAdjusted_;}

      //state action pair with a full model
    /** to calculate full model usage **/
    std::unordered_map<mlcore::State*, std::list<int>, mlcore::StateHash, mlcore::StateEqual> FullModelMap_;
};

}

#endif // MDPLIB_PRM_LAOSTARSOLVER_H
