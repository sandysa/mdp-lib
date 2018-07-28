#ifndef MPDLIB_SEARCHRESCUEPROBLEM_H
#define MPDLIB_SEARCHRESCUEPROBLEM_H

#include <unordered_set>
#include <map>
#include <cassert>
#include <utility>
#include <vector>

#include "SearchRescueState.h"
#include "SearchRescueAction.h"

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

class SRDetHeuristicGUSSP;

namespace searchrescue
{
    const unsigned char UP = 0;
    const unsigned char DOWN = 1;
    const unsigned char LEFT = 2;
    const unsigned char RIGHT = 3;
    const unsigned char SAVE = 4;

    const int UNKNOWN = 2;
    const int TRUE = 1;
    const int FALSE = 0;
}

/**
 * A class representing a search and rescue problem.
 * Problems can be read from a file with the following notation:
 *
 *   - A '.' character represents an empty cell.
 *   - A 'x' character represents a wall (can't be traversed).
 *   - A '@' character represents a hole (can be traversed at a larger cost).
 *   - A 'D' character represents a dead-end.
 *   - A 'S' character represents the start cell.
 *   - A 'G' character represents a potential goal cell (multiple goals are allowed).
 */
class SearchRescueProblem : public mlcore::Problem
{
    friend class SRDetHeuristicGUSSP;

private:
    int width_;
    int height_;
    int x0_;
    int y0_;
    int victims0_;
    int status0_;
    int maxVictims_;
    double actionCost_;
    double holeCost_;
    bool allDirections_;
    mlcore::State* absorbing;
    IntPairSet walls;
    IntPairSet holes;
    IntPairSet dead_ends;
    std::vector<std::pair<int,int>> clearedStates;

    void addSuccessor(SearchRescueState* state,
                      std::list<mlcore::Successor>& successors,
                      int val,
                      int limit,
                      int newx,
                      int newy,
                      int newvictims,
                      int newstatus,
                      double prob);

    void addAllActions();

    bool SearchRescueGoal(SearchRescueState* s) const;
public:
    /**
     * Default constructor.
     */
    SearchRescueProblem();
    std::vector<std::pair<int,int>> potential_goals;
    std::map<std::pair<int,int>, int> potential_goals_victims;
    /**
     * Constructs a search and rescue world from a string file representation
     * stored at the given filename. The constructor also receives
     * the cost of the actions.
     */
    SearchRescueProblem(const char* filename,
                     double actionCost = 1.0,
                     double holeCost = 100.0,
                     bool allDirections = false,
                     bool uniform_goal_dist = true);

    /**
     * Constructs a search and rescue with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
   SearchRescueProblem(int width, int height, int x0, int y0, int victims0, int status0,
                     double actionCost);


    /**
     * Constructs a search and rescue with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
    SearchRescueProblem(int width, int height, int x0, int y0, int victims0, int status0,
                     std::map<std::pair<int,int>, int> potential_goals_victims, double actionCost);


    /**
     * Constructs a search and rescue with the specified width, height,
     * goal states, initial state (x0,y0) and heuristic.
     * All states are equally probable to be a goal.
     */
   SearchRescueProblem(int width, int height, int x0, int y0, int victims, int status0,
                     mlcore::Heuristic* h);

    ~SearchRescueProblem()
    {
  //      delete goals_;
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
    virtual int getobservation(SearchRescueState* s);
    virtual int getobservation(int x, int y);

    /** Returns 1 if the state is a potential goal. Returns 0 otherwise.
     **/
    virtual bool isPotentialGoal(SearchRescueState* s);
    virtual bool isPotentialGoal(int x, int y);

    virtual bool SRGoal(SearchRescueState* s) const;

    double getactioncost(){return actionCost_;}

    int maxVictims() {return maxVictims_;}
};

#endif // MPDLIB_SEARCHRESCUEPROBLEM_H
