#ifndef MDPLIB_FFREDUCEDMODELSOLVER_H
#define MDPLIB_FFREDUCEDMODELSOLVER_H

#include <string>

#include "../ppddl/PPDDLState.h"

#include "../Action.h"
#include "../Problem.h"

#include "Solver.h"


namespace mlsolvers
{

/**
 * Implements a re-plannner that integrates FF-replan with the
 * reduced models framework. The full transition model is used up
 * to a given horizon. After that a deterministization model of the
 * problem is solved using the FF planner, available at
 *     https://fai.cs.uni-saarland.de/hoffmann/ff.html
 *
 * The determinization to be used is specified by passing the filename
 * of a PPDDL domain containing the determinization to be used.
 * The planner does not check that this is in fact a determinization
 * of the original problem, this responsibility is left to the user.
 */
class FFReducedModelSolver : public Solver
{
private:
    ////////////////////////////////////////////////////////////////////////////
    //                               VARIABLES                                //
    ////////////////////////////////////////////////////////////////////////////

    /* The problem to solve. */
    mlcore::Problem* problem_;

    /* The file where FF is to be called at. */
    std::string ffExecFilename_;

    /* The file name where the determinized PPDDL domain is defined. */
    std::string determinizedDomainFilename_;

    /*
     * The file name with the template used to create the problems to be solved
     * by FF.
     */
    std::string templateProblemFilename_;

    /* The file name of the updated problems solved by FF. */
    const std::string currentProblemFilename_ = "/tmp/fftmpfile";

    /* A map to store the actions generated by FF. */
    mlcore::StateActionMap ffStateActions_;

    /* A map to store the costs estimated by FF. */
    mlcore::StateIntMap ffStateCosts_;

    /*
     * A data structure to store the costs estimated by this planner. Each
     * vector entry correspond to a different horizon. For each horizon,
     * a map from state to costs is stored.
     */
    std::vector< mlcore::StateDoubleMap > estimatedCosts_;

    /* The maximum number of outcomes before switching to the reduced model. */
    int maxHorizon_;

    ////////////////////////////////////////////////////////////////////////////
    //                               FUNCTIONS                                //
    ////////////////////////////////////////////////////////////////////////////

    /*
     * Returns a string with the predicates in the given state.
     */
    std::string extractStatePredicates(mlppddl::PPDDLState* state);

    /*
     * Replaces the initial state in the problem file with the current state
     * and creates a new file with the new state.
     */
    void replaceInitStateInProblemFile(std::string currentStatePredicates);

    /*
     * Calls the FF planner and gets a cost and action estimate for
     * the current state. This function assumes that a PDDL file has already
     * been set up for FF corresponding to the state to be solved.
     * This is done by replaceInitStateInProblemFile()0.
     */
    std::pair<std::string, int> getActionNameAndCostFromFF();

    /*
     * Returns the action with the given name.
     */
    mlcore::Action* getActionFromName(std::string actionName);

    /* Returns an action greedily on estimatedCosts_. */
    mlcore::Action* greedyAction_(mlcore::State* s, int horizon);

    /* Returns the Q-Value of the the state and action at the given horizon.*/
    double qValue_(mlcore::State* s, mlcore::Action* a, int horizon);

    /*
     * Performs a dynamic programmin algorithm (AO*) to find the optimal
     * action for the given state and the given horizon. For horizon = 0,
     * this function invokes the FF planner.
     * The return value is the max residual observed while traversing the
     * sub-tree.
     * The last parameter is used to store if this state is a dead-end, which
     * happens if the best policy has a zero probability of reaching the goal.
     */
    double solve(mlcore::State* s, int horizon, bool& isDeadEnd);


public:
    FFReducedModelSolver(mlcore::Problem* problem,
                         std::string ffExecFilename,
                         std::string determinizedDomainFilename,
                         std::string templateProblemFilename,
                         int maxHorizon) :
        problem_(problem),
        ffExecFilename_(ffExecFilename),
        determinizedDomainFilename_(determinizedDomainFilename),
        templateProblemFilename_(templateProblemFilename),
        maxHorizon_(maxHorizon)
    {
        for (int i = 0; i <= maxHorizon_; i++) {
            estimatedCosts_.push_back(mlcore::StateDoubleMap());
        }
    }

    virtual ~FFReducedModelSolver() {}

    void maxHorizon(int value) { maxHorizon_ = value; }

    /**
     * Fids the best action for the given state. If the state is a
     * dead-end according to the given determinization, the method
     * returns a nullptr.
     */
    virtual mlcore::Action* solve(mlcore::State* s0);


};

}

#endif // MDPLIB_FFREDUCEDMODELSOLVER_H
