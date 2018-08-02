#ifndef MPDLIB_GUSSPGRIDWORLDSTATE_H
#define MPDLIB_GUSSPGRIDWORLDSTATE_H

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include "../../State.h"
#include "GridWorldState.h"
/**
 * A class representing a grid world state as described in AIAMA 3rd Edition.
 */
class GUSSPGridWorldState : public mlcore::State
{
private:

    int x_;
    int y_;
    std::vector<std::pair<std::pair<int, int>,double>> goalPos_;
    virtual std::ostream& print(std::ostream& os) const;

public:

    /**
     * Constructs a GUSSPGridWorldState representing grid state
     * on the problem given as a first parameter
     * and a belief over goals as second parameter.
     */
     GUSSPGridWorldState(mlcore::Problem* GUSSPproblem, int x, int y, std::vector<std::pair<std::pair<int, int>,double>> goalPos);

     /**
     * Copy constructor. The resulting state represents the same position as the
     * state passed as parameter.
     */
    GUSSPGridWorldState(const GUSSPGridWorldState& gws) : x_(gws.x_), y_(gws.y_), goalPos_(gws.goalPos_){}

    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

        GUSSPGridWorldState* gws = (GUSSPGridWorldState *) &rhs;
        x_ = gws->x_;
        y_ = gws->y_;
        goalPos_ =  gws->goalPos_;
        return *this;
    }

    virtual bool operator==(const mlcore::State& rhs) const
    {
        GUSSPGridWorldState* gws = (GUSSPGridWorldState *) &rhs;
        return x_ == gws->x_ && y_ == gws->y_ && goalPos_ == gws->goalPos_;
    }

    virtual bool equals(mlcore::State* other) const;

    virtual int hashValue() const;

    int x() {return x_;}
    int y() {return y_;}
    std::vector<std::pair<std::pair<int, int>,double>>goalPos() {return goalPos_;}

};

#endif // MPDLIB_GUSSPGRIDWORLDSTATE_H
