#ifndef MPDLIB_GUSSPGRIDWORLDACTION_H
#define MPDLIB_GUSSPGRIDWORLDACTION_H

#include "../../Action.h"

class GUSSPGridWorldAction : public mlcore::Action
{
private:
    unsigned char dir_;
    virtual std::ostream& print(std::ostream& os) const;

public:
    GUSSPGridWorldAction() : dir_(-1) {}

    GUSSPGridWorldAction(const unsigned char dir) : dir_(dir) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        GUSSPGridWorldAction* action = (GUSSPGridWorldAction*)  & rhs;
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

#endif // MPDLIB_GUSSPGRIDWORLDACTION_H
