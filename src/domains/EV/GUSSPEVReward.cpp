#include "../../../include/domains/EV/GUSSPEVProblem.h"
#include "../../../include/domains/EV/GUSSPEVState.h"
#include "../../../include/domains/EV/GUSSPEVAction.h"
#include "../../../include/domains/EV/GUSSPEVReward.h"
using namespace EV;
using namespace std;

double GUSSPEVReward::getReward(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, int rewardCase, mlcore::Problem* problem)
{
    if(rewardCase == 1)
        return getreward_C1(evs,a,evj, problem);
//    else if(rewardCase == 2)
//        return getreward_C2(evs,a,evj,problem);
//    else if(rewardCase == 3)
//        return getreward_C3(evs,a,evj, problem);
//    else if(rewardCase == 4)
//        return getreward_C4(evs,a,evj, problem);

    return 0.0;
}
double GUSSPEVReward::getChargingCosts(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* prob)const
{
    GUSSPEVProblem* problem = static_cast<GUSSPEVProblem*>(prob);
    if(((evs->soc()+EV::SPEED_L1 == evj->soc()) || (evs->soc()+EV::SPEED_L1 > 100 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 0) //normal speed charge
        {
                if(problem->isPeak(evs->timestep()))
                    return .2;
                return .15;
        }
         if(((evs->soc()+EV::SPEED_L2 == evj->soc()) || (evs->soc()+EV::SPEED_L2 > 100 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 1) //double speed charge
            {
                if(problem->isPeak(evs->timestep()))
                    return .8;
                return .4;
            }
         if(((evs->soc()+EV::SPEED_L3 == evj->soc()) || (evs->soc()+EV::SPEED_L3 > 100 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 2) //double speed charge
            {
                if(problem->isPeak(evs->timestep()))
                    return 6.2;
                return 4.5;
            }
        return 10.0;
}
double GUSSPEVReward::getlowestCost()
{
    return -6.2; // largest discharging cost
}

double GUSSPEVReward::getreward_C1(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* prob) const
{
    GUSSPEVProblem* problem = static_cast<GUSSPEVProblem*>(prob);
    if(evs->soc() == evj->soc() && evs->timestep()+1 == evj->timestep())
        return 0.0;

    if(a->level() >= 0 && a->level() < 3)
        return getChargingCosts(evs,a,evj,problem);

     if(a->level() >= 4 && a->level() < 7)
     {
        if(((evs->soc()-EV::SPEED_L1 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 4) //normal speed charge
        {
                if(problem->isPeak(evs->timestep()))
                    return -.2;
                return -.15;
        }
         if(((evs->soc()-EV::SPEED_L2 == evj->soc()) || (evs->soc()-EV::SPEED_L2 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 5) //double speed charge
            {
                if(problem->isPeak(evs->timestep()))
                    return -.8;
                return -.4;
            }
         if(((evs->soc()-EV::SPEED_L3 == evj->soc()) || (evs->soc()-EV::SPEED_L3 < 0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 6) //double speed charge
            {
                if(problem->isPeak(evs->timestep()))
                    return -6.2;
                return -4.5;
            }
     }

  }
//double GUSSPEVReward::getreward_C2(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* prob) const
//{
//     GUSSPEVProblem* problem = static_cast<GUSSPEVProblem*>(prob);
//      if(evs->soc() == evj->soc() && evs->timestep()+1 == evj->timestep())
//        return 0.0;
//
//        if(a->level() >= 0 && a->level() < 3)
//        return getChargingCosts(evs,a,evj,problem);
//
//     if(a->level() >= 4 && a->level() < 7)
//      {
//        if(((evs->soc()-EV::SPEED_L1 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 4) //normal speed charge
//        {
//                if(problem->isPeak(evs->timestep()))
//                    return -.1;
//                return -.05;
//        }
//         if(((evs->soc()-EV::SPEED_L2 == evj->soc()) || (evs->soc()-EV::SPEED_L2 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 5) //double speed charge
//            {
//                if(problem->isPeak(evs->timestep()))
//                    return -.5;
//                return -.25;
//            }
//         if(((evs->soc()-EV::SPEED_L3 == evj->soc()) || (evs->soc()-EV::SPEED_L3 < 0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 6) //double speed charge
//            {
//                if(problem->isPeak(evs->timestep()))
//                    return -4.0;
//                return -2.5;
//            }
//     }
//}
//
//double GUSSPEVReward::getreward_C3(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* prob) const
//{
//     GUSSPEVProblem* problem = static_cast<GUSSPEVProblem*>(prob);
//      if(evs->soc() == evj->soc() && evs->timestep()+1 == evj->timestep())
//        return 0.0;
//
//        if(a->level() >= 0 && a->level() < 3)
//        return getChargingCosts(evs,a,evj,problem);
//
//    double rate = 0.0;
//     if(a->level() >= 4 && a->level() < 7)
//      {
//       if(((evs->soc()-EV::SPEED_L1 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 4) //normal speed charge
//        {
//            if(evs->demand() == 0)
//                rate = (problem->isPeak(evs->timestep()))? -.2 : -.15;
//            else if(evs->demand() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.17 : -.12;
//            else if(evs->demand() == 2)
//                 rate = (problem->isPeak(evs->timestep()))? -.11 : -.06;
//            else if(evs->demand() == 3)
//                 rate = (problem->isPeak(evs->timestep()))? -.06 : -.03;
//        }
//
//         if(((evs->soc()-EV::SPEED_L2 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 5) //normal speed charge
//        {
//            if(evs->demand() == 0)
//                rate = (problem->isPeak(evs->timestep()))?  -1.6 : -.86;
//            else if(evs->demand() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.96 : -.62;
//            else if(evs->demand() == 2)
//                 rate = (problem->isPeak(evs->timestep()))? -.5 : -.25;
//            else if(evs->demand() == 3)
//                 rate = (problem->isPeak(evs->timestep()))? -.2 : -.1;
//        }
//         if(((evs->soc()-EV::SPEED_L3 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 6) //normal speed charge
//        {
//            if(evs->demand() == 0)
//                rate = (problem->isPeak(evs->timestep()))?  -3.75 : -3.25;
//            else if(evs->demand() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -3.25 : -2.5;
//            else if(evs->demand() == 2)
//                 rate = (problem->isPeak(evs->timestep()))? -1.15 : -.57;
//            else if(evs->demand() == 3)
//                 rate = (problem->isPeak(evs->timestep()))? -0.4 : -.2;
//        }
//    }
//       return rate;
//}
//double GUSSPEVReward::getreward_C4(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj, mlcore::Problem* prob) const
//{
//     GUSSPEVProblem* problem = static_cast<GUSSPEVProblem*>(prob);
//      if(evs->soc() == evj->soc() && evs->timestep()+1 == evj->timestep())
//        return 0.0;
//
//        if(a->level() >= 0 && a->level() < 3)
//        return getChargingCosts(evs,a,evj,problem);
//
//    double rate = 0.0;
//     if(a->level() >= 4 && a->level() < 7)
//      {
//       if(((evs->soc()-EV::SPEED_L1 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 4) //normal speed charge
//        {
//            if(evs->demand() == 0 && evs->price() == 0)
//                rate = (problem->isPeak(evs->timestep()))? -.5 : -.35;
//            else  if(evs->demand() == 0 && evs->price() == 1)
//                rate = (problem->isPeak(evs->timestep()))? -.4 : -.25;
//            else if(evs->demand() == 1 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.4 : -.25;
//            else if(evs->demand() == 1 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.3 : -.2;
//            else if(evs->demand() == 2 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.3 : -.2;
//             else if(evs->demand() == 2 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))?  -.2 : -.1;
//            else if(evs->demand() == 3 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.15 : -.1;
//             else if(evs->demand() == 3 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.1 : -.07;
//        }
//
//         if(((evs->soc()-EV::SPEED_L2 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 5) //normal speed charge
//        {
//            if(evs->demand() == 0 && evs->price() == 0)
//                rate = (problem->isPeak(evs->timestep()))? -1.08 : -.84;
//            else  if(evs->demand() == 0 && evs->price() == 1)
//                rate = (problem->isPeak(evs->timestep()))? -.9 : -.75;
//            else if(evs->demand() == 1 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.8 : -.7;
//            else if(evs->demand() == 1 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))?  -.7 : -.6;
//            else if(evs->demand() == 2 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))?  -.6 : -.4;
//             else if(evs->demand() == 2 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))?  -.4 : -.28;
//            else if(evs->demand() == 3 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.3 : -.2;
//             else if(evs->demand() == 3 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.2 : -.13;
//        }
//         if(((evs->soc()-EV::SPEED_L3 == evj->soc()) || (evs->soc()-EV::SPEED_L1 <0 && evj->soc() == 100)) && evs->timestep()+1 == evj->timestep() && a->level() == 6) //normal speed charge
//        {
//             if(evs->demand() == 0 && evs->price() == 0)
//                rate = (problem->isPeak(evs->timestep()))? -2.85 : -2.0 ;
//            else  if(evs->demand() == 0 && evs->price() == 1)
//                rate = (problem->isPeak(evs->timestep()))?  -1.87 : -1.23;
//            else if(evs->demand() == 1 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -1.53 : -1.0 ;
//            else if(evs->demand() == 1 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))?  -1.0 : -.5;
//            else if(evs->demand() == 2 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))?  -.7 : -.4;
//             else if(evs->demand() == 2 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))?  -.5 : -.3;
//            else if(evs->demand() == 3 && evs->price() == 0)
//                 rate = (problem->isPeak(evs->timestep()))? -.4 : -.3;
//             else if(evs->demand() == 3 && evs->price() == 1)
//                 rate = (problem->isPeak(evs->timestep()))? -.2 : -.1;
//        }
//    }
//       return rate;
//}
