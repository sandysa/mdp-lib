#ifndef MPDLIB_EVACTION_H
#define MPDLIB_EVACTION_H

#include "../../Action.h"
class EVAction : public mlcore::Action
{
private:
    unsigned char level_;
    virtual std::ostream& print(std::ostream& os) const;
public:
    EVAction() : level_(-1) {}

    EVAction(const unsigned char level) : level_(level) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        EVAction* action = (EVAction*)  & rhs;
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

#endif // MPDLIB_EVACTION_H
