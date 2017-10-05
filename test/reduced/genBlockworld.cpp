#include <cassert>
#include <ctime>
#include <list>
#include <sstream>
#include <string>
#include <typeinfo>

#include "../../include/domains/DummyState.h"
#include "../../include/domains/WrapperProblem.h"

#include "../../include/ppddl/mini-gpt/states.h"
#include "../../include/ppddl/mini-gpt/problems.h"
#include "../../include/ppddl/mini-gpt/domains.h"
#include "../../include/ppddl/mini-gpt/states.h"
#include "../../include/ppddl/mini-gpt/exceptions.h"
#include "../../include/ppddl/PPDDLHeuristic.h"
#include "../../include/ppddl/PPDDLProblem.h"

#include "../../include/solvers/DeterministicSolver.h"
#include "../../include/solvers/LAOStarSolver.h"
#include "../../include/solvers/Solver.h"
#include "../../include/solvers/VISolver.h"

#include "../../include/util/flags.h"
#include "../../include/util/general.h"

#include "../../include/Problem.h"

//#include "AStar.cpp"
//#include "AStar.h"

#include <sstream>
#include <algorithm>
#include <regex>

using namespace std;
using namespace mdplib;
using namespace mlppddl;
using namespace mlsolvers;


extern int yyparse();
extern FILE* yyin;
string current_file;
int warning_level = 0;

static int verbosity = 0;
static int k = 0;

mlcore::Problem* problem = nullptr;
mlcore::Heuristic* heuristic = nullptr;

/*
 * Parses the given PPDDL file, and returns true on success.
 */
static bool read_file( const char* ppddlFileName )
{
   	yyin = fopen( ppddlFileName, "r" );
     if( yyin == NULL ) {
        cout << "parser:" << ppddlFileName << ": " << strerror( errno ) << endl;
        return( false );
    }
    else {
        current_file = ppddlFileName;
        bool success;
        try {
            success = (yyparse() == 0);
        }
        catch( Exception exception ) {
            fclose( yyin );
            cout << exception << endl;
            return( false );
        }
        fclose( yyin );
        return( success );
    }
}

bool initPPDDL(string ppddlArgs)
{
   size_t pos_equals = ppddlArgs.find(":");
    assert(pos_equals != string::npos);
    string file = ppddlArgs.substr(0, pos_equals);
    string prob =
        ppddlArgs.substr(pos_equals + 1, ppddlArgs.size() - pos_equals);

    pair<state_t *,Rational> *initial = nullptr;

    if( !read_file( file.c_str() ) ) {
        cerr << "<main>: ERROR: couldn't read problem file `" << file << endl;
        return false;
    }
    problem_t* internalPPDDLProblem =
        (problem_t *)(problem_t::find(prob.c_str()));
    if( !internalPPDDLProblem ) {
        cerr << "<main>: ERROR: problem `" << prob << "' is not defined in file '" << file << "'" << endl;
        return false;
    }

    problem = new PPDDLProblem(internalPPDDLProblem);
//    heuristic = new mlppddl::PPDDLHeuristic(static_cast<PPDDLProblem*>(problem),mlppddl::FF);
//    problem->setHeuristic(heuristic);
}



/*  Additions by Sandhya - starts here until main*/
mlcore::State* getBestSucc_leastCost(mlcore::Problem* problem,
                               mlcore::State* s,
                               mlcore::Action* a)
{
double min = 10000.0;
mlcore::State* to_ret;
	for (mlcore::Successor sccr : problem->transition(s, a)) {
        if (sccr.su_prob >0 && sccr.su_state->cost() <min)
	{
		min  = sccr.su_state->cost();
		to_ret = sccr.su_state;
	}
    }
  return to_ret;
}

mlcore::State* getBestSucc_maxProb(mlcore::Problem* problem,
                               mlcore::State* s,
                               mlcore::Action* a)
{
double max = 0.0;
mlcore::State* to_ret;
	for (mlcore::Successor sccr : problem->transition(s, a)) {
        if (sccr.su_prob >0 && sccr.su_prob > max)
	{
		max  = sccr.su_prob;
		to_ret = sccr.su_state;
	}
    }
  return to_ret;
}

vector<string> split(const string &s, const char* delim){
    // to avoid modifying original string
    // first duplicate the original string and return a char pointer then free the memory
    vector<string> v,v1;
    std::string s1 = s;
	s1.erase(std::remove(s1.begin(), s1.end(), '['), s1.end());
	std::replace(s1.begin(), s1.end(), ']',' ');
    char * dup = strdup(s1.c_str());

    char * token = strtok(dup, delim);
    while(token != NULL){
        v.push_back(string(token));
        token = strtok(NULL, delim);
    }
    free(dup);

	 for (int i = 0; i < v.size(); i=i+2){
            v1.push_back(v[i]); }
	return v1;
}


std::string toString(mlcore:: State* s)
{
	//PPDDLState* state = (PPDDLState *) s;
	std::ostringstream buffer;
   	buffer << s;
    return buffer.str()  ;
}

mlcore::State* getGoal()
{
	for (mlcore::State* s : problem->states()) {
	if(problem->goal(s)){
		return s; }
	}
}

int getCount(string &s, vector<string> goal_config) {
	int count =0;
	for(int i=0;i< goal_config.size();i++) {
	std::size_t found = s.find(goal_config[i]);
  	if (found!=std::string::npos){
	count++;
	}
	}

return count;

}
int main(int argc, char* args[])
{
    register_flags(argc, args);

	bool print_states = false;
	bool print_actions = false;
	bool print_trans = false;
    // Reading flags.
    assert(flag_is_registered_with_value("domain"));
    string domainName = flag_value("domain");

    assert(flag_is_registered_with_value("problem"));

    if (flag_is_registered_with_value("v"))
       verbosity = stoi(flag_value("v"));


    if (flag_is_registered_with_value("k"))
        k = stoi(flag_value("k"));

    // Creating problem
   if (domainName == "ppddl") {
        string ppddlArgs = flag_value("problem");

        initPPDDL(ppddlArgs);
   }
 mlcore::StateSet reachableStates, tipStates;

	problem->generateAll(); // generate all states


	/********Printing details************/

	int states_count =0 ;
	for (mlcore::State* s : problem->states()) {
		states_count++;
     }
	cout<<"S="<< states_count <<"\n";

	int action_count =0;
	for (mlcore::Action* a : problem->actions()) {
	action_count++;
	}
	cout<<"A=" <<action_count <<"\n";

/*	cout<<"Init =" << problem->initialState() <<"\n";
	for (mlcore::State* s : problem->states()) {
		if(problem->goal(s))
	cout <<"Goal="<<s <<"\n";
	} */
/******** PRINT THE PROBLEM *******************/
/*	if(print_states){
	//int states_count =0 ;
	cout<<"printing states : \n";
	for (mlcore::State* s : problem->states()) {
	cout<<s <<"\n";
	}
	}

	if(print_actions){
	cout<<"printing actions : \n";
	for (mlcore::Action* a : problem->actions()) {
	cout<<a <<"\n";
	}
	}
 */
if(print_trans){
cout<<"printing transitions : \n";
 for (mlcore::Successor su : problem->transition(s, a)) {
	cout<< "s = " <<s << ";";
	cout<<"a = "<< a <<";";
	cout<< "j = " << su.su_state <<";";
	cout<< "prob = "<< su.su_prob <<"\n";
	}
	}
	}
}
}

//solving using lao* -- without cost adjustment

 double totalPlanningTime = 0.0;
    clock_t startTime = clock();
    bool costadjust = false;
    LAOStarSolver solver(problem);
    solver.solve(problem->initialState());
    clock_t endTime = clock();
    totalPlanningTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
    cout << "cost " << problem->initialState()->cost() <<
        " time " << totalPlanningTime << endl;



//solving using A* -- with cost adjustment


/*    double totalPlanningTime = 0.0;
    clock_t startTime = clock();
    bool costadjust = false;
    DeterministicSolver Solver(problem);
    Solver.solve(problem->initialState());
    clock_t endTime = clock();
    totalPlanningTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
    cout << " time " << totalPlanningTime << endl; */





//	cout<<"Printing best action:\n";
// 	for (mlcore::State* s : problem->states()) {
// 		if(s->bestAction() != nullptr)
// 		//cout<<s <<": has no best action\n";
// 		//else
// 		cout<<""<<s<< " : "<< s->bestAction() <<"\n";
// }



// solving using VI
/* double totalPlanningTime = 0.0;
    clock_t startTime = clock();
   VISolver vi(problem);
   vi.solve(problem->initialState());
   clock_t endTime = clock();
    totalPlanningTime += (double(endTime - startTime) / CLOCKS_PER_SEC);
    cout << "cost " << problem->initialState()->cost() <<
        " time " << totalPlanningTime << endl; */



	// cout <<goal_split;
  /* for (mlcore::State* s : problem->states()) {
	std::cout<<""<<s<<":"<<s->cost() <<"\n"; } */


//solve using ACAD
/* double totalPlanningTime = 0.0;
    clock_t startTime = clock();
	VISolver vi(problem,100000000, 1.0e-6);
   	vi.solve(problem->initialState());
	cout<<"VI Solved \n" << "***********************\n";  */

// get goal state config in string and predicates of the goal config
/*	mlcore::State* g = getGoal();
	string goal_config = toString(g);
	cout<<"goal State \n " << goal_config <<"\n";

	char delimit[]="()";
	vector<string> goal_split = split(goal_config,delimit);

	for (int i = 0; i < goal_split.size(); i++){
        cout<<goal_split[i]<< "\n";
	} */

	// convert state to string and get match count
/*	cout<<"initial state\n ";
	mlcore::State* s1 = problem->initialState();
	std::string s1_string = toString(s1);
	int count = getCount(s1_string, goal_split);
	cout<< s1_string <<"  match count = "<< count <<"\n"; */


/*	cout<<"Printing cost adjustment:\n";
	int max_count = 0;
	for (mlcore::State* s : problem->states()) {
		max_count++;
		if(max_count <= 200) {
		double bestCost = 0;
		int min = 100000.0;
		double bestQ =0;
		mlcore::Action* bestAction ;
		for (mlcore::Action* a : problem->actions()) {
		if(problem->applicable(s,a)){
		double qAction = std::min(10000.0, qvalue(problem, s, a));
		if(qAction <= min) {
			min = qAction;
			bestAction = a;
			bestQ = qAction;
		}
		}
		}
		  mlcore::State* j = getBestSucc_leastCost(problem, s, bestAction);
		//mlcore::State* j = getBestSucc_maxProb(problem, s, bestAction);
		double bestVal = j->cost();
		double new_cost = bestQ- bestVal;

		std::string s1_string = toString(s);
		int count = getCount(s1_string, goal_split);
		vector<string> s_vector = split(s1_string, delimit);
		double count_frac =  s_vector.size() - goal_split.size();
		count_frac = count_frac / count ;
//		cout<<""<<s << "best a"<< bestAction <<" : newcost = "<< new_cost <<" count = " << count <<" value = " << s->cost() << " fraction "<< count_frac <<"\n";

 	}
	} */
//fclose(pFile);
/*	cout<<"Printing cost adjustment:\n";
	for (mlcore::State* s : problem->states()) {
		index++;
		for (mlcore::Action* a : problem->actions()) {
		if(problem->applicable(s,a)){
		double qAction = std::min(10000.0, qvalue(problem, s, a));
		mlcore::State* j = getBestSucc(problem, s, a);
		double bestVal = j->cost();
		double new_cost = qAction- bestVal;
		cout<<""<<s << " a = "<< a <<" : cost = "<< new_cost <<"\n";
 		}
		}
	}
cout <<"index = "<< index <<"\n";*/

/////////////////////////////////////////////////

      // Releasing memory

    delete problem;
    return 0;
}
