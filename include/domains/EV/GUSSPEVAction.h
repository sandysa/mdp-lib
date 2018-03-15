#ifndef MPDLIB_GUSSPEVAction_H
#define MPDLIB_GUSSPEVAction_H

#include "../../Action.h"
class GUSSPEVAction : public mlcore::Action
{
private:
    unsigned char level_;
    virtual std::ostream& print(std::ostream& os) const;
public:
    GUSSPEVAction() : level_(-1) {}

    GUSSPEVAction(const unsigned char level) : level_(level) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        GUSSPEVAction* action = (GUSSPEVAction*)  & rhs;
        level_ =  action->level_;
        return *this;
    }

    /**
     * Overriding method from Action.
     */
    virtual int hashValue() const
    {
        return (int) level_;
    }

    unsigned char level() const
    {
        return level_;
    }
};

#endif // MPDLIB_GUSSPEVAction_H
