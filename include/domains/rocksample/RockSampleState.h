#ifndef MPDLIB_ROCKSAMPLESTATE_H
#define MPDLIB_ROCKSAMPLESTATE_H

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include "../../State.h"

/**
 * A class representing a grid world state as described in AIAMA 3rd Edition.
 */
class RockSampleState : public mlcore::State
{
private:
    int x_;
    int y_;
    int sampledRocks_;
    int status_;

    virtual std::ostream& print(std::ostream& os) const;

public:

    /**
     * Constructs a RockSampleState representing grid position (x,y)
     * on the problem
     * given as a first parameter.
     */
    RockSampleState(mlcore::Problem* problem, int x, int y, int collectedRocks, int status);

    /**
     * Copy constructor. The resulting state represents the same position as the
     * state passed as parameter.
     */
    RockSampleState(const RockSampleState& rss) : x_(rss.x_), y_(rss.y_),status_(rss.status_),sampledRocks_(rss.sampledRocks_) {}

    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

        RockSampleState* rss = (RockSampleState *) &rhs;
        x_ =  rss->x_;
        y_=  rss->y_;
        status_ = rss->status_;
        sampledRocks_ = rss->sampledRocks_;
        return *this;
    }

    virtual bool operator==(const mlcore::State& rhs) const
    {
        RockSampleState* rss = (RockSampleState *) &rhs;
        return x_ == rss->x_ && y_ == rss->y_ && status_ == rss->status_ && sampledRocks_ == rss->sampledRocks_;
    }

    virtual bool equals(mlcore::State* other) const;
    virtual int hashValue() const;


    int x() {return x_;}

    int y() {return y_;}

    int status() {return status_;}

    int sampledRocks() {return sampledRocks_;}

};

#endif // MPDLIB_ROCKSAMPLESTATE_H
