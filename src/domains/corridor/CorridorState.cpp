#include "../../../include/domains/corridor/CorridorProblem.h"
#include "../../../include/domains/corridor/CorridorState.h"
#include "../../../include/domains/corridor/CorridorAction.h"


CorridorState::CorridorState(int x, int y, bool alive,
                                mlcore::Problem* problem)
{
    x_ = x;
    y_ = y;
    alive_ = alive;
    problem_ = problem;

    /* Adding a successor entry for each action */
    for (int i = 0; i < 2; i++) {
        allSuccessors_.push_back(std::list<mlcore::Successor> ());
    }
}

std::ostream& CorridorState::print(std::ostream& os) const
{
    CorridorProblem* rtp = static_cast<CorridorProblem *> (problem_);
    os << "(" << x_  << ", " << y_ << ", " << alive_ << ")";
    return os;
}

mlcore::State& CorridorState::operator=(const mlcore::State& rhs)
{
    if (this == &rhs)
        return *this;

    const CorridorState* state = static_cast<const CorridorState *> (&rhs);
    x_ = state->x_;
    y_ = state->y_;
    alive_ = state->alive_;
    problem_ = state->problem_;
    return *this;
}

bool CorridorState::operator==(const mlcore::State& rhs) const
{
    const CorridorState* state = static_cast<const CorridorState *> (&rhs);
    return x_ == state->x_
            && y_ == state->y_
          && alive_ == state->alive_;
}
bool CorridorState::equals(mlcore::State* other) const
{
    CorridorState* state = static_cast<CorridorState *> (other);
    return *this ==  *state;
}

int CorridorState::hashValue() const
{
    int v =  (alive_ == true)? 1 : 0;
    return x_ + 31 * (y_ + 31 * (v));
}
