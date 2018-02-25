#include "../../../include/util/general.h"

#include "../../../include/domains/rocksample/RockSampleState.h"

RockSampleState::RockSampleState(
    mlcore::Problem* problem, int x, int y, int sampledRocks, int status) : x_(x), y_(y), status_(status), sampledRocks_(sampledRocks)
{
     problem_ = problem;
}

std::ostream& RockSampleState::print(std::ostream& os) const
{
    os << "RockSample : (" << x_ << "," << y_ << "," << sampledRocks_ << "," << status_ << ")";
    return os;
}

bool RockSampleState::equals(mlcore::State* other) const
{
    RockSampleState* rss = static_cast<RockSampleState*> (other);
    return *this == *rss;
}

int RockSampleState::hashValue() const
{
    return x_ + 31 * (y_ + 31 * (sampledRocks_ + 31 * status_));
}
