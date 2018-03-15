#ifndef MPDLIB_GUSSPGRIDWORLDPROBLEM_H
#define MPDLIB_GUSSPGRIDWORLDPROBLEM_H

#include <unordered_set>
#include <map>
#include <cassert>
#include <utility>
#include <vector>

#include "GridWorldState.h"
#include "GUSSPGridWorldState.h"
#include "GridWorldProblem.h"

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

class GWDetHeuristicGUSSP;

/**
 * A class representing a GUSSP formulation of grid world problem as described
 * in AIAMA 3rd Edition, with some additional variants.
 */
namespace GUSSPgridworld
{
    const unsigned char UP = 0;
    const unsigned char DOWN = 1;
    const unsigned char LEFT = 2;
    const unsigned char RIGHT = 3;
}


class GUSSPGridWorldProblem : public mlcore::Problem
{
    friend class GWDetHeuristicGUSSP;

private:
    int width_;
    int height_;
    int x0_;
    int y0_;
    std::vector<std::pair<std::pair<int, int>,double>> goalPos0_;
    double actionCost_;
    double holeCost_;
    bool allDirections_;
    mlcore::State* absorbing;
    IntPairSet walls;
    IntPairSet holes;
    IntPairSet dead_ends;
    PairDoubleMap* goals_;

     void addSuccessor(GUSSPGridWorldState* state,
                      std::list<mlcore::Successor>& successors,
                      int val,
                      int limit,
                      int newx,
                      int newy,
                      std::vector<std::pair<std::pair<int, int>,double>> newgoalPos,
                      double prob);
    void addAllActions();
    std::vector<std::pair<int,int>> potential_goals;
    bool GUSSPgridGoal(GUSSPGridWorldState* s) const;

    public:
    /**
     * Default constructor.
     *
     * Creates a grid world with a single goal state (0,0)
     * and all-zero heuristic.
     */
    GUSSPGridWorldProblem();

    /**
     * Constructs a grid world from a string file representation
     * stored at the given filename. The constructor also receives
     * the cost of the actions.
     */
    GUSSPGridWorldProblem(const char* filename,
                     double actionCost = 1.0,
                     double holeCost = 100.0,
                     bool allDirections = false,
                     bool uniform_dist = true);

     ~GUSSPGridWorldProblem()
      {
        delete goals_;
      }
    /**
     * Overrides method from GUSSPProblem.
     */
    virtual bool goal(mlcore::State* s) const;

   /**
     * Overrides method from GUSSPProblem.
     */
    virtual std::list<mlcore::Successor> transition(mlcore::State* s,
                                                    mlcore::Action* a);

    /** Returns if the SSP state of it is a potential goal or not **/
    virtual bool isPotentialGoal(mlcore::State* s);

    /**
     * Overrides method from GUSSPProblem.
     */
    virtual double cost(mlcore::State* s, mlcore::Action* a) const;

    /**
     * Overrides method from GUSSPProblem.
     */
    virtual bool applicable(mlcore::State* s, mlcore::Action* a) const;

    virtual void setGoalPos(std::vector<std::pair<int,int>> potential_goals, bool uniform_dist);

    /** returns observation if the state is a goal state or not**/
    virtual int getObservation(mlcore::State* s)const;

    /** randomly sets one goal to be true goal **/
    virtual void setTrueGoal(std::vector<std::pair<int,int>> potential_goals);

    double getactioncost(){return actionCost_;}
};

#endif // MPDLIB_GUSSPGRIDWORLDPROBLEM_H
