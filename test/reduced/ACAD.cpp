#include "../../include/solvers/VISolver.h"
#include<stdio.h>

using namespace mlcore;
using namespace mlsolvers;
using namespace std;

int getBestSucc(mlcore::Problem* problem,
                               mlcore::State* s,
                               mlcore::Action* a)
{
double min = 10000.0;
int to_ret =-1;
	for (mlcore::Successor sccr : problem->transition(s, a)) {
        if (sccr_prob >0 && sccr_state->cost() <min)
	{
		min  = su.su_state->cost();
		to_ret = su.su_state;
	}
    }
  return to_ret;
}

int determinize(mlcore::Problem* problem)
{	
	cout<<"Printing cost adjustment:\n";
	for (mlcore::State* s : problem->states()) {
		int a = s->bestAction();
		double qAction = std::min(mdplib::dead_end_cost, qvalue(problem, s, a));
		mlcore::State* j = getBestSucc(problem, s, a);
		double bestVal = j->cost();
		double new_cost = qAction - bestVal;
		cout<<""<<s << "best a"<<a <<" : cost = "<< new_cost[s][a] <<"\n";
	}
	return 1;
}
