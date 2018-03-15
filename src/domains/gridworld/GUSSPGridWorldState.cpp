#include <vector>

#include "../../../include/util/general.h"

#include "../../../include/domains/gridworld/GridWorldState.h"
#include "../../../include/domains/gridworld/GUSSPGridWorldState.h"


GUSSPGridWorldState::GUSSPGridWorldState(
    mlcore::Problem* GUSSPproblem, int x, int y, std::vector<std::pair<std::pair<int, int>,double>> goalPos) :
                                                                x_(x), y_(y), goalPos_(goalPos)
{
     problem_ = GUSSPproblem;
}

std::ostream& GUSSPGridWorldState::print(std::ostream& os) const
{
    os << "grid state: (" << x_ << "," << y_;
     for(int i = 0; i < goalPos_.size(); ++i){
        std::pair<std::pair<int, int>,double> pos = goalPos_[i];
        std::pair<int,int> locs = pos.first;
        os << ", [" << locs.first << "," << locs.second << "," <<
              pos.second << "]";
    }
    os << ")";
      return os;
}

bool GUSSPGridWorldState::equals(mlcore::State* other) const
{
    GUSSPGridWorldState* gws = static_cast<GUSSPGridWorldState*> (other);
    return *this == *gws;
}

int GUSSPGridWorldState::hashValue() const
{
     int hashVal = x_ + 31* y_;
     for(int i = 0; i < goalPos_.size(); ++i){
        std::pair<std::pair<int, int>,double> pos = goalPos_[i];
        std::pair<int,int> locs = pos.first;
        hashVal += 31 * (locs.first + 31  * locs.second);
    }
    return hashVal;
}
