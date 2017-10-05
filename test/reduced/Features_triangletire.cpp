#include<sstream>
#include <string>
#include<vector>
//#include "Features_state.cpp"
using namespace std;

namespace acad{

     std::vector<std::string> sparein_state(std::vector<string> state_pred) {
        vector<string> ontable_list;
     for (int i=0; i<state_pred.size(); i++) {
        if(state_pred.at(i).find("spare-in") != std::string::npos)
        {
         string s = state_pred.at(i).substr(state_pred.at(i).find(" ")+1,state_pred.at(i).find(")")-1);
         s = s.substr(0,s.length()-2);
         ontable_list.push_back(s);
    //     cout<<s<<endl;
        }
      }
      return ontable_list;
     }
     string dest_action(string action)
    {
        istringstream iss(action);
        string sub;
        do
        {
         iss >> sub;
        } while (iss);
      return sub.substr(0,2);
    }
    string vehicle_loc(std::vector<string> state_pred)
    {
        string loc;
        for (int i=0; i<state_pred.size(); i++) {
        if(state_pred.at(i).find("vehicle-at") != std::string::npos)
        {
           loc = state_pred.at(i).substr(state_pred.at(i).find(" ")+1,state_pred.at(i).find(")")-1);
           loc= loc.substr(0,loc.length()-3);
        //    cout<<loc<<endl;
        }
       return loc;
      }
    }
}
