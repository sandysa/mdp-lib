#ifndef MPDLIB_ROCKSAMPLEACTION_H
#define MPDLIB_ROCKSAMPLEACTION_H

#include "../../Action.h"

class RockSampleAction : public mlcore::Action
{
private:
    unsigned char dir_;
    virtual std::ostream& print(std::ostream& os) const;

public:
    RockSampleAction() : dir_(-1) {}

    RockSampleAction(const unsigned char dir) : dir_(dir) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        RockSampleAction* action = (RockSampleAction*)  & rhs;
        dir_ =  action->dir_;
        return *this;
    }

    /**
     * Overriding method from Action.
     */
    virtual int hashValue() const
    {
        return (int) dir_;
    }

    unsigned char dir() const
    {
        return dir_;
    }
};

#endif // MPDLIB_ROCKSAMPLEACTION_H
