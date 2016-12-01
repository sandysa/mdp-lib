#ifndef MDPLIB_PROBLEM_H
#define MDPLIB_PROBLEM_H

#include <list>

#include "State.h"
#include "Action.h"
#include "Heuristic.h"

#include "util/general.h"

namespace mlcore
{
/**
 * An abstract class for Stochastic Shortest Path Problem objects.
 *
 * This class provides pure virtual methods used to implement goal tests,
 * transition functions, cost functions and applicable action tests for a SSP.
 * It also maintains the following information: initial state, discount factor
 * (gamma), list of available actions and an heuristic for the problem.
 *
 * This class also provides an internal set for storing all states that have
 * been generated so far. The method 'addState' is used to populate this set,
 * and it is recommended that classes that inherit from Problem use this method
 * whenever a state is generated by the transition function. This ensures that
 * no duplicate states will be kept in memory. Moreover, the provided
 * destructor will take care of cleaning up all generated states.
 */
class Problem
{
protected:
    /**
     * The initial state for the problem.
     */
    State* s0;

    /**
     * Discount factor.
     */
    double gamma_;
    /**
     * A list of all the possible actions in this problem.
     */
    std::list<Action *> actions_;

    /**
     * An internal structure to store all states that are generated by
     * calls to the transition function.
     */
    StateSet states_;

    /**
     * A heuristic that estimates the cost to reach a goal from any state.
     */
    Heuristic* heuristic_;

public:

    /**
     * Common constructor for initializing gamma and the heuristic.
     */
    Problem() : gamma_(1.0), heuristic_(nullptr) {}

    /**
     * Common destructor. Destroys all stored states and all actions.
     */
    virtual ~Problem()
    {
        for (State* state : states_)
            delete (state);
        for (Action* action : actions_)
            delete (action);
    }

    /**
     * Goal check.
     *
     * Checks if the state given as parameter is a goal or not.
     *
     * @return true if the given state is a goal.
     */
    virtual bool goal(State* s) const =0;

    /**
     * Transition function for the problem.
     *
     * Returns a list with all successors of the given state when
     * the given action is applied.
     *
     * @return A list of succcessors of the given state after applying the
     *        given action.
     */
    virtual std::list<Successor> transition(State* s, Action *a) =0;

    /**
     * Cost function for the problem.
     *
     * Returns the cost of applying the given action to the given state.
     *
     * @return The cost of applying action the given action to the given state.
     */
    virtual double cost(State* s, Action *a) const =0;

     virtual bool applicable(State* s, Action *a) const =0;

    /**
     * Initial state for this problem.
     *
     * @return The initial state of this problem.
     */
    virtual State* initialState() const
    {
        return s0;
    }

    /**
    * Generates all states that can be reached from s0 and stores them.
    *
    * WARNING: This method changes the value of the 'bits' variable for all
    * states in the state set. Do not call this method while other code is using
    * these values.
    */
    void generateAll()
    {
        for (State* state: states_)
            state->clearBits(mdplib::VISITED);
        std::list<State *> queue;
        queue.push_front(s0);
        while (!queue.empty()) {
            State* cur = queue.front();
            queue.pop_front();
            if (cur->checkBits(mdplib::VISITED))
                continue;
            cur->setBits(mdplib::VISITED);
            for (Action* a : actions_) {
                if (!applicable(cur, a))
                    continue;
                std::list<Successor> successors = transition(cur, a);
                for (Successor sccr : successors) {
                    queue.push_front(sccr.su_state);
                }
            }
        }
    }

    /**
     * If a state equal to the given state has already been stored,
     * it returns the expanded state. Otherwise, it stores the state first
     * and the returns it.
     *
     * This method relies on the equals() and hashValue() methods of the
     * State class.
     *
     * @param s A state used as a model of an internal state to be returned.
     * @return The state stored internally that equals the given state.
     */
    State* addState(State* s)
    {
        bool check = states_.insert(s).second;
        State* ret = *states_.find(s);
        // If the the state was found but the object representing it in
        // memory is different to the given one, delete the given one.
        if ((void *) ret != (void *) s && !check) {
            delete s;
        }
        return ret;
    }

    /**
     * Returns a state stored that is equal to the given state, if such a state
     * has been stored before. Otherwise, it returns a nullptr.
     */
    State* getState(State* s)
    {
        if (states_.count(s) > 0)
            return *states_.find(s);
        return nullptr;
    }


    /**
     * Returns the set containing all states generated so far.
     *
     * @return The states generated so far.
     */
    virtual StateSet& states()
    {
        return states_;
    }

    /**
     * Returns the list of actions in this problem.
     */
    std::list<Action*>& actions()
    {
        return actions_;
    }

    /**
     * Returns the discount factor.
     */
    double gamma()
    {
        return gamma_;
    }

    /**
     * Sets the discount factor.
     */
    void gamma(double g) { gamma_ = g; }

    /**
     * Returns the heuristic used for this problem.
     */
    Heuristic* heuristic()
    {
        return heuristic_;
    }

    /**
     Sets the heuristic to use for this problem.
     */
    void setHeuristic(Heuristic* heuristic)
    {
        heuristic_ = heuristic;
    }
};

}


#endif // MDPLIB_PROBLEM_H
