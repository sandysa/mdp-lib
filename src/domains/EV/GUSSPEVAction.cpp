#include "../../../include/domains/EV/GUSSPEVAction.h"
#include "../../../include/domains/EV/GUSSPEVProblem.h"

std::ostream& GUSSPEVAction::print(std::ostream& os) const
{
    os << "action ";
    if (level_ == EV::CHARGE_L1)
        os << "CHARGE_L1";
    if (level_ == EV::CHARGE_L2)
        os << "CHARGE_L2";
    if (level_ == EV::CHARGE_L3)
        os << "CHARGE_L3";
    if (level_ == EV::NOP)
        os << "NOP";

     if (level_ == EV::DISCHARGE_L1)
        os << "DISCHARGE_L1";
    if (level_ == EV::DISCHARGE_L2)
        os << "DISCHARGE_L2";
    if (level_ == EV::DISCHARGE_L3)
        os << "DISCHARGE_L3";
    return os;
}
