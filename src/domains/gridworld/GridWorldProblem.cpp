#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "../../../include/Problem.h"
#include "../../../include/domains/gridworld/GridWorldProblem.h"
#include "../../../include/domains/gridworld/GridWorldState.h"
#include "../../../include/domains/gridworld/GridWorldAction.h"

using namespace std;

void GridWorldProblem::addAllActions()
{
    mlcore::Action* up = new GridWorldAction(gridworld::UP);
    mlcore::Action* down = new GridWorldAction(gridworld::DOWN);
    mlcore::Action* left = new GridWorldAction(gridworld::LEFT);
    mlcore::Action* right = new GridWorldAction(gridworld::RIGHT);
    actions_.push_front(up);
    actions_.push_front(down);
    actions_.push_front(left);
    actions_.push_front(right);
}


GridWorldProblem::GridWorldProblem() :
                    width_(0), height_(0), x0_(0), y0_(0), status0_(gridworld::UNKNOWN),
                    goals_(0), actionCost_(0.03)
{
    absorbing = new GridWorldState(this, -1, -1, 1);
    addAllActions();
}

GridWorldProblem::GridWorldProblem(const char* filename,
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
//                                                                                                     cout << " potential goal = " << pos.first << "," << pos.second << endl;
     }
    s0 = new GridWorldState(this, x0_, y0_, gridworld::FALSE);
    absorbing = new GridWorldState(this, -1, -1, gridworld::TRUE);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
    setGoal(potential_goals, uniform_dist);
}


GridWorldProblem::GridWorldProblem(
    int width, int height, int x0, int y0, int status0,
    PairDoubleMap* goals, double actionCost) :
        width_(width), height_(height), x0_(x0), y0_(y0), status0_(status0),
        actionCost_(actionCost)
{
    goals_ = new PairDoubleMap();
    for (auto const & goalEntry : *goals)
        (*goals_)[goalEntry.first] = goalEntry.second;

    s0 = new GridWorldState(this, x0_, y0_, status0_);
    absorbing = new GridWorldState(this, -1, -1, gridworld::TRUE);
    this->addState(s0);
    addAllActions();
}


GridWorldProblem::GridWorldProblem(int width, int height, int x0, int y0, int status0,
                     std::map<std::pair<int,int>, double> potential_goals_prob0, double actionCost):
                     width_(width), height_(height), x0_(x0), y0_(y0), status0_(status0),
                     actionCost_(actionCost)

{
   potential_goals_prob.clear();
   potential_goals_prob.insert(potential_goals_prob0.begin(), potential_goals_prob0.end());
   s0 = new GridWorldState(this, x0_, y0_, status0_);
   absorbing = new GridWorldState(this, -1, -1, gridworld::TRUE);
    this->addState(s0);
    addAllActions();
}

GridWorldProblem::GridWorldProblem(int width, int height,
                                   int x0, int y0, int status0,
                                   PairDoubleMap* goals, mlcore::Heuristic* h)
                                   : width_(width), height_(height),
                                     x0_(x0), y0_(y0), status0_(status0)
{
    goals_ = new PairDoubleMap();
    for (auto const & goalEntry : *goals)
        (*goals_)[goalEntry.first] = goalEntry.second;

    s0 = new GridWorldState(this, x0_, y0_, status0_);
    absorbing = new GridWorldState(this, -1, -1, gridworld::TRUE);
    this->addState(s0);
    heuristic_ = h;
    gamma_ = 1.0;
    addAllActions();
}

void GridWorldProblem::setGoal(std::vector<std::pair<int,int>> potential_goals, bool uniform_dist){
    goals_ = new PairDoubleMap();
    int index = rand() % potential_goals.size()-1;
    cout << "potential goal size = " << potential_goals.size() << endl;//<< " random number generated= " << index << endl;
    std::pair<int,int> pos = potential_goals.at(index);
    goals_->insert(std::make_pair(pos, 0.0));
    std::cout << "True Goal pos = " << pos.first << "," << pos.second << endl;

        /** If uniform_dist is true, then all potential goals are equally probable.
        ** Otherwise, the true goal is probable with 0.5 probability.
        ** All other goals are equally probable -- assuming one true goal.
        **/
    if(uniform_dist){
        for(int i = 0; i < potential_goals.size(); i++){
            std::pair<int,int> pos = potential_goals.at(i);
           // potential_goals_prob.insert({pos, (1.0/potential_goals.size())});
             potential_goals_prob.insert({pos, 0.5});
        }
    } else {
            for(int i = 0; i < potential_goals.size(); i++){
                std::pair<int,int> pos = potential_goals.at(i);
                 if(goals_->find(pos) != goals_->end()) // it's a goal.
                     potential_goals_prob.insert({pos, 0.8});
                else
                    potential_goals_prob.insert({pos, 0.5}); // because there is one tue goal
            }
    }
}

bool GridWorldProblem::gridGoal(GridWorldState* gws) const
{
   if(gws->status() == gridworld::TRUE)
        true;

    return false;
}

bool GridWorldProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}

int GridWorldProblem::getobservation(GridWorldState* gws){
    std::pair<int,int> pos(gws->x(),gws->y());
    if(goals_->find(pos) != goals_->end())
        return 1;

    // not a goal. remove from the hashmap.
    if(potential_goals_prob.find(pos) != potential_goals_prob.end())
        potential_goals_prob.erase(pos);

    return 0;
}

bool GridWorldProblem::isPotentialGoal(GridWorldState* gws){
    std::pair<int,int> pos(gws->x(),gws->y());
    if(potential_goals_prob.find(pos) != potential_goals_prob.end())
        return true;

    return false;
}

bool WasPotentialGoal(int x, int y, std::vector<std::pair<int,int>> potential_goals){
     for(int i = 0; i < potential_goals.size(); i++){
        std::pair<int,int> pos = potential_goals.at(i);
        if(pos.first == x && pos.second == y)
            return true;
     }
    return false;
}


std::list<mlcore::Successor>
GridWorldProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    GridWorldState* state = static_cast<GridWorldState *> (s);
    GridWorldAction* action = static_cast<GridWorldAction *> (a);

    std::list<mlcore::Successor> successors;
    int obs =  getobservation(state);

    int newstatus = gridworld::UNKNOWN; //by default
    if(isPotentialGoal(state))
        newstatus = (obs == 1)? gridworld::TRUE : gridworld::FALSE;

    if (s == absorbing || gridGoal(state) || newstatus == gridworld::TRUE) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }

    if(newstatus == gridworld::FALSE && state->status() != gridworld::FALSE){
         addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y(), newstatus,
                     1.0);
        return successors;
    }

    if (dead_ends.count(std::pair<int, int>(state->x(), state->y()))) {
        s->markDeadEnd();
        successors.push_front(mlcore::Successor(s, 1.0));
        return successors;
    }

    double probForward = 0.8;
    int numSuccessors = allDirections_ ? 3 : 2;
    double probSides = 0.2 / numSuccessors;
    if (action->dir() == gridworld::UP) {
           addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, newstatus,
                     probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), newstatus,
                     probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), newstatus,
                     probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->y(), 0,
                         state->x(), state->y() - 1, newstatus,
                         probSides);
        }
    }
        else if (action->dir() == gridworld::DOWN) {
        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, newstatus,
                     probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), newstatus,
                     probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), newstatus,
                     probSides);

        if (allDirections_) {
            addSuccessor(state, successors, height_ - 1, state->y(),
                         state->x(), state->y() + 1, newstatus,
                         probSides);
        }
    } else if (action->dir() == gridworld::LEFT) {
        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), newstatus,
                     probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, newstatus,
                     probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, newstatus,
                     probSides);

        if (allDirections_) {
            addSuccessor(state, successors, width_ - 1, state->x(),
                         state->x() + 1, state->y(), newstatus,
                         probSides);
        }
    } else if (action->dir() == gridworld::RIGHT) {
        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), newstatus,
                     probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, newstatus,
                     probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, newstatus,
                     probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->x(), 0,
                         state->x() - 1, state->y(), newstatus,
                         probSides);
        }
    }
    return successors;
}


double GridWorldProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    GridWorldState* gws = static_cast<GridWorldState *> (s);
    if (gridGoal(gws)) {
        std::pair<int,int> pos(gws->x(),gws->y());
        return (*goals_)[pos];
    }
    if (holes.count(std::pair<int, int> (gws->x(), gws->y())) != 0)
        return holeCost_ * actionCost_;

   GridWorldProblem* gwp = const_cast<GridWorldProblem*> (this);
  //  if(isPotentialGoal(gws,potential_goals) && gws->status() == gridworld::UNKNOWN)
    if(gwp->isPotentialGoal(gws) && gws->status() == gridworld::UNKNOWN)
        return 0.0;

    return actionCost_;
}


bool GridWorldProblem::applicable(mlcore::State* s, mlcore::Action* a) const
{
    return true;
}


void GridWorldProblem::addSuccessor(
    GridWorldState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, int newstatus, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
            GridWorldState *next = new GridWorldState(this, newx, newy, newstatus);
            successors.push_front(mlcore::Successor(this->addState(next), prob));
//        /** Check if the new status is valid. Cannot move to a potential goal
//        ** state with status as unknown after it was revealed to not be a goal.
//        ** This is to stay consistent with-- "cannot unsee goals" **/
//            std::pair<int,int>pos (newx,newy);
            // originally a potential goal, but not anymore

//            if(std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end() &&
//            potential_goals_prob.find(pos) == potential_goals_prob.end() && newstatus == gridworld::FALSE)
//            {
//                GridWorldState *next = new GridWorldState(this, newx, newy, newstatus);
//                successors.push_front(mlcore::Successor(this->addState(next), prob));
//            }
//            // not a potential goal
//            else if(std::find(potential_goals.begin(), potential_goals.end(), pos) == potential_goals.end())
//            {
//                GridWorldState *next = new GridWorldState(this, newx, newy, newstatus);
//                successors.push_front(mlcore::Successor(this->addState(next), prob));
//            }
//            // transitioning to goal.
//            else  if(std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end() &&
//            potential_goals_prob.find(pos) != potential_goals_prob.end() && newstatus == gridworld::TRUE)
//            {
//                GridWorldState *next = new GridWorldState(this, newx, newy, newstatus);
//                successors.push_front(mlcore::Successor(this->addState(next), prob));
//            }
    } else {
        successors.push_front(mlcore::Successor(state, prob));
    }
}
