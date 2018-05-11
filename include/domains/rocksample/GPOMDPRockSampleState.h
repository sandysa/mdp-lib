#ifndef MPDLIB_GPOMDPROCKSAMPLESTATE_H
#define MPDLIB_GPOMDPROCKSAMPLESTATE_H

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include "../../State.h"

/**
 * A class representing a grid world state as described in AIAMA 3rd Edition.
 */
class GPOMDPRockSampleState : public mlcore::State
{
private:
    int x_;
    int y_;
    int sampledRocks_; // 0 indicates nothing sampled yet. 1 indicates successfully sampled.
    std::pair<int,int> gpair_; // the goal state x,y

    virtual std::ostream& print(std::ostream& os) const;

    /* A cache of all successors (for all actions) of this state */
    std::vector<mlcore::SuccessorsList> allSuccessors_;

public:

    /**
     * Constructs a RockSampleState representing grid position (x,y)
     * on the problem
     * given as a first parameter.
     */
    GPOMDPRockSampleState(mlcore::Problem* problem, int x, int y, int sampledRocks,
                        std::pair<int, int> goalPos);

    /**
     * Copy constructor. The resulting state represents the same position as the
     * state passed as parameter.
     */
    GPOMDPRockSampleState(const GPOMDPRockSampleState& rss) : x_(rss.x_), y_(rss.y_),sampledRocks_(rss.sampledRocks_),
                                                                                    gpair_(rss.gpair_){}

     /**
     * Returns a pointer to the successor cache of this state.
     */
    std::vector<mlcore::SuccessorsList>* allSuccessors()
    {
        return &allSuccessors_;
    }


    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

        GPOMDPRockSampleState* rss = (GPOMDPRockSampleState *) &rhs;
        x_ =  rss->x_;
        y_=  rss->y_;
        sampledRocks_ = rss->sampledRocks_;
        gpair_ =  rss->gpair_;
        return *this;
    }

    virtual bool operator==(const mlcore::State& rhs) const
    {
        GPOMDPRockSampleState* rss = (GPOMDPRockSampleState *) &rhs;
        return x_ == rss->x_ && y_ == rss->y_ && sampledRocks_ == rss->sampledRocks_  && gpair_ == rss->gpair_;
    }

    virtual bool equals(mlcore::State* other) const;
    virtual int hashValue() const;


    int x() {return x_;}

    int y() {return y_;}

    int sampledRocks() {return sampledRocks_;}

    std::pair<int, int> goalPair() {return gpair_;}
};

#endif // MPDLIB_GPOMDPROCKSAMPLESTATE_H
