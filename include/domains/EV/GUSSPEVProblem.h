#ifndef MPDLIB_GUSSPEVProblem_H
#define MPDLIB_GUSSPEVProblem_H

#include <unordered_set>

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

#include "GUSSPEVState.h"
#include "GUSSPEVAction.h"
#include "GUSSPEVReward.h"

namespace EV
{
    const int SPEED_L1 = 2;
    const int SPEED_L2 = 10;
    const int SPEED_L3 = 80;
    const int START_DEMAND_DIST = 0;
    const int START_PRICE_DIST = 1;

    //actions:
    const unsigned char CHARGE_L1  = 0;
    const unsigned char CHARGE_L2  = 1;
    const unsigned char CHARGE_L3  = 2;
    const unsigned char NOP  = 3;
    const unsigned char DISCHARGE_L1  = 4;
    const unsigned char DISCHARGE_L2  = 5;
    const unsigned char DISCHARGE_L3  = 6;

    const double success_prob = 0.8;
    const double failure_prob = 0.2;

    const int horizon_ = 16;
    const int time_interval_ = 30;
    	/****
		 * d1: peak distribution
		 * d2: midpeak distribution
		 * d3: offpeak distribution
		 * d4: super offpeak distribution
		 */
		/****
		 * p1: highest price
		 * p2: normal price distribution
		 */
	const double peak_peak_d1 = 0.5;
	const double peak_peak_d2 = 0.3;
    const double peak_peak_d3 = 0.1;
	const double peak_peak_d4 = 0.1;

    const double peak_offpeak_d1 = 0.2;
    const double peak_offpeak_d2 = 0.3;
    const double peak_offpeak_d3 = 0.4;
    const double peak_offpeak_d4 = 0.1;

    const double offpeak_peak_d1 = 0.3;
    const double offpeak_peak_d2 = 0.4;
    const double offpeak_peak_d3 = 0.2;
    const double offpeak_peak_d4 = 0.1;

    const double offpeak_offpeak_d1 = 0.1;
    const double offpeak_offpeak_d2 = 0.1;
    const double offpeak_offpeak_d3 = 0.4;
    const double offpeak_offpeak_d4 = 0.4;

    const double d1_p1 = .7;
    const double d1_p2 = .3;

    const double d2_p1 = .6;
    const double d2_p2 = .4;

    const double d3_p1 = .3;
    const double d3_p2 = .7;

    const double d4_p1 = .2;
    const double d4_p2 = .8;

    const int A = 7; //number of actions
}
class EVDetHeuristicGUSSP;

class GUSSPEVProblem : public mlcore::Problem
{
     friend class EVDetHeuristicGUSSP;

private:
    int start_soc_;
    int end_soc_;
    double start_time_;
    double end_time_;
    int rewardCase_;
    int horizon_;
    int exit_time_range_; //sets the time range considered for potential goals.
    std::vector<std::pair<int,double>> goalPos0_;
    void addAllActions();

   // bool isGoal(GUSSPEVState* s);

public:
    /**
     * Constructs Ev Problem with specified parameters.
     */
   GUSSPEVProblem(int start_soc, int end_soc, int start_time, int reward_case, int exit_time_range, bool uniform_dist);

   GUSSPEVProblem(int start_soc, int end_soc, int start_time, int end_time , int reward_case);

    ~GUSSPEVProblem(){}
    std::vector<int> potential_goals; // depends only on exit time

    int timestep_interval(){return EV::time_interval_;}

     mlcore::State* absorbing_;
    /**
     * Overrides method from Problem.
     */
    virtual bool goal(mlcore::State* s) const;

    /**
     * Overrides method from Problem.
     */
    virtual std::list<mlcore::Successor> transition(mlcore::State* s,
                                                mlcore::Action* a);
    /**
     * Overrides method from Problem.
     */

     virtual double cost(mlcore::State* s, mlcore::Action* a) const;

    /**
     * Overrides method from Problem.
     */
    virtual bool applicable(mlcore::State* s, mlcore::Action* a) const;

    bool PeakHour(int i)
    {
        if(i >= 6 &&  i <= 10)
            return true;

        if(i >= 16 && i <= 20)
            return true;

        return false;
    }
    double end_soc(){return end_soc_;}

     bool isPeak(int timestep);
     bool GUSSPEVgoal(GUSSPEVState* s) const;

     virtual double Meancost(mlcore::State* s, mlcore::Action* a);

    /** Returns if the SSP state of it is a potential goal or not **/
    virtual bool isPotentialGoal(GUSSPEVState* evs);

    virtual void setInitBelief(std::vector<int> potential_goals, bool uniform_dist);

    /** returns observation if the state is a goal state or not**/
    virtual int getObservation(GUSSPEVState* evs) const;

    virtual std::vector<std::pair<int, double>> updateBelief(std::vector<std::pair<int,double>> curr_belief);

    /** randomly sets one goal to be true goal **/
    virtual void setTrueGoal(std::vector<int> potential_goals);

    virtual double getlowestCost()
    {
        GUSSPEVReward* reward = new GUSSPEVReward();
        double val = reward->getlowestCost();
        delete reward;
        return val;
    }
};

#endif // MPDLIB_GUSSPEVProblem_H
