#ifndef MDPLIB_CORRIDORSTATE_H
#define MDPLIB_CORRIDORSTATE_H

#include <functional>

#include "../../Problem.h"
#include "../../State.h"


class CorridorState : public mlcore::State
{
private:
    int x_;
    int y_;
    bool alive_;

    /* A cache of all successors (for all actions) of this state */
    std::vector<mlcore::SuccessorsList> allSuccessors_;

    virtual std::ostream& print(std::ostream& os) const;

public:
    /**
     * Creates a state for the CORRIDOR problem with the given (x,y, alive) position
     * and assigned to the given index.
     *
     * Every tuple (x,NARROW, alive) should be assigned to a unique index.
     */
    CorridorState(int x, int y, bool alive, mlcore::Problem* problem);

    virtual ~CorridorState() {}

    int x() const { return x_; }
    int y() const {return y_; }
    bool alive() const { return alive_; }

    /**
     * Returns a pointer to the successor cache of this state.
     */
    std::vector<mlcore::SuccessorsList>* allSuccessors()
    {
        return &allSuccessors_;
    }

    /**
     * Overrides method from State.
     */
    virtual mlcore::State& operator=(const mlcore::State& rhs);

    /**
     * Overrides method from State.
     */
    virtual bool operator==(const mlcore::State& rhs) const;

    /**
     * Overrides method from State.
     */
    virtual bool equals(mlcore::State* other) const;

    /**
     * Overrides method from State.
     */
    virtual int hashValue() const;
};

#endif // MDPLIB_CORRIDORSTATE_H
