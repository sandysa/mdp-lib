#ifndef GUSSPEVREWARD_H
#define GUSSPEVREWARD_H

#include "../../Problem.h"
#include "../../../include/domains/EV/GUSSPEVState.h"
#include "../../../include/domains/EV/GUSSPEVAction.h"
#include "../../../include/domains/EV/GUSSPEVProblem.h"

using namespace std;

class GUSSPEVReward
{
public:
    GUSSPEVReward(){};
    ~GUSSPEVReward(){};
    virtual double getChargingCosts(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* problem)const;

    virtual double getreward_C1(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* problem) const;

//    virtual double getreward_C2(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* problem)const;
//
//    virtual double getreward_C3(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* problem) const;
//
//    virtual double getreward_C4(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* problem)const;

    virtual double getReward(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, int rewardCase, mlcore::Problem* problem);

    virtual double getlowestCost();

};
#endif //GUSSPEVREWARD_H
