#ifndef MPDLIB_EVPROBLEM_H
#define MPDLIB_EVPROBLEM_H

#include <unordered_set>

#include "../../Problem.h"
#include "../../State.h"
#include "../../Action.h"
#include "../../util/general.h"

#include "EVState.h"
#include "EVAction.h"
#include "EVReward.h"

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

class EVProblem : public mlcore::Problem
{
private:
    int start_soc_;
    int end_soc_;
    double start_time_;
    double end_time_;
    int rewardCase_;
    int horizon_;

    void addAllActions();

   // bool isGoal(EVState* s);

    /* If true, the flat transition function will be used */
    bool useFlatTransition_ = false;

     /*
     * A flat transition function where every action has the same number
     * of successors in all states.
     */
    virtual std::list<mlcore::Successor> flatTransition(mlcore::State* s,
                                                        mlcore::Action* a);

public:
    /**
     * Constructs Ev Problem with specified parameters.
     */
   EVProblem(int start_soc, int end_soc, int start_time, int end_time, int reward_case);

    ~EVProblem(){}

    int timestep_interval(){return EV::time_interval_;}

     /*
     * Returns the number of successors of the action, under the flat
     * transition, which is the same for every state but the initial state.
     */
    int numSuccessorsAction(EVAction* a);

    void useFlatTransition(bool value) { useFlatTransition_ = value; }

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

   /**
    * Generates all states that can be reached from s0 and stores them.
    *
    * WARNING: This method changes the value of the 'bits' variable for all
    * states in the state set. Do not call this method while other code is using
    * these values.
    */
   // virtual void generateAll();

    bool PeakHour(int i)
    {
        if(i >= 6 &&  i <= 10)
            return true;

        if(i >= 16 && i <= 20)
            return true;

        return false;
    }
    double end_soc(){return end_soc_;}

    virtual double get_comm_factor(EVState* evs, EVState* evj);

     virtual double get_factor(EVState* evs, EVState* evj);
     bool isPeak(int timestep);

      virtual double Meancost(mlcore::State* s, mlcore::Action* a);
};

#endif // MPDLIB_EVPROBLEM_H
