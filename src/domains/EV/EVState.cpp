#include "../../../include/domains/EV/EVProblem.h"
#include "../../../include/domains/EV/EVState.h"


EVState::EVState(mlcore::Problem* problem, int soc, int t, int demand, int price, int exit_comm)
{
    soc_ = soc;
    timestep_ = t;
    demand_ = demand;
    price_ = price;
    exit_comm_ = exit_comm;
    problem_ = problem;

    /* Adding a successor entry for each action */
    for (int i = 0; i < 7; i++) {
        allSuccessors_.push_back(std::list<mlcore::Successor> ());
    }
}

bool EVState::equals(mlcore::State* other) const
{
    EVState* state = static_cast<EVState *> (other);;
    return *this ==  *state;
}

int EVState::hashValue() const
{
   return soc_ + 31 * (timestep_ + 31 *(demand_ + 31 *(price_ + 31 * exit_comm_)));

}

std::ostream& EVState::print(std::ostream& os) const
{
    EVProblem* rtp = static_cast<EVProblem *> (problem_);
    os << "(" << soc_  << ", " << timestep_ << ", " << demand_ << ", " << price_ << ", " << exit_comm_ << ")";
    return os;
}
