#ifndef MDPLIB_CORRIDORACTION_H
#define MDPLIB_CORRIDORACTION_H

#include <cassert>

#include "../../Action.h"
#include "../../Problem.h"
#include "../../State.h"


/**
 * A class implementing actions in the corridor domain.
 * There are 2 actions that move forward only in two speeds: low and high.
 * Each speed has different success probability and cost.
 */
class CorridorAction : public mlcore::Action
{
private:
    unsigned char speed_;
    virtual std::ostream& print(std::ostream& os) const;

public:
    CorridorAction() : speed_(-1) {}

    CorridorAction(const unsigned char dir) : speed_(dir) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        CorridorAction* action = (CorridorAction*)  & rhs;
        speed_ =  action->speed_;
        return *this;
    }

    /**
     * Overriding method from Action.
     */
    virtual int hashValue() const
    {
        return (int) speed_;
    }

    unsigned char speed() const
    {
        return speed_;
    }
};

#endif // MDPLIB_CORRIDORACTION_H
