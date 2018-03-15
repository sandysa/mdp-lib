#include "../../../include/domains/gridworld/GUSSPGridWorldAction.h"
#include "../../../include/domains/gridworld/GUSSPGridWorldProblem.h"

std::ostream& GUSSPGridWorldAction::print(std::ostream& os) const
{
    os << "action ";
    if (dir_ == GUSSPgridworld::UP)
        os << "up";
    if (dir_ == GUSSPgridworld::DOWN)
        os << "down";
    if (dir_ == GUSSPgridworld::LEFT)
        os << "left";
    if (dir_ == GUSSPgridworld::RIGHT)
        os << "right";
    return os;
}
