#include<sstream>
#include <string>
#include<vector>
#include "Features_state.cpp"

using namespace std;

namespace acad{
  
     std::vector<std::string> onTable_state(std::vector<string> state_pred) {
        vector<string> ontable_list;
     for (int i=0; i<state_pred.size(); i++) {
        if(state_pred.at(i).find("on-table") != std::string::npos)    
        {
         string s = state_pred.at(i).substr(state_pred.at(i).find(" ")+1,state_pred.at(i).find(")")-1);
         s = s.substr(0,2);
         ontable_list.push_back(s);
       //  cout<<s<<endl;
        }
      }  
      return ontable_list;
     }
   
   string onTable_action(string action)
    {
        istringstream iss(action);
        string sub;
        do
        {
         iss >> sub;
        } while (iss);
      return sub.substr(0,2);
    }
    
    double frac_satisfied(std::vector<string> state_pred, mlcore::Problem* problem)
    {
        vector<string> goal_predicates;
        string goal_state_string;
        for (mlcore::State* s : problem->states()) {
        if(problem->goal(s))
                goal_state_string = acad::toString_state(s);
         }
        goal_predicates = acad::getPredicates(goal_state_string);
        
        double match=0;
         for (int i=0; i<goal_predicates.size(); i++) {
           if(std::find(state_pred.begin(),state_pred.end(),goal_predicates.at(i))!= state_pred.end())
            match++;
         } 
 
    return (match/goal_predicates.size());
    }
}

