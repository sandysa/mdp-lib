#include "../../include/ppddl/PPDDLAction.h"
#include "../../include/ppddl/PPDDLProblem.h"
#include "../../include/ppddl/PPDDLState.h"

#include "../../include/ppddl/PPDDLHeuristic.h"
#include "../../include/ppddl/PPDDLProblem.h"

#include "../../include/Problem.h"
#include "Features_blocksworld.cpp"
#include "Features_triangletire.cpp"

#include "../../src/ppddl/PPDDLAction.cpp"
#include<sstream>
#include <string>
#include<algorithm>

using namespace std;
using namespace mdplib;
using namespace mlppddl;


namespace acad
{
  double new_cost_blocksworld(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem) 
{
        PPDDLAction* action = (PPDDLAction *) a;
        PPDDLState* state = (PPDDLState *) s;
        string s_str = acad::toString_state(s);
        std::vector<string> state_predicates =  acad::getPredicates(s_str);
        vector<string> on_table_state = onTable_state(state_predicates);
        double match = acad::frac_satisfied(state_predicates,problem);
    std::ostringstream os;
	for(mlcore::Action* a1: problem->actions()){
		if(a1==a && action->pAction()->enabled(*state->pState())){
		 string a_string = acad::toString_action(a1);
         string base_tower =  onTable_action(a_string);
         if(a_string.find("pick-up-from-table")!=std::string::npos) 
                     return 1.1;
                         
         if(a_string.find("put-on-block")!=std::string::npos) 
         {
                    if(match >= 0.4 )
                            return 1.7; //was 1.7
                        else 
                            return 1.1; //was 1.1   
         }
             
          if(a_string.find("pick-tower")!=std::string::npos ) 
          {
             if(std::find(on_table_state.begin(),on_table_state.end(),base_tower)!= on_table_state.end())
             {  
                  if(match <= 0.5)
                     return 2;
                  else
                        return 1.5;
              }
              else
                  return 0.4;
          }
          if(a_string.find("pick-up b")!=std::string::npos)         
          {
             if(std::find(on_table_state.begin(),on_table_state.end(),base_tower)!= on_table_state.end())
             {                   
                    if(match < 0.2) return 1.6;
                    else return .6;
             }
             else
             {
                 if(match <= 0.2 )
                            return 1;
                        else 
                            return .8;
             }
          }
          if(a_string.find("put-tower-on-block")!=std::string::npos) 
          {
               if(std::find(on_table_state.begin(),on_table_state.end(),base_tower)!= on_table_state.end())
               {
                     if(match<0.2)
                         return .7;
                     if(match>=0.2 && match <0.4)
                         return 1.6;
                     else if(match>= 0.4)
                         return 0.7;
               }
                else
                {
                    if(match <= 0.2 )
                            return 1.1;
                        else if(match > 0.2 && match <= 0.4)
                            return 2;
                        else
                            return 4;   
                }
           }
    	}
	}
   	return 1;
}


double new_cost_triangletire(mlcore::State* s, mlcore::Action* a, mlcore::Problem* problem) 
{
    PPDDLAction* action = (PPDDLAction *) a;
        PPDDLState* state = (PPDDLState *) s;
        string s_str = acad::toString_state(s);
        std::vector<string> state_predicates =  acad::getPredicates(s_str);
        vector<string> spare_in_state = sparein_state(state_predicates);
        bool hasSpare= false, ntft =false, spdest= false, spotherloc = false, spsource=false;
        string vehicle_loc = acad::vehicle_loc(state_predicates);
   //     printvector(spare_in_state);
        
        if(s_str.find("not-flattire")!=std::string::npos)
              ntft=true;
         if(s_str.find("hasspare")!=std::string::npos)
            hasSpare=true;
          if(std::find(spare_in_state.begin(),spare_in_state.end(),vehicle_loc)!= spare_in_state.end())    
              spsource = true;
        for(mlcore::Action* a1: problem->actions()){
            if(a1==a && action->pAction()->enabled(*state->pState())){
                string a_string = acad::toString_action(a1);
                    if(a_string.find("move-car")!=std::string::npos) 
                    {
                        string dest = acad::dest_action(a_string);
                         if(std::find(spare_in_state.begin(),spare_in_state.end(),dest)!= spare_in_state.end()) 
                             spdest = true;
                         else spotherloc=true; //check conditions for this
                    }
                }
            } 
            
           if(hasSpare && ntft && spdest )
                    return 1;
                if(ntft&& spsource)
                    return -5;
                if(ntft && spotherloc && spdest)
                    return 0.5;
                else if(ntft && spdest)
                    return 1;
                
                if(hasSpare && ntft)
                    return 1.5;
            //    if(spotherloc && ntft && curr_loc.trim().equals("n12") )
            //        return 1;
                if(spotherloc && ntft)
                    return 5;
                
                else if(ntft && !spdest && !hasSpare)
                    return 1000;  
            
     return 1;       
}
}
