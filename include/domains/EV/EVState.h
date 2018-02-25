#ifndef MPDLIB_EVSTATE_H
#define MPDLIB_EVSTATE_H

#include <iostream>

#include "../../State.h"

class EVState : public mlcore::State
{
private:
    int soc_;
    int timestep_;
    int demand_;
    int price_;
    int exit_comm_;

 /* A cache of all successors (for all actions) of this state */
    std::vector<mlcore::SuccessorsList> allSuccessors_;

 virtual std::ostream& print(std::ostream& os) const;

public:
    EVState(mlcore::Problem* problem, int soc, int timestep, int domand, int price, int exit_comm);

    virtual mlcore::State& operator=(const mlcore::State& rhs)
    {
        if (this == &rhs)
            return *this;

       EVState* evs = (EVState *) &rhs;
       soc_ = evs->soc_;
       timestep_ = evs->timestep_;
       demand_ = evs->demand_;
       price_ = evs->price_;
       exit_comm_ = evs->exit_comm_;
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
        EVState* evs = (EVState *) &rhs;
        return soc_ == evs->soc_ &&  timestep_  == evs->timestep_ &&
                demand_ == evs->demand_ && price_ == evs->price_ && exit_comm_ == evs->exit_comm_;
    }

    virtual bool equals(mlcore::State* other) const;

    virtual int hashValue() const;

    int soc() const { return soc_; }

    int timestep() const { return timestep_; }

    int demand() const{ return demand_; }

    int price() const{ return price_; }

    int exit_comm() const{ return exit_comm_; }

    };

#endif // MDPLIB_EVSTATE_H

