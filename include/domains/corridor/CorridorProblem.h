#ifndef MDPLIB_CORRIDORPROBLEM_H
#define MDPLIB_CORRIDORPROBLEM_H

#include <unordered_map>
#include <vector>
#include <list>
#include <unordered_set>


#include "../../Action.h"
#include "../../Problem.h"
#include "../../State.h"
#include "../../util/general.h"

#include "CorridorAction.h"
#include "CorridorState.h"


/** currently works for 1-d grid and move forward action only. **/
namespace grid_corridor
{
    const char narrow = 'N';
    const char wall = 'X';
    const char start = 'S';
    const char goal = 'G';

    const unsigned char SLOW = 0;
    const unsigned char FAST = 1;

    /* Checks if the character is one of the valid track characters */
    inline bool checkValid(char c)
    {
        char valid[] = {wall,narrow};
        for (int i = 0; i < 2; i++)
            if (c == valid[i])
                return true;
        return false;
    }
}

/**
 * A class implementing the corridor problem.
 *
 * Moving at high speed in narrow corridor has higher probability of crashing
 * compared to moving at low speed in narrow areas.
 *
 * Crashing has a higher cost of recovery.
 */
class CorridorProblem : public mlcore::Problem
{
private:
    private:
    int width_;
    int height_;
    int x0_;
    int y0_;
    PairDoubleMap* goals_;
    mlcore::State* absorbing;
    IntPairSet walls;
    IntPairSet narrow;

    void addSuccessor(CorridorState* state,
                      std::list<mlcore::Successor>& successors,
                      int val,
                      int limit,
                      int newx,
                      int newy,
                      bool alive,
                      double prob);

    void addAllActions();

    bool corridorGoal(CorridorState* s) const;
public:

    /**
     * Constructs a grid world corridor from a string file representation
     * stored at the given filename. The constructor also receives
     * the cost of the actions.
     */
    CorridorProblem(const char* filename);


    ~CorridorProblem()
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
};

#endif // MDPLIB_CorridorPROBLEM_H
