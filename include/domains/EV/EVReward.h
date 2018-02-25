#ifndef EVREWARD_H
#define EVREWARD_H

#include "../../Problem.h"
#include "../../../include/domains/EV/EVState.h"
#include "../../../include/domains/EV/EVAction.h"
#include "../../../include/domains/EV/EVProblem.h"

//using namespace EV;
using namespace std;

class EVReward
{
public:
    EVReward(){};
    ~EVReward(){};
    virtual double getChargingCosts(EVState* evs, EVAction* a, EVState* evj, mlcore::Problem* problem)const;

    virtual double getreward_C1(EVState* evs, EVAction* a, EVState* evj, mlcore::Problem* problem) const;

    virtual double getreward_C2(EVState* evs, EVAction* a, EVState* evj, mlcore::Problem* problem)const;

    virtual double getreward_C3(EVState* evs, EVAction* a, EVState* evj, mlcore::Problem* problem) const;

    virtual double getreward_C4(EVState* evs, EVAction* a, EVState* evj, mlcore::Problem* problem)const;

    virtual double getReward(EVState* evs, EVAction* a, EVState* evj, int rewardCase, mlcore::Problem* problem);

};
#endif
