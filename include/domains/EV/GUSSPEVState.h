#ifndef MPDLIB_GUSSPEVState_H
#define MPDLIB_GUSSPEVState_H

#include <iostream>
#include <vector>

#include "../../State.h"

class GUSSPEVState : public mlcore::State
{
private:
    int soc_;
    int timestep_;
    std::vector<std::pair<int, double>> goalPos_; //belief vector over exit times.

  /* A cache of all successors (for all actions) of this state */
    std::vector<mlcore::SuccessorsList> allSuccessors_;

 virtual std::ostream& print(std::ostream& os) const;

public:
    GUSSPEVState(mlcore::Problem* problem, int soc, int timestep, std::vector<std::pair<int, double>> goalPos);

    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

       GUSSPEVState* evs = (GUSSPEVState *) &rhs;
       soc_ = evs->soc_;
       timestep_ = evs->timestep_;
       goalPos_ == evs->goalPos_;
       return *this;
    }
    /**
     * Returns a pointer to the successor cache of this state.
     */
    std::vector<mlcore::SuccessorsList>* allSuccessors()
    {
        return &allSuccessors_;
    }

    virtual bool operator==(const mlcore::State& rhs) const
    {
        GUSSPEVState* evs = (GUSSPEVState *) &rhs;
        return soc_ == evs->soc_ &&  timestep_  == evs->timestep_ && goalPos_ == evs->goalPos_;
    }

    virtual bool equals(mlcore::State* other) const;

    virtual int hashValue() const;

    int soc() const { return soc_; }

    int timestep() const { return timestep_; }

    std::vector<std::pair<int, double>> goalPos() const {return goalPos_;}

    };

#endif // MDPLIB_GUSSPEVState_H

