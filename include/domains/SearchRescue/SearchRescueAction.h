#ifndef MPDLIB_SEARCHRESCUEACTION_H
#define MPDLIB_SEARCHRESCUEACTION_H

#include "../../Action.h"

class SearchRescueAction : public mlcore::Action
{
private:
    unsigned char dir_;
    virtual std::ostream& print(std::ostream& os) const;

public:
    SearchRescueAction() : dir_(-1) {}

    SearchRescueAction(const unsigned char dir) : dir_(dir) {}

    /**
     * Overriding method from Action.
     */
    virtual mlcore::Action& operator=(const mlcore::Action& rhs)
    {
        if (this == &rhs)
            return *this;

        SearchRescueAction* action = (SearchRescueAction*)  & rhs;
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

#endif // MPDLIB_GRIDWORLDACTION_H
