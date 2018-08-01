#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "../../../include/Problem.h"
#include "../../../include/domains/gridworld/GUSSPGridWorldProblem.h"
#include "../../../include/domains/gridworld/GUSSPGridWorldState.h"
#include "../../../include/domains/gridworld/GUSSPGridWorldAction.h"
#include "../../../include/domains/gridworld/GridWorldAction.h"
#include "../../../include/domains/gridworld/GridWorldState.h"


void GUSSPGridWorldProblem::addAllActions()
{
    mlcore::Action* up = new GUSSPGridWorldAction(GUSSPgridworld::UP);
    mlcore::Action* down = new GUSSPGridWorldAction(GUSSPgridworld::DOWN);
    mlcore::Action* left = new GUSSPGridWorldAction(GUSSPgridworld::LEFT);
    mlcore::Action* right = new GUSSPGridWorldAction(GUSSPgridworld::RIGHT);
    actions_.push_front(up);
    actions_.push_front(down);
    actions_.push_front(left);
    actions_.push_front(right);
}
/**
** default constructor
**/
GUSSPGridWorldProblem::GUSSPGridWorldProblem() :
                        width_(0), height_(0), actionCost_(0.03),
                        x0_(0), y0_(0)
{
    std::pair<int, int> pos (-1,-1);
    goalPos0_.push_back(std::make_pair(pos,1.0));
    absorbing = new GUSSPGridWorldState(this, -1,-1, goalPos0_);
    addAllActions();
}

void GUSSPGridWorldProblem::setGoalPos(std::vector<std::pair<int,int>> potential_goals,
                                bool uniform_dist)
{
    if (uniform_dist){
     for (int i = 0; i < potential_goals.size(); ++i){
            std::pair<int,int> pos (potential_goals.at(i));
            goalPos0_.push_back(std::make_pair(pos, (1.0/potential_goals.size())));
        }
     }
}


GUSSPGridWorldProblem::GUSSPGridWorldProblem(const char* filename,
                                   double actionCost,
                                   double holeCost,
                                   bool allDirections,
                                   bool uniform_dist)
{
   std::ifstream myfile (filename);
   // Once the file is finished parsing, these will have correct values
    width_ = 0, height_ = 0;
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
    setGoalPos(potential_goals, uniform_dist);

    s0 = new GUSSPGridWorldState(this, x0_, y0_, goalPos0_);
    absorbing = new GUSSPGridWorldState(this, -1, -1, goalPos0_);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
    setTrueGoal(potential_goals);
                                                                                                std::cout <<" potential goal size = "
                                                                                                << potential_goals.size() << std::endl;
}
/** randomly sets one goal to be true goal **/
void GUSSPGridWorldProblem::setTrueGoal(std::vector<std::pair<int,int>> potential_goals)
{
    if (potential_goals.size() == 1)
        goals_->insert(std::make_pair(potential_goals.at(0), 0.0));
    else {
    int rn = rand() % potential_goals.size();
    goals_ = new PairDoubleMap();
    goals_->insert(std::make_pair(potential_goals.at(rn), 0.0));
    std::pair<int, int> pos = potential_goals.at(rn);
                                                                            //   std::cout << " true goal pos = " << pos.first << "," << pos.second << std::endl;
    }
}

bool GUSSPGridWorldProblem::GUSSPgridGoal(GUSSPGridWorldState* gws) const
{
    std::pair<int, int> curr_loc (gws->x(), gws->y());
    std::vector<std::pair<std::pair<int,int>, double>> goalPos = gws->goalPos();
     for (int i = 0; i < goalPos.size(); ++i){
            std::pair<std::pair<int, int>, double> pos = goalPos.at(i);
            std::pair<int, int> locs = pos.first;
            if (locs.first == curr_loc.first && locs.second == curr_loc.second && pos.second == 1.0)
                return true;
     }
     return false;
}


/**
* returns 1 if it is a true goal, 0 otherwise.
**/
int GUSSPGridWorldProblem::getObservation(mlcore::State * s) const
{
    GUSSPGridWorldState* gus = static_cast<GUSSPGridWorldState*> (s);
    std::pair<int,int> pos(gus->x(), gus->y());
    return goals_->find(pos) != goals_->end();
}

bool GUSSPGridWorldProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}
bool GUSSPGridWorldProblem::isPotentialGoal(mlcore::State* s)
{
    GUSSPGridWorldState* gws = static_cast<GUSSPGridWorldState*> (s);
    std::pair<int, int> pos(gws->x(), gws->y());
    return (std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end());
}
/** normalizes belief based on observation **/
std::vector<std::pair<std::pair<int,int>, double>> updateBelief(std::vector<std::pair<std::pair<int,int>, double>> curr_belief)
{
    double total = 0;
    std::vector<std::pair<std::pair<int,int>, double>> new_belief;
    for (auto i = curr_belief.begin(); i != curr_belief.end(); ++i)
    {
        std::pair<std::pair<int,int>, double> pos (*i);
        total+= pos.second;
    }
    for (auto i = curr_belief.begin(); i != curr_belief.end(); ++i)
    {
        std::pair<std::pair<int,int>, double> pos (*i);
        new_belief.push_back(std::make_pair(pos.first, (pos.second/total)));
    }
    return new_belief;
}

std::list<mlcore::Successor>
GUSSPGridWorldProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    GUSSPGridWorldState* state = static_cast<GUSSPGridWorldState *> (s);
    GUSSPGridWorldAction* action = static_cast<GUSSPGridWorldAction *> (a);

    std::vector<std::pair<std::pair<int,int>, double>> goalPos = state->goalPos();
    std::list<mlcore::Successor> successors;
    /** If goal or absorbing state **/

    if (s == absorbing || GUSSPgridGoal(state)) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }

    if (dead_ends.count(std::pair<int, int>(state->x(), state->y()))) {
        s->markDeadEnd();
        successors.push_front(mlcore::Successor(s, 1.0));
        return successors;
    }
    /** Otherwise check observation to update belief over goal configurations **/
    int obs =  getObservation(s);
    /** If potential goal, update beliefs based on observation **/
    if (isPotentialGoal(s)){
        for (auto i = goalPos.begin(); i != goalPos.end(); ++i){
            std::pair<std::pair<int,int>, double> pos (*i);
            std::pair<int,int> locs = pos.first;
            if (locs.first == state->x() && locs.second == state->y() && pos.second !=obs)
            {
                goalPos.erase(i);
                goalPos.push_back(std::make_pair(locs,obs));
                std::vector<std::pair<std::pair<int,int>, double>> goalPos_new = updateBelief(goalPos);
                addSuccessor(state, successors, 100, 0,
                             state->x(), state->y(), goalPos_new, 1.0);
              return successors;
             }
        }
    }
    /** Otherwise, update successor SSP states only. **/
    double probForward = 0.8;
    int numSuccessors = allDirections_ ? 3 : 2;
    double probSides = 0.2 / numSuccessors;

    if (action->dir() == GUSSPgridworld::DOWN) {
        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->goalPos(), probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->goalPos(), probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->y(), 0,
                         state->x(), state->y() - 1, state->goalPos(), probSides);
        }
    } else if (action->dir() == GUSSPgridworld::UP) {
        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->goalPos(), probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->goalPos(), probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, successors, height_ - 1, state->y(),
                         state->x(), state->y() + 1, state->goalPos(), probSides);
        }
    } else if (action->dir() == GUSSPgridworld::LEFT) {
        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->goalPos(), probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->goalPos(), probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, successors, width_ - 1, state->x(),
                         state->x() + 1, state->y(), state->goalPos(), probSides);
        }
    } else if (action->dir() == GUSSPgridworld::RIGHT) {
        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->goalPos(), probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->goalPos(), probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->x(), 0,
                         state->x() - 1, state->y(), state->goalPos(), probSides);
        }
    }
    return successors;
}

double GUSSPGridWorldProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    GUSSPGridWorldState* state = static_cast<GUSSPGridWorldState *> (s);
    if (GUSSPgridGoal(state))
        return 0;

    if (holes.count(std::pair<int, int> (state->x(), state->y())) != 0)
        return holeCost_ * actionCost_;

    return actionCost_;
}

bool GUSSPGridWorldProblem::applicable(mlcore::State* s, mlcore::Action* a) const {
    return true;
}

void GUSSPGridWorldProblem::addSuccessor(
    GUSSPGridWorldState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, std::vector<std::pair<std::pair<int, int>,double>> newgoalPos, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
        GUSSPGridWorldState *next = new GUSSPGridWorldState(this, newx, newy, newgoalPos);
        successors.push_front(mlcore::Successor(this->addState(next), prob));
    } else {
        successors.push_front(mlcore::Successor(state, prob));
    }
}
