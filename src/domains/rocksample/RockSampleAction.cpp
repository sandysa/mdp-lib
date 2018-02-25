#include "../../../include/domains/rocksample/RockSampleAction.h"
#include "../../../include/domains/rocksample/RockSampleProblem.h"

std::ostream& RockSampleAction::print(std::ostream& os) const
{
    os << "action ";
    if (dir_ == rocksample::UP)
        os << "up";
    if (dir_ == rocksample::DOWN)
        os << "down";
    if (dir_ == rocksample::LEFT)
        os << "left";
    if (dir_ == rocksample::RIGHT)
        os << "right";
    if (dir_ == rocksample::SAMPLE)
        os << "sample";
    return os;
}
