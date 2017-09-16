#include<sstream>
#include <string>
#include<algorithm>

#include "../../include/ppddl/PPDDLAction.h"
#include "../../include/ppddl/PPDDLProblem.h"
#include "../../include/ppddl/PPDDLState.h"

#include "../../include/ppddl/PPDDLHeuristic.h"

#include "../../include/Problem.h"

using namespace std;


namespace acad{
 std::string toString_state(mlcore:: State* s)
    {
    std::ostringstream buffer; 
    buffer << s;
    return buffer.str()  ;
    }

    std::string toString_action(mlcore::Action* a1)
    {
    std::ostringstream buffer; 
    buffer << a1;
    return buffer.str() ;
    } 
    std::vector<std::string> getPredicates(string s)
    {
        vector<string> list;
        while(s.length()>1)
        {
            int pos1 = s.find("(");
            int pos2 = s.find(")")-2;
            if(pos1!= std::string::npos && pos2!= std::string::npos)
                {
                    string sub = s.substr(pos1,pos2);
                    //  cout<<sub<<endl;
                    list.push_back(sub);
                    s= s.substr(pos2+3,s.length());
                }
            else
                { 
                    break;
                }
          }
          return list;
    }
    void printvector(std::vector<string> pred)
    {
      for(int i=0;i<pred.size();i++) {
         cout<<pred.at(i)<<"  ";   
        }   
            
        
    }
    

}