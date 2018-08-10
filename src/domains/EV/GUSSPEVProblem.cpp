#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

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
        potential_goals.push_back(std::make_pair(EV::horizon_ - i, -1));
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
    potential_goals.push_back(std::make_pair(end_time_,-1));

    setInitBelief(potential_goals, true); // uniform dist set to true

     s0 =  new GUSSPEVState(this, start_soc_, 0, goalPos0_);
     absorbing_ = new GUSSPEVState(this, 100, 100, goalPos0_);
     this->addState(s0);
     this->addState(absorbing_);
     addAllActions();
}

void GUSSPEVProblem::setInitBelief(std::vector<std::pair<int,int>> potential_goals, bool uniform_dist)
{
   if(potential_goals.size() == 1)
        goalPos0_.push_back(std::make_pair(potential_goals.at(0), 1.0));

    /** sets uniform initial belief to all potential goals **/
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
        if(potential_goals.at(i).first == evs->timestep())
            return true;
    }
    return false;
}

bool GUSSPEVProblem::isPotentialGoal(int soc, int t)
{
    for(int i = 0; i < potential_goals.size(); i++)
    {
        if(potential_goals.at(i).first == t)
            return true;
    }
    return false;
}

void GUSSPEVProblem::setTrueGoal(std::vector<std::pair<int,int>> potential_goals)
{
    int rn = rand() % potential_goals.size();
    end_time_ = potential_goals.at(rn).first;
    std::cout << "Departure time = " <<  end_time_ << std::endl;
}

bool GUSSPEVProblem::goal(mlcore::State * s) const
{
       return s == absorbing_;
}

bool GUSSPEVProblem::GUSSPEVgoal(GUSSPEVState * s) const
{
//       return s->timestep() == end_time_;
        std::vector<std::pair<std::pair<int,int>,double>> pg = s->goalPos();
        for(auto it =  pg.begin(); it!= pg.end(); ++it){
            std::pair<std::pair<int,int>,double> val = *it;
            if(val.second == 1)
                return true;
        }
        return false;
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

int GUSSPEVProblem::getObservation(int soc, int t) const
{
    if(t == end_time_)
        return 1;

    return 0;
}
void printVector(std::vector<std::pair<std::pair<int,int>, double>> gp)
{
    for(auto it = gp.begin(); it != gp.end(); ++it)
    {
        std::pair<std::pair<int,int>,double> val = *it;
        std::cout << val.first.first << "," << val.first.second << "," << val.second << std::endl;
    }
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
 std::vector<std::pair<std::pair<int,int>, double>> GUSSPEVProblem::updateBelief( std::vector<std::pair<std::pair<int,int>, double>> curr_belief)
{
    double total = 0;
    int goal_index  = -1; //indicates if the true goal has been observed.
    std::vector<std::pair<std::pair<int,int>, double>> new_belief;
    for (int i = 0; i < curr_belief.size(); i++)
    {
        std::pair<std::pair<int,int>, double> pos =  curr_belief.at(i);
        total+= pos.second;
        if(pos.second == 1) //true goal observed
            goal_index = i;
    }
    for (int i = 0; i < curr_belief.size(); i++)
    {
        std::pair<std::pair<int,int>, double> pos = curr_belief.at(i);
        if(goal_index > -1){
            if( i == goal_index)
                 new_belief.push_back(std::make_pair(pos.first, 1.0));
            else
                new_belief.push_back(std::make_pair(pos.first, 0.0));
        }
        else
            new_belief.push_back(std::make_pair(pos.first, (pos.second/total)));
    }
     //order them based on locations. otherwise [(A,0), (B,1)] and [(B,1),(A,0)] are treated as different states.
    std::sort(new_belief.begin(), new_belief.end());
    return new_belief;
}

 std::vector<std::pair<std::pair<int,int>, double>> GUSSPEVProblem::getGoalPos(int soc, int t,  std::vector<std::pair<std::pair<int,int>, double>> gp){

    std::vector<std::pair<std::pair<int,int>, double>> goalPos_new = gp;
    int obs = getObservation(soc, t);

    /** update belief of potential goal **/
    if (isPotentialGoal(soc, t))
    {
        for (auto i = gp.begin(); i != gp.end(); ++i){
            std::pair<std::pair<int,int>, double> pos (*i);
            if (pos.first.first == t && pos.second != obs && obs == 0)
            {
                gp.erase(i);
                gp.push_back(std::make_pair(pos.first, obs));
                std::vector<std::pair<std::pair<int,int>, double>> goalPos_new = updateBelief(gp);
                return goalPos_new;
               }
           }
      }
    return goalPos_new;
}


std::list<mlcore::Successor>
GUSSPEVProblem::transition(mlcore::State* s, mlcore::Action* action)
{
    std::list<mlcore::Successor> successors;
    assert(applicable(s, action));

    GUSSPEVState* evs = static_cast<GUSSPEVState*>(s);
    GUSSPEVAction* a =  static_cast<GUSSPEVAction*>(action);
    std::vector<std::pair<std::pair<int,int>, double>> goalPos = evs->goalPos();

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

    std::vector<std::pair<std::pair<int,int>, double>> gp_new = evs->getGoalBelief();
    /** SSP transitions **/
    int t = min(EV::horizon_-1 , evs->timestep()+1);
    if(a->level() == 3) //soc unaltered for NOP
    {
        //gp_new  = getGoalPos(evs->soc(), t, goalPos);
         GUSSPEVState* evj  = new GUSSPEVState(this, evs->soc(), t, gp_new);
         double trans  = getSOCfactor(evs, a, evj);
          if (trans > 0) {
          //    allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                delete evj;
                addSuccessor(evs,allSuccessors,a->level(),evs->soc(), t, gp_new, trans);
            } else {
                delete evj;
               }
      return allSuccessors->at(idAction);
    }

    if(a->level() < 3) //charging
    {
      for(int soc = evs->soc(); soc <= 100; soc++){
//        gp_new  = getGoalPos(soc, t, goalPos);
          GUSSPEVState* evj  = new GUSSPEVState(this, soc, t, gp_new);
             double trans  =  getSOCfactor(evs, a, evj);
             if(trans > 0){
//                    allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                     delete evj;
                     addSuccessor(evs,allSuccessors,a->level(), soc, t, gp_new, trans);
              } else {
                    delete evj;
               }
          }
        return allSuccessors->at(idAction);
    }
///** reaches here only for discharging **/
    for(int soc = 0; soc <= evs->soc(); soc++){
//                    gp_new  = getGoalPos(soc, t, goalPos);
                    GUSSPEVState* evj  = new GUSSPEVState(this, soc, t, gp_new);
                    double trans  =  getSOCfactor(evs, a, evj);
                    if(trans > 0){
//                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(evj), trans));
                         delete evj;
                        addSuccessor(evs,allSuccessors,a->level(),soc, t, gp_new, trans);
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
            rate += su.su_prob * (reward->getReward(evs, eva, evj, rewardCase_, this));
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

void GUSSPEVProblem::addSuccessor(
    GUSSPEVState* state, std::vector<mlcore::SuccessorsList>* allSuccessors, int idAction,
    int newsoc, int newt, std::vector<std::pair<std::pair<int, int>,double>> newgoalPos, double prob)
{
        /** If potential goal, update beliefs based on observation **/
        if (isPotentialGoal(newsoc, newt)){
            for (auto i = newgoalPos.begin(); i != newgoalPos.end(); ++i){
                std::pair<std::pair<int,int>, double> pos (*i);
                std::pair<int,int> locs = pos.first;
                if (locs.first == newt)
                {
                  if(pos.second == 0 || pos.second == 1) //already collapsed belief
                   {
                        GUSSPEVState *next = new GUSSPEVState(this, newsoc, newt, newgoalPos);
                        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(next), prob));
                        return;
                   } else {
                       for(int obs = 0; obs <= 1; obs ++){
                             std::vector<std::pair<std::pair<int, int>,double>> newgoalPoscp = newgoalPos;
                             std::vector<std::pair<std::pair<int,int>, double>>::iterator old_val = std::find(newgoalPoscp.begin(), newgoalPoscp.end(), pos);
                             if (old_val != newgoalPoscp.end())
                                newgoalPoscp.erase(old_val);

                             newgoalPoscp.push_back(std::make_pair(locs,obs));
                             newgoalPoscp = updateBelief(newgoalPoscp);
                             GUSSPEVState *next = new GUSSPEVState(this, newsoc, newt, newgoalPoscp);
                             mlcore::Successor succ (this->addState(next), prob * 0.5);
                             allSuccessors->at(idAction).push_back(succ);
                       }
                    }
                }
            }
        }
        else{
             GUSSPEVState *next = new GUSSPEVState(this, newsoc, newt, newgoalPos);
             allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(next), prob));
        }
}
