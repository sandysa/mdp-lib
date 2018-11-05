#include "../../../include/domains/corridor/CorridorAction.h"
#include "../../../include/domains/corridor/CorridorProblem.h"

std::ostream& CorridorAction::print(std::ostream& os) const
{
    os << "action ";
    if (speed_ == grid_corridor::SLOW)
        os << "move slow";
    if (speed_ == grid_corridor::FAST)
        os << "move fast";
    return os;
}
