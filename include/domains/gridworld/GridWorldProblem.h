#ifndef MPDLIB_GRIDWORLDPROBLEM_H
#define MPDLIB_GRIDWORLDPROBLEM_H

#include <unordered_set>
#include <map>
#include <cassert>
#include <utility>

#include "GridWorldState.h"

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

class GWManhattanHeuristic;

namespace gridworld
{
    const unsigned char UP = 0;
    const unsigned char DOWN = 1;
    const unsigned char LEFT = 2;
    const unsigned char RIGHT = 3;

    const int UNKNOWN = 2;
    const int TRUE = 1;
    const int FALSE = 0;
}

/**
 * A class representing a grid world problem as described
 * in AIAMA 3rd Edition, with some additional variants.
 *
 * Problems can be read from a file with the following notation:
 *
 *   - A '.' character represents an empty cell.
 *   - A 'x' character represents a wall (can't be traversed).
 *   - A '@' character represents a hole (can be traversed at a larger cost).
 *   - A 'D' character represents a dead-end.
 *   - A 'S' character represents the start cell.
 *   - A 'G' character represents a potential goal cell (multiple goals are allowed).
 */
class GridWorldProblem : public mlcore::Problem
{
    friend class GWManhattanHeuristic;

private:
    int width_;
    int height_;
    int x0_;
    int y0_;
    int status0_;
    double actionCost_;
    double holeCost_;
    bool allDirections_;
    PairDoubleMap* goals_;
    mlcore::State* absorbing;
    IntPairSet walls;
    IntPairSet holes;
    IntPairSet dead_ends;


    void addSuccessor(GridWorldState* state,
                      std::list<mlcore::Successor>& successors,
                      int val,
                      int limit,
                      int newx,
                      int newy,
                      int newstatus,
                      double prob);

    void addAllActions();

    bool gridGoal(GridWorldState* s) const;
public:
    /**
     * Default constructor.
     *
     * Creates a grid world with a single goal state (0,0)
     * and all-zero heuristic.
     */
    GridWorldProblem();
    std::vector<std::pair<int,int>> potential_goals;
    std::map<std::pair<int,int>, double> potential_goals_prob;
    /**
     * Constructs a grid world from a string file representation
     * stored at the given filename. The constructor also receives
     * the cost of the actions.
     */
    GridWorldProblem(const char* filename,
                     double actionCost = 1.0,
                     double holeCost = 100.0,
                     bool allDirections = false,
                     bool uniform_goal_dist = true);

    /**
     * Constructs a grid world with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
    GridWorldProblem(int width, int height, int x0, int y0, int status0,
                     PairDoubleMap* goals, double actionCost);


    /**
     * Constructs a grid world with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
    GridWorldProblem(int width, int height, int x0, int y0, int status0,
                     std::map<std::pair<int,int>, double> potential_goals_prob, double actionCost);


    /**
     * Constructs a grid world with the specified width, height,
     * goal states, initial state (x0,y0) and heuristic.
     * All states are equally probable to be a goal.
     */
    GridWorldProblem(int width, int height, int x0, int y0, int status0,
                     PairDoubleMap* goals, mlcore::Heuristic* h);

    ~GridWorldProblem()
    {
        delete goals_;
    }

    /**
     * Overrides method from Problem.
     */
    virtual bool goal(mlcore::State* s) const;

    /**
     * Overrides method from Problem.
     */
    virtual std::list<mlcore::Successor> transition(mlcore::State* s,
                                                    mlcore::Action* a);

    /**
     * Overrides method from Problem.
     */
    virtual double cost(mlcore::State* s, mlcore::Action* a) const;

    /**
     * Overrides method from Problem.
     */
    virtual bool applicable(mlcore::State* s, mlcore::Action* a) const;

    /** set the true goal in GUSSp
    ** this is set to generate observations accordingly.
    ** One state is randomly chosen from the set of potential goals and set as the true goal.
    **/
    virtual void setGoal(std::vector<std::pair<int,int>> potential_goals, bool uniform_goal_dist);

    /** Returns 1 if the state is a true goal. Returns 0 otherwise.
     **/
    virtual int getobservation(GridWorldState* s);

    /** Returns 1 if the state is a potential goal. Returns 0 otherwise.
     **/
    virtual bool isPotentialGoal(GridWorldState* s);

    double getactioncost(){return actionCost_;}
};

#endif // MPDLIB_GRIDWORLDPROBLEM_H
