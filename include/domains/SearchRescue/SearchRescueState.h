#ifndef MPDLIB_SEARCHRESCUESTATE_H
#define MPDLIB_SEARCHRESCUESTATE_H

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include "../../State.h"

/**
 * A class representing a grid world state as described in AIAMA 3rd Edition.
 */
class SearchRescueState : public mlcore::State
{
private:
    int x_;
    int y_;
    int savedVictims_;
    int status_;

    virtual std::ostream& print(std::ostream& os) const;

public:

    /**
     * Constructs a SearchRescueState representing grid position (x,y)
     * on the problem
     * given as a first parameter.
     */
    SearchRescueState(mlcore::Problem* problem, int x, int y, int victims, int status);

    /**
     * Copy constructor. The resulting state represents the same position as the
     * state passed as parameter.
     */
    SearchRescueState(const SearchRescueState& srs) : x_(srs.x_), y_(srs.y_),status_(srs.status_),savedVictims_(srs.savedVictims_) {}

    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

        SearchRescueState* srs = (SearchRescueState *) &rhs;
        x_ =  srs->x_;
        y_=  srs->y_;
        status_ = srs->status_;
        savedVictims_ = srs->savedVictims_;
        return *this;
    }

    virtual bool operator==(const mlcore::State& rhs) const
    {
        SearchRescueState* srs = (SearchRescueState *) &rhs;
        return x_ == srs->x_ && y_ == srs->y_ && status_ == srs->status_ && savedVictims_ == srs->savedVictims_;
    }

    virtual bool equals(mlcore::State* other) const;
    virtual int hashValue() const;


    int x() {return x_;}

    int y() {return y_;}

    int status() {return status_;}

    int victims() {return savedVictims_;}

};

#endif // MPDLIB_SEARCHRESCUESTATE_H
