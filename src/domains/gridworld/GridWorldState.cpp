#include "../../../include/util/general.h"

#include "../../../include/domains/gridworld/GridWorldState.h"

GridWorldState::GridWorldState(
    mlcore::Problem* problem, int x, int y, int status) : x_(x), y_(y), status_(status)
{
     problem_ = problem;
}

std::ostream& GridWorldState::print(std::ostream& os) const
{
    os << "grid state: (" << x_ << "," << y_ << "," << status_ << ")";
    return os;
}

bool GridWorldState::equals(mlcore::State* other) const
{
    GridWorldState* gws = static_cast<GridWorldState*> (other);
    return *this == *gws;
}

int GridWorldState::hashValue() const
{
    return x_ + 31 * (y_ + 31 * status_);
}

int GridWorldState::x() const
{
    return x_;
}

int GridWorldState::y() const
{
    return y_;
}

int GridWorldState::status() const
{
    return status_;
}

