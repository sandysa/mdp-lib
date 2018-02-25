#include "../../../include/util/general.h"

#include "../../../include/domains/SearchRescue/SearchRescueState.h"

SearchRescueState::SearchRescueState(
    mlcore::Problem* problem, int x, int y, int victims, int status) : x_(x), y_(y), status_(status), savedVictims_(victims)
{
     problem_ = problem;
}

std::ostream& SearchRescueState::print(std::ostream& os) const
{
    os << "SearchRescue : (" << x_ << "," << y_ << "," << savedVictims_ << "," << status_ << ")";
    return os;
}

bool SearchRescueState::equals(mlcore::State* other) const
{
    SearchRescueState* srs = static_cast<SearchRescueState*> (other);
    return *this == *srs;
}

int SearchRescueState::hashValue() const
{
    return x_ + 31 * (y_ + 31 * (savedVictims_ + 31 * status_));
}
