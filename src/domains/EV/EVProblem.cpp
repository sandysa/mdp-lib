#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../../../include/Problem.h"
#include "../../../include/domains/EV/EVProblem.h"
#include "../../../include/domains/EV/EVState.h"
#include "../../../include/domains/EV/EVAction.h"
#include "../../../include/domains/EV/EVReward.h"
//#include "EVRewards.cpp"

void EVProblem::addAllActions()
{
    mlcore::Action* charge_L1 = new EVAction(EV::CHARGE_L1);
    mlcore::Action* charge_L2 = new EVAction(EV::CHARGE_L2);
    mlcore::Action* charge_L3 = new EVAction(EV::CHARGE_L3);
    mlcore::Action* nop = new EVAction(EV::NOP);
    mlcore::Action* discharge_L1 = new EVAction(EV::DISCHARGE_L1);
    mlcore::Action* discharge_L2 = new EVAction(EV::DISCHARGE_L2);
    mlcore::Action* discharge_L3 = new EVAction(EV::DISCHARGE_L3);
    actions_.push_front(charge_L1);
    actions_.push_front(charge_L2);
    actions_.push_front(charge_L3);
    actions_.push_front(nop);
    actions_.push_front(discharge_L1);
    actions_.push_front(discharge_L2);
    actions_.push_front(discharge_L3);
}
EVProblem:: EVProblem(int start_soc, int end_soc, int start_time,
                        int end_time, int reward_case):
                        start_soc_(start_soc), end_soc_(end_soc), start_time_(start_time),
                        end_time_(end_time), rewardCase_(reward_case)
{
/**
* at start, the exit communication is set to false (3). Plans to horizon_ by default
*/
    s0 =  new EVState(this, start_soc_, 0, EV::START_DEMAND_DIST, EV::START_PRICE_DIST, 3);
    addAllActions();
    problem_name_ = "ev";
    absorbing_ = new EVState(this, 100, 100, EV::START_DEMAND_DIST, EV::START_PRICE_DIST, 0);
    this->addState(absorbing_);
}

/**
* FILL GOAL STATES
*/
bool EVProblem::goal(mlcore::State * s) const
{
 /*   EVState* evs = static_cast<EVState*>(s);
    if(evs->soc() >= end_soc_ && ((evs->exit_comm() == 0)|| evs->timestep() == EV::horizon_-1))
        return true; */
          if(s == absorbing_)
            return true;
      return false;
}

bool EVProblem::applicable(mlcore::State* state, mlcore::Action* action) const
{
    EVState* evs = static_cast<EVState*>(state);
    EVAction* a = static_cast<EVAction*>(action);
    if(evs == absorbing_)
        return true;

    if(evs->soc() == 0 && a->level() > 3) // trying to discharge
        return false;

    if(evs->soc() == 100 && a->level() < 3) // trying to charge
        return false;

 /*   if(evs->soc() < end_soc_ && evs->exit_comm() == 0)
        return false;

    if(evs->soc() < end_soc_ && evs->timestep() == EV::horizon_-1 )
            return false; */

    return true;

}
double EVProblem::get_comm_factor(EVState* evs, EVState* evj)
{
    double comm_peak = .2;
    double comm_offpeak = .05;

	if(evs->timestep() <  EV::horizon_ - 3)
	{
        if(evj->exit_comm() == 2 && evs->exit_comm() == 3 && evs->timestep() == EV::horizon_-7) //very high chance of exiting at H-4 (most likely departure other than exiting at horizon_
            return comm_peak;
        if(evj->exit_comm() == 3 && evs->exit_comm() == 3 && evs->timestep() == EV::horizon_-7)
			return 1-comm_peak;
        if(evj->exit_comm() == 2 && evs->exit_comm() == 3 ) // at all other times
			return comm_offpeak;
		if(evj->exit_comm() == 3 && evs->exit_comm() == 3 ) // at all other times
			return 1-comm_offpeak;
		if(evj->exit_comm() == 1 && evs->exit_comm() == 2)
			return 1;
		if(evj->exit_comm() == 0 && evs->exit_comm() == 1)
			return 1.0;
	}
		else if(evs->timestep() >= EV::horizon_-3) // no new exit time communication happens.. if no time has been communicated, exist @ H
		{
			if(evj->exit_comm() == 3 && evs->exit_comm() == 3)
				return 1;
			if(evj->exit_comm() == 1 && evs->exit_comm() == 2)
				return 1;
			if(evj->exit_comm() == 0 && evs->exit_comm() == 1)
				return 1.0;
		}
		return 0;	// all other cases that fail for prev if.
}

bool EVProblem::isPeak(int timestep)
{
    double actual_time = (timestep* EV::time_interval_)/60.0 + start_time_;
	long  iPart = (long) actual_time;
	return PeakHour(iPart-1);
}

double EVProblem::get_factor(EVState* evs, EVState* evj)
{
    double factor=0;
	if(rewardCase_ <3 )
	{
        if(evs->demand() == evj->demand() && evs->demand() == EV::START_DEMAND_DIST && evs->price() == evj->price() && evs->price() == EV::START_PRICE_DIST )
        return 1.0;

        return 0.0;
    }
   if(rewardCase_ == 3)
		{
		/** otherwsie, prob wont sum to 1... Also, price dist should not change in RF 3 **/
            if(evs->price() != evj->price())
                return 0.0;

			if(!isPeak(evs->timestep()) &&isPeak(evj->timestep())) //moving from off-peak to peak hour
			{
				if(evj->demand() ==0)
					return EV::offpeak_peak_d1;
				if(evj->demand() ==1)
					return EV::offpeak_peak_d2;
				if(evj->demand() ==2 )
					return EV::offpeak_peak_d3;
				else return EV::offpeak_peak_d4;
			}
			if(isPeak(evs->timestep()) &&isPeak(evj->timestep())) //both are peak hour
			{
				if(evj->demand() ==0)
					return EV::peak_peak_d1;
				else if(evj->demand() ==1)
					return EV::peak_peak_d2;
				else if(evj->demand() ==2)
					return EV::peak_peak_d3;
				else
					return EV::peak_peak_d4;
			}
			if(isPeak(evs->timestep()) && !isPeak(evj->timestep())) //moving from peak to off peak hour
			{
				if(evj->demand() ==0)
					return EV::peak_offpeak_d1;
				else if(evj->demand() ==1)
					return EV::peak_offpeak_d2;
				else if(evj->demand() ==2)
					return EV::peak_offpeak_d3;
				else
					return EV::peak_offpeak_d4;
			}
			if(!isPeak(evs->timestep()) && !isPeak(evj->timestep())) //both are off peak hour
			{
				if(evj->demand() ==0)
					return EV::offpeak_offpeak_d1;
				else if(evj->demand() ==1)
					return EV::offpeak_offpeak_d2;
				else if(evj->demand() ==2)
					return EV::offpeak_offpeak_d3;
				else
					return EV::offpeak_offpeak_d4;
			}
		}
		if(rewardCase_ == 4)
		{
			if(!isPeak(evs->timestep()) &&isPeak(evj->timestep())) //moving from off-peak to peak hour
			{
				if(evj->demand() ==0 && evj->price() ==0 )
					return EV::offpeak_peak_d1* EV::d1_p1;
				if(evj->demand() ==0 && evj->price() ==1 )
					return EV::offpeak_peak_d1* EV::d1_p2;

				if(evj->demand() ==1 && evj->price() ==0 )
					return EV::offpeak_peak_d2* EV::d2_p1;
				if(evj->demand() ==1 && evj->price() ==1 )
					return EV::offpeak_peak_d2* EV::d2_p2;

				if(evj->demand() ==2 && evj->price() ==0 )
					return EV::offpeak_peak_d3* EV::d3_p1;
				if(evj->demand() ==2 && evj->price() == 1)
					return EV::offpeak_peak_d3 * EV::d3_p2;

				if(evj->demand() ==3 && evj->price() ==0 )
					return EV::offpeak_peak_d4* EV::d4_p1;
				if(evj->demand() ==3 && evj->price() == 1)
					return EV::offpeak_peak_d4 * EV::d4_p2;
			}
			if(isPeak(evs->timestep()) &&isPeak(evj->timestep())) //both are peak hour
			{
				if(evj->demand() ==0 && evj->price() ==0 )
					return EV::peak_peak_d1* EV::d1_p1;
				if(evj->demand() ==0 && evj->price() ==1 )
					return EV::peak_peak_d1* EV::d1_p2;

				if(evj->demand() ==1 && evj->price() ==0 )
					return EV::peak_peak_d2* EV::d2_p1;
				if(evj->demand() ==1 && evj->price() ==1 )
					return EV::peak_peak_d2* EV::d2_p2;

				if(evj->demand() ==2 && evj->price() ==0 )
					return EV::peak_peak_d3* EV::d3_p1;
				if(evj->demand() ==2 && evj->price() == 1)
					return EV::peak_peak_d3 * EV::d3_p2;

				if(evj->demand() ==3 && evj->price() ==0 )
					return EV::peak_peak_d4* EV::d4_p1;
				if(evj->demand() ==3 && evj->price() == 1)
					return EV::peak_peak_d4 * EV::d4_p2;
			}
			if(isPeak(evs->timestep()) && !isPeak(evj->timestep())) //moving from peak to off peak hour
			{
				if(evj->demand() ==0 && evj->price() ==0 )
					return EV::peak_offpeak_d1* EV::d1_p1;
				if(evj->demand() ==0 && evj->price() ==1 )
					return EV::peak_offpeak_d1* EV::d1_p2;

				if(evj->demand() ==1 && evj->price() ==0 )
					return EV::peak_offpeak_d2* EV::d2_p1;
				if(evj->demand() ==1 && evj->price() ==1 )
					return EV::peak_offpeak_d2* EV::d2_p2;

				if(evj->demand() ==2 && evj->price() ==0 )
					return EV::peak_offpeak_d3* EV::d3_p1;
				if(evj->demand() ==2 && evj->price() == 1)
					return EV::peak_offpeak_d3 * EV::d3_p2;

				if(evj->demand() ==3 && evj->price() ==0 )
					return EV::peak_offpeak_d4* EV::d4_p1;
				if(evj->demand() ==3 && evj->price() == 1)
					return EV::peak_offpeak_d4 *EV::d4_p2;
			}
			if(!isPeak(evs->timestep()) && !isPeak(evj->timestep())) //both are off peak hour
			{
				if(evj->demand() ==0 && evj->price() ==0 )
					return EV::offpeak_offpeak_d1* EV::d1_p1;
				if(evj->demand() ==0 && evj->price() ==1 )
					return EV::offpeak_offpeak_d1* EV::d1_p2;

				if(evj->demand() ==1 && evj->price() ==0 )
					return EV::offpeak_offpeak_d2* EV::d2_p1;
				if(evj->demand() ==1 && evj->price() ==1 )
					return EV::offpeak_offpeak_d2* EV::d2_p2;

				if(evj->demand() ==2 && evj->price() ==0 )
					return EV::offpeak_offpeak_d3* EV::d3_p1;
				if(evj->demand() ==2 && evj->price() == 1)
					return EV::offpeak_offpeak_d3 * EV::d3_p2;

				if(evj->demand() ==3 && evj->price() ==0 )
					return EV::offpeak_offpeak_d4* EV::d4_p1;
				if(evj->demand() ==3 && evj->price() == 1)
					return EV::offpeak_offpeak_d4 * EV::d4_p2;
			}
		}
		return factor;
}
double getSOCfactor(EVState* evs, EVAction* a, EVState* evj)
{
    if(evs->timestep()+1 != evj->timestep())
         return 0.0;

    if(evs->soc() == 100 && a->level() < 3) //trying to charge in max charge
        return 0.0;
    if(evs->soc() == 0 && a->level() > 3) //trying to discharge in min charge
        return 0.0;

   if(evs->soc() == evj->soc() && evs->timestep()+1 == evj->timestep()){
      if(a->level() == 3)
        return 1.0;
    else
        return EV::failure_prob; // failure to charge or discharge
   }

    //successful charging
   if(evs->soc() + EV::SPEED_L1 == evj->soc() && a->level() == 0 )
        return EV::success_prob;
    if(evs->soc() + EV::SPEED_L2 == evj->soc() && a->level() == 1)
        return EV::success_prob;
    if(evs->soc() + EV::SPEED_L3 == evj->soc() && a->level() == 2)
        return EV::success_prob;

     //successful discharging
   if(evs->soc() - EV::SPEED_L1 == evj->soc() && a->level() == 4)
        return EV::success_prob;
    if(evs->soc() - EV::SPEED_L2 == evj->soc() && a->level() == 5)
        return EV::success_prob;
    if(evs->soc() - EV::SPEED_L3 == evj->soc() && a->level() == 6)
        return EV::success_prob;


    //charging fractions
    if(evs->soc() + EV::SPEED_L1 > 100 && evj->soc() == 100 && a->level() == 0)
        return EV::success_prob;
    if(evs->soc() + EV::SPEED_L2 > 100 && evj->soc() == 100 && a->level() == 1)
        return EV::success_prob;
    if(evs->soc() + EV::SPEED_L3 > 100 && evj->soc() == 100 && a->level() == 2)
        return EV::success_prob;

    //discharging fractions
    if(evs->soc() - EV::SPEED_L1 < 0 && evj->soc() == 0 && a->level() == 4)
        return EV::success_prob;
    if(evs->soc() - EV::SPEED_L2 < 0 && evj->soc() == 0 && a->level() == 5)
        return EV::success_prob;
    if(evs->soc() - EV::SPEED_L3 < 0 && evj->soc() == 0 && a->level() == 6)
       return EV::success_prob;

    return 0.0;
}


std::list<mlcore::Successor>
EVProblem::transition(mlcore::State* s, mlcore::Action* action)
{
    if (useFlatTransition_)
        return flatTransition(s, action);

    std::list<mlcore::Successor> successors;
    assert(applicable(s, action));

      if (goal(s) || s == absorbing_ ) {
        successors.push_back(
            mlcore::Successor(this->addState(absorbing_), 1.0));
        return successors;
        }

    EVState* evs = static_cast<EVState*>(s);
    EVAction* a =  static_cast<EVAction*>(action);
    if(evs->soc() < end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1)){
        successors.push_back(mlcore::Successor(this->addState(absorbing_), 1.0));
     return successors;
    }

    if(evs->soc() >= end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1)){
        successors.push_back(mlcore::Successor(this->addState(absorbing_), 1.0));
        return successors;
      }

    int idAction = a->hashValue();
    std::vector<mlcore::SuccessorsList>* allSuccessors = evs->allSuccessors();
     if (!allSuccessors->at(idAction).empty()) {
        return allSuccessors->at(idAction);
    }



    int t = min(EV::horizon_-1 , evs->timestep()+1);
    if(a->level() == 3)
    {
         for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, evs->soc(), t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if (trans > 0) {
                   //   successors.push_back(mlcore::Successor(this->addState(evj), trans));
                       allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                    }
               }
           }
        }

       // return successors;
        return allSuccessors->at(idAction);
    }

    if(a->level() < 3) //charging
    {
      for(int soc = evs->soc(); soc <= 100; soc++){
        for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, soc, t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if(trans > 0){
                     //   successors.push_back(mlcore::Successor(this->addState(evj), trans));
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                    }
                }
         }
    }
   }
   // return successors;
        return allSuccessors->at(idAction);
    }
/** reaches here only for discharging **/
  for(int soc = 0; soc <= evs->soc(); soc++){
        for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, soc, t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if(trans > 0){
                   //      successors.push_back(mlcore::Successor(this->addState(evj), trans));
                             allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                    }
                }
         }
    }
   }
     // return successors;
        return allSuccessors->at(idAction);
}

//Returns the same numbre of successors
std::list<mlcore::Successor>
EVProblem::flatTransition(mlcore::State* s, mlcore::Action* action)
{
    std::list<mlcore::Successor> successors;
    assert(applicable(s, action));
    EVState* evs = static_cast<EVState*>(s);
    EVAction* a =  static_cast<EVAction*>(action);

    int numsucc = numSuccessorsAction(a);

      if (goal(s) || s == absorbing_ ) {
        for(int i = 0 ; i < numsucc; i++){
            successors.push_back(
                mlcore::Successor(this->addState(absorbing_), 1.0/numsucc));
        }
        return successors;
        }


    if(evs->soc() < end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1)){
        for(int i = 0 ; i < numsucc; i++){
            successors.push_back(
                mlcore::Successor(this->addState(absorbing_), 1.0/numsucc));
        }
     return successors;
    }

    if(evs->soc() >= end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1)){
         for(int i = 0 ; i < numsucc; i++){
            successors.push_back(
                mlcore::Successor(this->addState(absorbing_), 1.0/numsucc));
        }
        return successors;
      }

    int idAction = a->hashValue();
    std::vector<mlcore::SuccessorsList>* allSuccessors = evs->allSuccessors();
     if (!allSuccessors->at(idAction).empty()) {
        return allSuccessors->at(idAction);
    }

    int t = min(EV::horizon_-1 , evs->timestep()+1);
    if(a->level() == 3)
    {
         for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, evs->soc(), t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if (trans > 0) {
                      successors.push_back(mlcore::Successor(this->addState(evj), trans));
//                       allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                    }
               }
           }
        }
    }

    else if(a->level() < 3) //charging
    {
      for(int soc = evs->soc(); soc <= 100; soc++){
        for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, soc, t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if(trans > 0){
                        successors.push_back(mlcore::Successor(this->addState(evj), trans));
//                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                        }
                    }
            }
            }
        }
    }
/** reaches here only for discharging **/
 else{
  for(int soc = 0; soc <= evs->soc(); soc++){
        for(int demand = 0; demand <= 3; demand++){
            for(int price = 0; price < 2; price++){
                for(int depart_comm = 0; depart_comm <= 3; depart_comm++){
                    EVState* evj  = new EVState(this, soc, t, demand, price, depart_comm);
                    double trans  = get_comm_factor(evs, evj) * get_factor(evs, evj) * getSOCfactor(evs, a, evj);
                    if(trans > 0){
                         successors.push_back(mlcore::Successor(this->addState(evj), trans));
//                             allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                        }
                    }
            }
        }
    }
   }
     int diff = numsucc - successors.size();
        if(diff == 0)
            return successors;
        else{
            std::list<mlcore::Successor> temp_successors;
            if(diff == 2){
                for(int i = 0; i < diff; i++){
                    for(const mlcore::Successor& su : successors){
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(su.su_state), su.su_prob/2.0));
                        }
                }
             }
            else if(successors.size() == 1 && numsucc >= 2){
                for(int i = 0; i < numsucc; i++){
                    for(const mlcore::Successor& su : successors)
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(su.su_state), 1.0/numsucc));
                }
            }
            else if ((numsucc/ successors.size()) == 2){
                for(int i = 0; i < 2; i++){
                    for(const mlcore::Successor& su : successors)
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(su.su_state), su.su_prob/2.0));
                 }
            }
            if(allSuccessors->at(idAction).size() != numsucc){
                std::cout << "mismatch!! numsucc = " <<  numsucc << " tran size=" << allSuccessors->at(idAction).size() <<
                " allsucc size = " << successors.size() << ", " << s << a << endl;

                for(const mlcore::Successor& su : successors){
                    std::cout << su.su_state << " prob = " << su.su_prob << endl;
                    }
                std::exit(EXIT_FAILURE);
            }
            return allSuccessors->at(idAction);
        }
}


double EVProblem::Meancost(mlcore::State* s , mlcore::Action* a)
{
    EVReward* reward = new EVReward();
    EVAction* eva = static_cast<EVAction*>(a);
    EVState* evs =  static_cast<EVState*>(s);
    double rate = 0.0;
    if(!applicable(s,a))
        return rate;

    for(mlcore::Successor& su : this->transition(s, a))
        {
            EVState* evj = static_cast<EVState*>(su.su_state);
            rate += su.su_prob * reward->getReward(evs, eva, evj, rewardCase_, this);
        }
    delete reward;
    return rate;
}
double EVProblem::cost (mlcore::State* s , mlcore::Action* a) const
	{
        EVState* evs =  static_cast<EVState*>(s);
        EVProblem* evp = const_cast<EVProblem*> (this);
        if(goal(evs))
            return 0;

        if(evs->soc() < end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1))
            return 100;

        if(evs->soc() >= end_soc_ && ((evs->exit_comm() == 0)) || (evs->timestep() == EV::horizon_-1))
            return 0;

       return evp->Meancost(s,a);
    }

int EVProblem::numSuccessorsAction(EVAction* eva)
{
    int eva_level = eva->hashValue();
    if(rewardCase_ < 3){
        if(eva_level == 3)//NOP
            return 2;

        return 4;
    }
    else if(rewardCase_ == 3){
        if(eva_level == 3) // NOP
            return 8;

         return 16;
     }
    else if(rewardCase_ == 4){
         if(eva_level == 3) // NOP
            return 16;

         return 32;

    }

}
