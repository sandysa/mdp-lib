#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../../../include/Problem.h"
#include "../../../include/domains/EV/GUSSPEVProblem.h"
#include "../../../include/domains/EV/GUSSPEVState.h"
#include "../../../include/domains/EV/GUSSPEVAction.h"
#include "../../../include/domains/EV/GUSSPEVReward.h"

void GUSSPEVProblem::addAllActions()
{
    mlcore::Action* charge_L1 = new GUSSPEVAction(EV::CHARGE_L1);
    mlcore::Action* charge_L2 = new GUSSPEVAction(EV::CHARGE_L2);
    mlcore::Action* charge_L3 = new GUSSPEVAction(EV::CHARGE_L3);
    mlcore::Action* nop = new GUSSPEVAction(EV::NOP);
    mlcore::Action* discharge_L1 = new GUSSPEVAction(EV::DISCHARGE_L1);
    mlcore::Action* discharge_L2 = new GUSSPEVAction(EV::DISCHARGE_L2);
    mlcore::Action* discharge_L3 = new GUSSPEVAction(EV::DISCHARGE_L3);
    actions_.push_front(charge_L1);
    actions_.push_front(charge_L2);
    actions_.push_front(charge_L3);
    actions_.push_front(nop);
    actions_.push_front(discharge_L1);
    actions_.push_front(discharge_L2);
    actions_.push_front(discharge_L3);
}
GUSSPEVProblem:: GUSSPEVProblem(int start_soc, int end_soc, int start_time,
                        int reward_case, int exit_range, bool uniform_dist):
                        start_soc_(start_soc), end_soc_(end_soc), start_time_(start_time),
                        rewardCase_(reward_case), exit_time_range_(exit_range)
{
    problem_name_ = "ev";
    for (int i = 0; i < exit_time_range_; i++){
        potential_goals.push_back(EV::horizon_ - i);
     }
     setInitBelief(potential_goals, uniform_dist);

     s0 =  new GUSSPEVState(this, start_soc_, 0, goalPos0_);
     absorbing_ = new GUSSPEVState(this, 100, 100, goalPos0_);
     this->addState(s0);
     this->addState(absorbing_);
     addAllActions();
     setTrueGoal(potential_goals);
}

GUSSPEVProblem:: GUSSPEVProblem(int start_soc, int end_soc, int start_time, int end_time,
                        int reward_case):
                        start_soc_(start_soc), end_soc_(end_soc), start_time_(start_time),
                        end_time_(end_time), rewardCase_(reward_case)
{
    problem_name_ = "ev";
    potential_goals.push_back(end_time_);

    setInitBelief(potential_goals, true); // uniform dist set to true

     s0 =  new GUSSPEVState(this, start_soc_, 0, goalPos0_);
     absorbing_ = new GUSSPEVState(this, 100, 100, goalPos0_);
     this->addState(s0);
     this->addState(absorbing_);
     addAllActions();
}

void GUSSPEVProblem::setInitBelief(std::vector<int> potential_goals, bool uniform_dist)
{
   if(potential_goals.size() == 1)
        goalPos0_.push_back(std::make_pair(potential_goals.at(0), 1.0));

    /** sets unfirm initial belief to all potential goals **/
    if(uniform_dist)
    {
     for (int i = 0; i < potential_goals.size(); ++i){
          goalPos0_.push_back(std::make_pair(potential_goals.at(i), (1.0/potential_goals.size())));
        }
     }
 }
bool GUSSPEVProblem::isPotentialGoal(GUSSPEVState* evs)
{
    for(int i = 0; i < potential_goals.size(); i++)
    {
        if(potential_goals.at(i) == evs->timestep())
            return true;
    }
    return false;
}

void GUSSPEVProblem::setTrueGoal(std::vector<int> potential_goals)
{
    int rn = rand() % potential_goals.size();
    end_time_ = potential_goals.at(rn);
    std::cout << "Departure time = " <<  end_time_ << std::endl;
}

bool GUSSPEVProblem::goal(mlcore::State * s) const
{
       return s == absorbing_;
}

bool GUSSPEVProblem::GUSSPEVgoal(GUSSPEVState * s) const
{
       return s->timestep() == end_time_;
}


bool GUSSPEVProblem::applicable(mlcore::State* state, mlcore::Action* action) const
{
    GUSSPEVState* evs = static_cast<GUSSPEVState*>(state);
    GUSSPEVAction* a = static_cast<GUSSPEVAction*>(action);
    if(state == absorbing_)
        return true;

    if(evs->soc() == 0 && a->level() > 3) // trying to discharge
        return false;

    if(evs->soc() == 100 && a->level() < 3) // trying to charge
        return false;

   return true;
}
/** returns 1 if its a goal, 0 otherwise. **/
int GUSSPEVProblem::getObservation(GUSSPEVState* evs) const
{
    if(evs->timestep() == end_time_)
        return 1;
    return 0;
}

bool GUSSPEVProblem::isPeak(int timestep)
{
    double actual_time = (timestep* EV::time_interval_)/60.0 + start_time_;
	long  iPart = (long) actual_time;
	return PeakHour(iPart-1);
}

double getSOCfactor(GUSSPEVState* evs, GUSSPEVAction* a, GUSSPEVState* evj)
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
/** normalizes belief based on observation **/
std::vector<std::pair<int, double>> GUSSPEVProblem::updateBelief(std::vector<std::pair<int,double>> curr_belief)
{
    double total = 0;
    std::vector<std::pair<int,double>> new_belief;
    for (auto i = curr_belief.begin(); i != curr_belief.end(); ++i)
    {
        std::pair<int, double> pos (*i);
        total+= pos.second;
    }
    for (auto i = curr_belief.begin(); i != curr_belief.end(); ++i)
    {
        std::pair<int, double> pos (*i);
        new_belief.push_back(std::make_pair(pos.first, (pos.second/total)));
    }
    return new_belief;
}

std::list<mlcore::Successor>
GUSSPEVProblem::transition(mlcore::State* s, mlcore::Action* action)
{
    std::list<mlcore::Successor> successors;
    assert(applicable(s, action));

    GUSSPEVState* evs = static_cast<GUSSPEVState*>(s);
    GUSSPEVAction* a =  static_cast<GUSSPEVAction*>(action);
    std::vector<std::pair<int, double>> goalPos = evs->goalPos();

    if (goal(s) || GUSSPEVgoal(evs)) {
        successors.push_back(
            mlcore::Successor(this->addState(absorbing_), 1.0));
        return successors;
        }

    int idAction = a->hashValue();
    std::vector<mlcore::SuccessorsList>* allSuccessors = evs->allSuccessors();
     if (!allSuccessors->at(idAction).empty()) {
        return allSuccessors->at(idAction);
    }

    /** otherwise, get observation **/
    int obs = getObservation(evs);

    /** update belief of potential goal **/
    if (isPotentialGoal(evs))
    {
        for (auto i = goalPos.begin(); i != goalPos.end(); ++i){
            std::pair<int, double> pos (*i);
            if (pos.first == evs->timestep() && pos.second !=obs && obs == 0)
            {
                goalPos.erase(i);
                goalPos.push_back(std::make_pair(pos.first, obs));
                std::vector<std::pair<int, double>> goalPos_new = updateBelief(goalPos);
                GUSSPEVState* evj  = new GUSSPEVState(this, evs->soc(), evs->timestep(), goalPos_new);
                successors.push_back(mlcore::Successor(this->addState(evj), 1.0));
                return successors;
               }
           }
      }

    /** SSP transitions **/
    int t = min(EV::horizon_-1 , evs->timestep()+1);
    if(a->level() == 3) //soc unaltered for NOP
    {
          GUSSPEVState* evj  = new GUSSPEVState(this, evs->soc(), t, goalPos);
          double trans  = getSOCfactor(evs, a, evj);
          if (trans > 0) {
              allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
            } else {
                delete evj;
               }
      return allSuccessors->at(idAction);
    }

    if(a->level() < 3) //charging
    {
      for(int soc = evs->soc(); soc <= 100; soc++){
            GUSSPEVState* evj  = new GUSSPEVState(this, soc, t, goalPos);
             double trans  =  getSOCfactor(evs, a, evj);
             if(trans > 0){
                    allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
              } else {
                    delete evj;
               }
          }
        return allSuccessors->at(idAction);
    }
///** reaches here only for discharging **/
    for(int soc = 0; soc <= evs->soc(); soc++){
                 GUSSPEVState* evj  = new GUSSPEVState(this, soc, t, goalPos);
                    double trans  =  getSOCfactor(evs, a, evj);
                    if(trans > 0){
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                    } else {
                        delete evj;
                    }
                }
          return allSuccessors->at(idAction);
}
double GUSSPEVProblem::Meancost(mlcore::State* s , mlcore::Action* a)
{
    GUSSPEVReward* reward = new GUSSPEVReward();
    GUSSPEVAction* eva = static_cast<GUSSPEVAction*>(a);
    GUSSPEVState* evs =  static_cast<GUSSPEVState*>(s);
    double rate = 0.0;
    if(!applicable(s,a))
        return rate;

    for(mlcore::Successor& su : this->transition(s, a))
        {
            GUSSPEVState* evj = static_cast<GUSSPEVState*>(su.su_state);
            rate += su.su_prob * reward->getReward(evs, eva, evj, rewardCase_, this);
         }
    delete reward;
    return rate;
}
double GUSSPEVProblem::cost (mlcore::State* s , mlcore::Action* a) const
	{
        GUSSPEVState* evs =  static_cast<GUSSPEVState*>(s);
        GUSSPEVProblem* evp = const_cast<GUSSPEVProblem*> (this);
        if(goal(s))
            return 0;

        if(evs->soc() < end_soc_ && GUSSPEVgoal(evs))
            return 100;

        if(evs->soc() >= end_soc_ && GUSSPEVgoal(evs))
            return 0;

       return evp->Meancost(s,a);
    }
