#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "../../../include/Problem.h"
#include "../../../include/domains/SearchRescue/SearchRescueProblem.h"
#include "../../../include/domains/SearchRescue/SearchRescueState.h"
#include "../../../include/domains/SearchRescue/SearchRescueAction.h"

using namespace std;

void SearchRescueProblem::addAllActions()
{
    mlcore::Action* up = new SearchRescueAction(searchrescue::UP);
    mlcore::Action* down = new SearchRescueAction(searchrescue::DOWN);
    mlcore::Action* left = new SearchRescueAction(searchrescue::LEFT);
    mlcore::Action* right = new SearchRescueAction(searchrescue::RIGHT);
    mlcore::Action* save = new SearchRescueAction(searchrescue::SAVE);
    actions_.push_front(up);
    actions_.push_front(down);
    actions_.push_front(left);
    actions_.push_front(right);
    actions_.push_front(save);
}


SearchRescueProblem::SearchRescueProblem() :
                    width_(0), height_(0), x0_(0), y0_(0), victims0_(0), status0_(searchrescue::UNKNOWN),
                    actionCost_(0.03)
{
    absorbing = new SearchRescueState(this, -1, -1, -1, 1);
    addAllActions();
}

SearchRescueProblem::SearchRescueProblem(const char* filename,
                                   double actionCost,
                                   double holeCost,
                                   bool allDirections,
                                   bool uniform_dist)
{
    std::ifstream myfile (filename);
    // Once the file is finished parsing, these will have correct values
    width_ = 0, height_ = 0;
    potential_goals.clear();
    std::vector<std::pair<int,int>> goal_locations;
    if (myfile.is_open()) {
        std::string line;
        while ( std::getline (myfile, line) ) {
            for (width_ = 0; width_ < line.size(); width_++) {
                if (line.at(width_) == 'x') {
                    walls.insert(std::pair<int, int>(width_, height_));
                } else if (line.at(width_) == '@') {
                    holes.insert(std::pair<int, int>(width_, height_));
                } else if (line.at(width_) == 'G') {
                       potential_goals.push_back(std::pair<int, int>(width_, height_));
                } else if (line.at(width_) == 'D') {
                    dead_ends.insert(std::pair<int, int> (width_, height_));
                } else if (line.at(width_) == 'S') {
                    x0_ = width_;
                    y0_ = height_;
                  } else {
                    assert(line.at(width_) == '.');
                }
            }
            height_++;
        }
        myfile.close();
    } else {
        std::cerr << "Invalid file " << filename << std::endl;
        exit(-1);
    }
    actionCost_ = actionCost;
    holeCost_ = holeCost;
    allDirections_ = allDirections;

    int num_potential_goals = potential_goals.size();
    for(int i = 0; i < potential_goals.size(); i++){
        std::pair<int,int> pos = potential_goals.at(i);
     }
    s0 = new SearchRescueState(this, x0_, y0_, 0, searchrescue::FALSE); //no victims at start state
    absorbing = new SearchRescueState(this, -1, -1, -1, searchrescue::TRUE);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
    setGoal(potential_goals, uniform_dist);
}


SearchRescueProblem::SearchRescueProblem(
    int width, int height, int x0, int y0, int victims0, int status0,
    double actionCost) :
        width_(width), height_(height), x0_(x0), y0_(y0), victims0_(victims0), status0_(status0),
        actionCost_(actionCost)
{
    s0 = new SearchRescueState(this, x0_, y0_, victims0_, status0_);
    absorbing = new SearchRescueState(this, -1, -1, -1, searchrescue::TRUE);
    this->addState(s0);
    addAllActions();
}


SearchRescueProblem::SearchRescueProblem(int width, int height, int x0, int y0, int victims0, int status0,
                     std::map<std::pair<int,int>, int> potential_goals_victims0, double actionCost):
                     width_(width), height_(height), x0_(x0), y0_(y0), victims0_(victims0), status0_(status0),
                     actionCost_(actionCost)

{
   potential_goals_victims.clear();
   potential_goals_victims.insert(potential_goals_victims0.begin(), potential_goals_victims0.end());
   s0 = new SearchRescueState(this, x0_, y0_, victims0_, status0_);
   absorbing = new SearchRescueState(this, -1, -1, -1, searchrescue::TRUE);
    this->addState(s0);
    addAllActions();
}

SearchRescueProblem::SearchRescueProblem(int width, int height,
                                   int x0, int y0, int victims0, int status0,
                                   mlcore::Heuristic* h)
                                   : width_(width), height_(height),
                                     x0_(x0), y0_(y0), victims0_(victims0), status0_(status0)
{
    s0 = new SearchRescueState(this, x0_, y0_, victims0_, status0_);
    absorbing = new SearchRescueState(this, -1, -1, -1, searchrescue::TRUE);
    this->addState(s0);
    heuristic_ = h;
    gamma_ = 1.0;
    addAllActions();
    clearedStates.clear();
}
/** Generates a random number between 0 and 5 for the number of victims in each
** potential goal location.
**/
void SearchRescueProblem::setGoal(std::vector<std::pair<int,int>> potential_goals, bool uniform_dist){
    int totalVictims = 0;
    for(int i = 0; i < potential_goals.size(); i++){
            double prob = rand();
            int index = (prob < 0.5)? 0 : (rand() % 5) +1;
 //           int index =  (prob < 0.5)? 0 : 2;
            std::pair<int,int> pos = potential_goals.at(i);
            potential_goals_victims.insert({pos, index});
            totalVictims = totalVictims + index;
                                                                                        cout << " pos = " << pos.first << ","
                                                                                        << pos.second << " victims= " << index << endl;
    }
    maxVictims_ = totalVictims;
    std::cout << "totalVictims = " << totalVictims << endl;
}

bool SearchRescueProblem::SRGoal(SearchRescueState* srs) const
{
   if(srs->victims() == maxVictims_ && srs->status() == searchrescue::TRUE)
        return true;

     return false;
}

bool SearchRescueProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}

int SearchRescueProblem::getobservation(SearchRescueState* srs){
    std::pair<int,int> pos(srs->x(),srs->y());
    if(potential_goals_victims.find(pos) != potential_goals_victims.end()){
        return potential_goals_victims.at(pos);
    }
   return -1;
}

bool SearchRescueProblem::isPotentialGoal(SearchRescueState* srs){
     std::pair<int,int> pos(srs->x(),srs->y());
     if (std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end())
        return true;

    return false;
}

void printSavedStates(std::vector<std::pair<int,int>> v){
for (const auto i: v)
  std::cout << i.first << "," << i.second << endl;
}

std::list<mlcore::Successor>
SearchRescueProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    SearchRescueState* state = static_cast<SearchRescueState *> (s);
    SearchRescueAction* action = static_cast<SearchRescueAction *> (a);

    std::list<mlcore::Successor> successors;

    if (s == absorbing || SRGoal(state)) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }
    if (dead_ends.count(std::pair<int, int>(state->x(), state->y()))) {
        s->markDeadEnd();
        successors.push_front(mlcore::Successor(s, 1.0));
        return successors;
    }

    int victims =  getobservation(state);
    int newstatus = searchrescue::UNKNOWN; //by default

    if (state->status() == searchrescue::UNKNOWN) // figuring if its a goal
    {
        if (victims > 0)
            newstatus = searchrescue::TRUE;
        if (victims == 0)
            newstatus = searchrescue::FALSE;
    }
    std::pair<int,int> pos(state->x(),state->y());
    /** deterministic action. Update number of saved victims.
    ** Also prevents saving the same person again!
    **/
   if (action->dir() == searchrescue::SAVE && victims > 0  && state->status() != searchrescue::FALSE
        && state->victims() + victims <= maxVictims()
 //       && !(std::find(clearedStates.begin(), clearedStates.end(), pos) != clearedStates.end())
        ) {
        int newvictims =  state->victims() + victims;
        addSuccessor(state, successors, width_ + 1, 0,
                     state->x(), state->y(), newvictims,
                     searchrescue::TRUE, 1.0);

        // all victims saved
        clearedStates.push_back(pos);
 //       cout << "adding " << pos.first << ", " << pos.second << " to saved list. new victims = " << newvictims << endl;
        return successors;
    }

    double probForward = 0.8;
    int numSuccessors = allDirections_ ? 3 : 2;
    double probSides = 0.2 / numSuccessors;
    if (action->dir() == searchrescue::UP) {
           addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->victims(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->victims(),
                     newstatus, probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->victims(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->y(), 0,
                         state->x(), state->y() - 1, state->victims(),
                         newstatus, probSides);
        }
    }
        else if (action->dir() == searchrescue::DOWN) {
        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->victims(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->victims(),
                     newstatus, probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->victims(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, height_ - 1, state->y(),
                         state->x(), state->y() + 1, state->victims(),
                         newstatus, probSides);
        }
    } else if (action->dir() == searchrescue::LEFT) {
        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->victims(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->victims(),
                     newstatus, probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->victims(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, width_ - 1, state->x(),
                         state->x() + 1, state->y(), state->victims(),
                         newstatus, probSides);
        }
    } else if (action->dir() == searchrescue::RIGHT) {
        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->victims(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->victims(),
                     newstatus, probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->victims(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->x(), 0,
                         state->x() - 1, state->y(), state->victims(),
                         newstatus, probSides);
        }
    }

    return successors;
}

double SearchRescueProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    SearchRescueAction* sra = static_cast<SearchRescueAction *> (a);
    if (sra->dir() == searchrescue::SAVE)
        return 2.0 * actionCost_;

    SearchRescueState* srs = static_cast<SearchRescueState *> (s);
    if (SRGoal(srs)) {
        return 0;
    }
    if (holes.count(std::pair<int, int> (srs->x(), srs->y())) != 0)
        return holeCost_ * actionCost_;

   SearchRescueProblem* srp = const_cast<SearchRescueProblem*> (this);
   if(srp->isPotentialGoal(srs) && srs->status() == searchrescue::UNKNOWN)
        return 0.0;

    return actionCost_;
}


bool SearchRescueProblem::applicable(mlcore::State* s, mlcore::Action* a) const
{
    SearchRescueState* srs = static_cast<SearchRescueState *> (s);
    SearchRescueProblem* srp = const_cast<SearchRescueProblem*> (this);
    SearchRescueAction* sra = static_cast<SearchRescueAction *> (a);
    std::pair<int,int> pos (srs->x(),srs->y());
    if (sra->dir() == searchrescue::SAVE) // applicable in potential goals
    {
       if(srp->isPotentialGoal(srs) && srs->status() != searchrescue::FALSE && srp->getobservation(srs) > 0
  //          && !(std::find(clearedStates.begin(), clearedStates.end(), pos) != clearedStates.end())
          )
            return true;
        else
            return false;
    }
    // move action is always applicable
        return true;
}

void SearchRescueProblem::addSuccessor(
    SearchRescueState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, int newvictims, int newstatus, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
            SearchRescueState *next = new SearchRescueState(this, newx, newy, newvictims, newstatus);
            successors.push_front(mlcore::Successor(this->addState(next), prob));
    } else {
        successors.push_front(mlcore::Successor(state, prob));
    }
}
