#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "../../../include/Problem.h"
#include "../../../include/domains/rocksample/GUSSPRockSampleProblem.h"
#include "../../../include/domains/rocksample/GUSSPRockSampleState.h"
#include "../../../include/domains/rocksample/GUSSPRockSampleAction.h"

using namespace std;

void GUSSPRockSampleProblem::addAllActions()
{
    mlcore::Action* up = new GUSSPRockSampleAction(rocksample::UP);
    mlcore::Action* down = new GUSSPRockSampleAction(rocksample::DOWN);
    mlcore::Action* left = new GUSSPRockSampleAction(rocksample::LEFT);
    mlcore::Action* right = new GUSSPRockSampleAction(rocksample::RIGHT);
    mlcore::Action* sample = new GUSSPRockSampleAction(rocksample::SAMPLE);
    actions_.push_front(up);
    actions_.push_front(down);
    actions_.push_front(left);
    actions_.push_front(right);
    actions_.push_front(sample);
}


GUSSPRockSampleProblem::GUSSPRockSampleProblem() :
                    width_(0), height_(0), x0_(0), y0_(0), sampledRocks0_(0), actionCost_(0.03)
{
    std::pair<int, int> pos (-1,-1);
    goalPos0_.push_back(std::make_pair(pos,1.0));
    absorbing = new GUSSPRockSampleState(this, -1, -1, -1, goalPos0_);
    addAllActions();
}

GUSSPRockSampleProblem::GUSSPRockSampleProblem(const char* filename,
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
    setGoalProb(potential_goals, uniform_dist);
    s0 = new GUSSPRockSampleState(this, x0_, y0_, 0, goalPos0_); //no rocks at start state
    absorbing = new GUSSPRockSampleState(this, -1, -1, -1, goalPos0_);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
    setTrueGoal(potential_goals);
}

void GUSSPRockSampleProblem::setGoalProb(std::vector<std::pair<int,int>> potential_goals,
                                bool uniform_dist)
{
    if (uniform_dist){
     for (int i = 0; i < potential_goals.size(); ++i){
            std::pair<int,int> pos (potential_goals.at(i));
            goalPos0_.push_back(std::make_pair(pos, (1.0/potential_goals.size())));
        }
     }
}


/** Generates a random number between 0 and 5 for the number of victims in each
** potential goal location.
**/
void GUSSPRockSampleProblem::setTrueGoal(std::vector<std::pair<int,int>> potential_goals){
   goals_ = new PairDoubleMap();
       /** If there is only one potential goal, then set that to be true goal **/
    if (potential_goals.size() == 1){
        goals_->insert(std::make_pair(potential_goals.at(0), 0.0));
        return;
     }
     else {
        int totalSamples = 0;
//        for(int i = 0; i < potential_goals.size(); i++){
//            int index = rand() %2;
//            if (index == 1)
//                goals_->insert(std::make_pair(potential_goals.at(i), 0.0));
//
//            totalSamples += index;
//         }

              //Updated for robot experiment:
            int index  =  rand() % potential_goals.size();
            goals_->insert(std::make_pair(potential_goals.at(index), 0.0));
            totalSamples += 1;
            std::pair<int,int> goalpos =  potential_goals.at(index);
                                                                                           std::cout << "potential goal size = " << potential_goals.size()
                                                                                        << " totalSamples = " << totalSamples << endl;
                                                                                        std::cout << "True goal:(" << goalpos.first <<"," <<goalpos.second <<")" << std::endl;
          // if no goal set, then set first to be a goal.
        if(goals_->size() == 0)
            goals_->insert(std::make_pair(potential_goals.at(0), 0.0));
       }
}

bool GUSSPRockSampleProblem::GUSSPRSGoal(GUSSPRockSampleState* rss) const
{
   return rss->sampledRocks() == 1;
}

bool GUSSPRockSampleProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}

int GUSSPRockSampleProblem::getObservation(GUSSPRockSampleState* rss){
    std::pair<int,int> pos(rss->x(),rss->y());
     if(goals_->find(pos) != goals_->end())
        return 1;
   return 0;
}

int GUSSPRockSampleProblem::getObservation(int x, int y){
    std::pair<int,int> pos(x,y);
     if(goals_->find(pos) != goals_->end())
        return 1;
   return 0;
}


bool GUSSPRockSampleProblem::isPotentialGoal(GUSSPRockSampleState* rss){
     std::pair<int,int> pos(rss->x(),rss->y());
     if (std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end())
        return true;

    return false;
}

bool GUSSPRockSampleProblem::isPotentialGoal(int x, int y){
     std::pair<int,int> pos(x,y);
     if (std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end())
        return true;

    return false;
}

/** normalizes belief based on observation **/
std::vector<std::pair<std::pair<int,int>, double>> GUSSPRockSampleProblem::updateBelief
                                                 (std::vector<std::pair<std::pair<int,int>, double>> curr_belief)
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
GUSSPRockSampleProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    GUSSPRockSampleState* state = static_cast<GUSSPRockSampleState *> (s);
    GUSSPRockSampleAction* action = static_cast<GUSSPRockSampleAction *> (a);
    std::vector<std::pair<std::pair<int,int>, double>> goalPos = state->goalPos();
    std::list<mlcore::Successor> successors;

    if (s == absorbing || GUSSPRSGoal(state)) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }
    if (dead_ends.count(std::pair<int, int>(state->x(), state->y()))) {
        s->markDeadEnd();
        successors.push_front(mlcore::Successor(s, 1.0));
        return successors;
    }
    /** Otherwise check observation to update belief over goal configurations **/
    int obs =  getObservation(state);

//    /** If potential goal, update beliefs based on observation **/
//    if (isPotentialGoal(state)){
//        for (auto i = goalPos.begin(); i != goalPos.end(); ++i){
//            std::pair<std::pair<int,int>, double> pos (*i);
//            std::pair<int,int> locs = pos.first;
//            if (locs.first == state->x() && locs.second == state->y() && pos.second !=obs && obs == 0)
//            {
//                goalPos.erase(i);
//                goalPos.push_back(std::make_pair(locs,obs));
//                std::vector<std::pair<std::pair<int,int>, double>> goalPos_new = updateBelief(goalPos);
//                addSuccessor(state, successors, 100, 0,
//                             state->x(), state->y(), state->sampledRocks(), goalPos_new, 1.0);
////                                                                                            std::cout << "updating potential goal for " << state <<
////                                                                                            " obs  = " << obs << std::endl;
//              return successors;
//             }
//        }
//    }
    /** SAMPLE action: **/
    if (action->dir() == rocksample::SAMPLE && obs == 1 && obs != state->sampledRocks())
    {
        addSuccessor(state, successors, 100, 0,
                             state->x(), state->y(), obs, goalPos, 1.0);
        return successors;
    }
    /** return if the transition is found in the cache **/
        int idAction = action->hashValue();
        std::vector<mlcore::SuccessorsList>* allSuccessors = state->allSuccessors();
        if (!allSuccessors->at(idAction).empty()) {
            return allSuccessors->at(idAction);
        }

     /** Otherwise, update successor SSP states only. **/
    double probForward = 0.8;
    int numSuccessors = allDirections_ ? 3 : 2;
    double probSides = 0.2 / numSuccessors;


    if (action->dir() == rocksample::UP) {
        addSuccessor(state, allSuccessors, idAction, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     state->goalPos(), probForward);

        addSuccessor(state, allSuccessors, idAction, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probSides);

        addSuccessor(state, allSuccessors, idAction, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, allSuccessors, idAction,state->y(), 0,
                         state->x(), state->y() - 1, state->sampledRocks(),
                         state->goalPos(), probSides);
        }
    } else if (action->dir() == rocksample::DOWN) {
        addSuccessor(state, allSuccessors, idAction, state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     state->goalPos(), probForward);

        addSuccessor(state, allSuccessors, idAction, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probSides);

        addSuccessor(state, allSuccessors, idAction, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, allSuccessors, idAction, height_ - 1, state->y(),
                         state->x(), state->y() + 1, state->sampledRocks(),
                         state->goalPos(), probSides);
        }
    } else if (action->dir() == rocksample::LEFT) {
        addSuccessor(state, allSuccessors, idAction, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probForward);

        addSuccessor(state, allSuccessors, idAction,state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     state->goalPos(), probSides);

        addSuccessor(state, allSuccessors, idAction, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     state->goalPos(), probSides);

        if (allDirections_) {
                addSuccessor(state, allSuccessors, idAction, width_ - 1, state->x(),
                         state->x() + 1, state->y(), state->sampledRocks(),
                         state->goalPos(), probSides);
        }
    } else if (action->dir() == rocksample::RIGHT) {
        addSuccessor(state, allSuccessors, idAction, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     state->goalPos(), probForward);

        addSuccessor(state, allSuccessors, idAction, state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     state->goalPos(), probSides);

        addSuccessor(state, allSuccessors, idAction, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     state->goalPos(), probSides);

        if (allDirections_) {
            addSuccessor(state, allSuccessors, idAction, state->x(), 0,
                         state->x() - 1, state->y(), state->sampledRocks(),
                         state->goalPos(), probSides);
        }
    }
    return allSuccessors->at(idAction);
}

double GUSSPRockSampleProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    GUSSPRockSampleAction* rsa = static_cast<GUSSPRockSampleAction *> (a);
    GUSSPRockSampleState* rss = static_cast<GUSSPRockSampleState *> (s);
    GUSSPRockSampleProblem* rsp = const_cast<GUSSPRockSampleProblem*> (this);

    if (GUSSPRSGoal(rss))
        return 0;

    if (rsa->dir() == rocksample::SAMPLE)
    {
        if (rsp->getObservation(rss) > 0)
            return 2.0 * actionCost_;
        else
            return 10.0 * actionCost_;
    }

    if (holes.count(std::pair<int, int> (rss->x(), rss->y())) != 0)
        return holeCost_ * actionCost_;

    return actionCost_;
}


bool GUSSPRockSampleProblem::applicable(mlcore::State* s, mlcore::Action* a) const
{
    GUSSPRockSampleState* rss = static_cast<GUSSPRockSampleState *> (s);
    GUSSPRockSampleProblem* srp = const_cast<GUSSPRockSampleProblem*> (this);
    GUSSPRockSampleAction* sra = static_cast<GUSSPRockSampleAction *> (a);
    std::pair<int,int> pos (rss->x(),rss->y());
    if (sra->dir() == rocksample::SAMPLE) // applicable in potential goals
    {
       return srp->isPotentialGoal(rss);
    }
    // move action is always applicable
        return true;
}

void GUSSPRockSampleProblem::addSuccessor(
    GUSSPRockSampleState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, int newsamples, std::vector<std::pair<std::pair<int, int>,double>> newgoalPos, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
            GUSSPRockSampleState *next = new GUSSPRockSampleState(this, newx, newy, newsamples, newgoalPos);
            successors.push_front(mlcore::Successor(this->addState(next), prob));
     } else {
          successors.push_front(mlcore::Successor(state, prob));
    }
}

void GUSSPRockSampleProblem::addSuccessor(
    GUSSPRockSampleState* state, std::vector<mlcore::SuccessorsList>* allSuccessors, int idAction,
    int val, int limit, int newx, int newy, int newsamples, std::vector<std::pair<std::pair<int, int>,double>> newgoalPos, double prob)
{
       /** Otherwise check observation to update belief over goal configurations **/
    int obs =  getObservation(newx, newy);
    std::vector<std::pair<std::pair<int, int>,double>> newgoalPoscp  = newgoalPos;
    /** If potential goal, update beliefs based on observation **/
    if (isPotentialGoal(newx, newy)){
        for (auto i = newgoalPoscp.begin(); i != newgoalPoscp.end(); ++i){
            std::pair<std::pair<int,int>, double> pos (*i);
            std::pair<int,int> locs = pos.first;
            if (locs.first == newx && locs.second == newy && pos.second !=obs && obs == 0)
            {
                newgoalPoscp.erase(i);
                newgoalPoscp.push_back(std::make_pair(locs,obs));
                newgoalPos = updateBelief(newgoalPoscp);
                break;
             }
        }
    }

    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
            GUSSPRockSampleState *next = new GUSSPRockSampleState(this, newx, newy, newsamples, newgoalPos);
            allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(next), prob));

     } else {
        allSuccessors->at(idAction).push_back(mlcore::Successor(this->addState(state), prob));
    }
}
