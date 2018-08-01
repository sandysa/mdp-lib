#include "../../../include/domains/EV/GUSSPEVProblem.h"
#include "../../../include/domains/EV/GUSSPEVState.h"


GUSSPEVState::GUSSPEVState(mlcore::Problem* problem, int soc, int t, std::vector<std::pair<std::pair<int,int>, double>> goalPos)
{
    soc_ = soc;
    timestep_ = t;
    goalPos_ = goalPos;
    problem_ = problem;
    goalBelief_ =  goalPos_;

    /* Adding a successor entry for each action */
    for (int i = 0; i < 7; i++) {
        allSuccessors_.push_back(std::list<mlcore::Successor> ());
    }
}

bool GUSSPEVState::equals(mlcore::State* other) const
{
    GUSSPEVState* state = static_cast<GUSSPEVState *> (other);;
    return *this ==  *state;
}

int GUSSPEVState::hashValue() const
{
   int hashVal = soc_ + 31* timestep_;
     for(int i = 0; i < goalPos_.size(); ++i){
        std::pair<std::pair<int,int>,double> pos = goalPos_[i];
        hashVal += 31 * pos.first.first ;
    }
    return hashVal;

}

std::ostream& GUSSPEVState::print(std::ostream& os) const
{
    GUSSPEVProblem* rtp = static_cast<GUSSPEVProblem *> (problem_);
    os << "(" << soc_  << ", " << timestep_ << ",";
    for(int i = 0; i < goalPos_.size(); ++i){
        std::pair<std::pair<int,int>,double> pos = goalPos_[i];
        os << "[" << pos.first.first << "," <<
              pos.second << "], ";
    }
    os << " )";
      return os;
    return os;
}
