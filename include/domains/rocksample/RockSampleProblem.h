#ifndef MPDLIB_ROCKSAMPLEPROBLEM_H
#define MPDLIB_ROCKSAMPLEPROBLEM_H

#include <unordered_set>
#include <map>
#include <cassert>
#include <utility>
#include <vector>

#include "RockSampleState.h"
#include "RockSampleAction.h"

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

class RSDetHeuristicGUSSP;

namespace rocksample
{
    const unsigned char UP = 0;
    const unsigned char DOWN = 1;
    const unsigned char LEFT = 2;
    const unsigned char RIGHT = 3;
    const unsigned char SAMPLE = 4;

    const int UNKNOWN = 2;
    const int GOOD = 1;
    const int BAD = 0;
}

/**
 * A class representing a modified ROCKSAMPLEproblem to be an SSP. (https://arxiv.org/pdf/1207.4166.pdf)
 * Problems can be read from a file with the following notation:
 *
 *   - A '.' character represents an empty cell.
 *   - A 'x' character represents a wall (can't be traversed).
 *   - A '@' character represents a hole (can be traversed at a larger cost).
 *   - A 'D' character represents a dead-end.
 *   - A 'S' character represents the start cell.
 *   - A 'G' character represents a potential goal cell (multiple goals are allowed).
 */
class RockSampleProblem : public mlcore::Problem
{
    friend class RSDetHeuristicGUSSP;

private:
    int width_;
    int height_;
    int x0_;
    int y0_;
    int sampledRocks0_;
    int status0_;
    int maxRocks_;
    double actionCost_;
    double holeCost_;
    bool allDirections_;
    mlcore::State* absorbing;
    IntPairSet walls;
    IntPairSet holes;
    IntPairSet dead_ends;
    std::vector<std::pair<int,int>> clearedStates;
    PairDoubleMap* goals_;

    void addSuccessor(RockSampleState* state,
                      std::list<mlcore::Successor>& successors,
                      int val,
                      int limit,
                      int newx,
                      int newy,
                      int newsamples,
                      int newstatus,
                      double prob);

    void addAllActions();

    bool RockSampleGoal(RockSampleState* s) const;
public:
    /**
     * Default constructor.
     */
    RockSampleProblem();
    std::vector<std::pair<int,int>> potential_goals;
    std::map<std::pair<int,int>, double> potential_goals_prob;
    /**
     * Constructs a rocksample world from a string file representation
     * stored at the given filename. The constructor also receives
     * the cost of the actions.
     */
    RockSampleProblem(const char* filename,
                     double actionCost = 1.0,
                     double holeCost = 100.0,
                     bool allDirections = false,
                     bool uniform_goal_dist = true);

    /**
     * Constructs a rocksample with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
   RockSampleProblem(int width, int height, int x0, int y0, int sampled0, int status0,
                     double actionCost);


    /**
     * Constructs a rocksample with the specified width, height,
     * goal states, initial state (x0,y0,w0).
     * All states are equally probable to be a goal.
     * Heuristic is initialized to all-zero heuristic.
     */
    RockSampleProblem(int width, int height, int x0, int y0, int sampled0, int status0,
                     std::map<std::pair<int,int>, double> potential_goals_prob, double actionCost);


    /**
     * Constructs a rocksample with the specified width, height,
     * goal states, initial state (x0,y0) and heuristic.
     * All states are equally probable to be a goal.
     */
   RockSampleProblem(int width, int height, int x0, int y0, int sampled0, int status0,
                     mlcore::Heuristic* h);

    ~RockSampleProblem()
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
    virtual int getobservation(RockSampleState* s);

    /** Returns 1 if the state is a potential goal. Returns 0 otherwise.
     **/
    virtual bool isPotentialGoal(RockSampleState* s);

    virtual bool RSGoal(RockSampleState* s) const;

    double getactioncost(){return actionCost_;}

    int maxRocks() {return maxRocks_;}
};

#endif // MPDLIB_ROCKSAMPLEPROBLEM_H
