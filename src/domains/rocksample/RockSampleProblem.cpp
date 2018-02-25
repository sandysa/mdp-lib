#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "../../../include/Problem.h"
#include "../../../include/domains/rocksample/RockSampleProblem.h"
#include "../../../include/domains/rocksample/RockSampleState.h"
#include "../../../include/domains/rocksample/RockSampleAction.h"

using namespace std;

void RockSampleProblem::addAllActions()
{
    mlcore::Action* up = new RockSampleAction(rocksample::UP);
    mlcore::Action* down = new RockSampleAction(rocksample::DOWN);
    mlcore::Action* left = new RockSampleAction(rocksample::LEFT);
    mlcore::Action* right = new RockSampleAction(rocksample::RIGHT);
    mlcore::Action* sample = new RockSampleAction(rocksample::SAMPLE);
    actions_.push_front(up);
    actions_.push_front(down);
    actions_.push_front(left);
    actions_.push_front(right);
    actions_.push_front(sample);
}


RockSampleProblem::RockSampleProblem() :
                    width_(0), height_(0), x0_(0), y0_(0), sampledRocks0_(0), status0_(rocksample::UNKNOWN),
                    actionCost_(0.03)
{
    absorbing = new RockSampleState(this, -1, -1, -1, 1);
    addAllActions();
}

RockSampleProblem::RockSampleProblem(const char* filename,
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
    s0 = new RockSampleState(this, x0_, y0_, 0, rocksample::BAD); //no rocks at start state
    absorbing = new RockSampleState(this, -1, -1, -1, rocksample::GOOD);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
    setGoal(potential_goals, uniform_dist);
}


RockSampleProblem::RockSampleProblem(
    int width, int height, int x0, int y0, int sampledRocks0, int status0,
    double actionCost) :
        width_(width), height_(height), x0_(x0), y0_(y0), sampledRocks0_(sampledRocks0), status0_(status0),
        actionCost_(actionCost)
{
    s0 = new RockSampleState(this, x0_, y0_, sampledRocks0_, status0_);
    absorbing = new RockSampleState(this, -1, -1, -1, rocksample::GOOD);
    this->addState(s0);
    addAllActions();
}


RockSampleProblem::RockSampleProblem(int width, int height, int x0, int y0, int sampledRocks0, int status0,
                     std::map<std::pair<int,int>, double> potential_goals_prob0, double actionCost):
                     width_(width), height_(height), x0_(x0), y0_(y0), sampledRocks0_(sampledRocks0), status0_(status0),
                     actionCost_(actionCost)

{
   potential_goals_prob.clear();
   potential_goals_prob.insert(potential_goals_prob0.begin(), potential_goals_prob0.end());
   s0 = new RockSampleState(this, x0_, y0_, sampledRocks0_, status0_);
   absorbing = new RockSampleState(this, -1, -1, -1, rocksample::GOOD);
    this->addState(s0);
    addAllActions();
}

RockSampleProblem::RockSampleProblem(int width, int height,
                                   int x0, int y0, int victims0, int status0,
                                   mlcore::Heuristic* h)
                                   : width_(width), height_(height),
                                     x0_(x0), y0_(y0), sampledRocks0_(victims0), status0_(status0)
{
    s0 = new RockSampleState(this, x0_, y0_, sampledRocks0_, status0_);
    absorbing = new RockSampleState(this, -1, -1, -1, rocksample::GOOD);
    this->addState(s0);
    heuristic_ = h;
    gamma_ = 1.0;
    addAllActions();
    clearedStates.clear();
}
/** Generates a random number between 0 and 5 for the number of victims in each
** potential goal location.
**/
void RockSampleProblem::setGoal(std::vector<std::pair<int,int>> potential_goals, bool uniform_dist){
   goals_ = new PairDoubleMap();
    maxRocks_ = 1;
    /** If there is only one potential goal, then set that to be true goal **/
    if (potential_goals.size() == 1){
        goals_->insert(std::make_pair(potential_goals.at(0), 0.0));
        return;
     }
      /** If uniform_dist is true, then all potential goals are equally probable.
        ** Otherwise, the random number is generated as probablity.
        **/
    for(int i = 0; i < potential_goals.size(); i++){
        std::pair<int,int> pos = potential_goals.at(i);
        if(uniform_dist)
            potential_goals_prob.insert({pos, 0.5});
        else{
             double prob = ((double) rand() / (RAND_MAX));
             potential_goals_prob.insert({pos, prob});
        }
    }
    cout << "potential goal size = " << potential_goals.size() << endl;
     for (auto const& element : potential_goals_prob){
         double prob = ((double) rand() / (RAND_MAX));
         if (prob >= element.second){
            goals_->insert(std::make_pair(element.first, 0.0));
            std::pair<int, int> pos (element.first);
            std::cout << "rock present in = " << pos.first << "," << pos.second << endl;
            }
     }
     // if no goal set, then set first to be a goal.
     if(goals_->size() == 0)
         goals_->insert(std::make_pair(potential_goals.at(0), 0.0));
}

bool RockSampleProblem::RSGoal(RockSampleState* rss) const
{
   if(rss->sampledRocks() == maxRocks_ && rss->status() == rocksample::GOOD)
        return true;

     return false;
}

bool RockSampleProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}

int RockSampleProblem::getobservation(RockSampleState* rss){
    std::pair<int,int> pos(rss->x(),rss->y());
     if(goals_->find(pos) != goals_->end())
        return 1;
   return -1;
}

bool RockSampleProblem::isPotentialGoal(RockSampleState* rss){
     std::pair<int,int> pos(rss->x(),rss->y());
     if (std::find(potential_goals.begin(), potential_goals.end(), pos) != potential_goals.end())
        return true;

    return false;
}

void printCollectedRocks(std::vector<std::pair<int,int>> v){
for (const auto i: v)
  std::cout << i.first << "," << i.second << endl;
}

std::list<mlcore::Successor>
RockSampleProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    RockSampleState* state = static_cast<RockSampleState *> (s);
    RockSampleAction* action = static_cast<RockSampleAction *> (a);

    std::list<mlcore::Successor> successors;

    if (s == absorbing || RSGoal(state)) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }
    if (dead_ends.count(std::pair<int, int>(state->x(), state->y()))) {
        s->markDeadEnd();
        successors.push_front(mlcore::Successor(s, 1.0));
        return successors;
    }

    int rocks =  getobservation(state);
    int newstatus = rocksample::UNKNOWN; //by default

    if (state->status() == rocksample::UNKNOWN && isPotentialGoal(state)) // figuring if its a goal
    {
        if (rocks == 1)
            newstatus = rocksample::GOOD;
        if (rocks == -1)
            newstatus = rocksample::BAD;
    }
     std::pair<int,int> pos(state->x(),state->y());
    /** deterministic action. Update number of sampled rocks.
    ** Also prevents sampling the same rocks again!
    **/
   if (action->dir() == rocksample::SAMPLE && state->status() == rocksample::GOOD) {
        int newsamples =  (rocks > 0)? state->sampledRocks() + rocks : state->sampledRocks() ;
        addSuccessor(state, successors, width_ + 1, 0,
                     state->x(), state->y(), newsamples,
                     rocksample::GOOD, 1.0);

        // all rocks sampled
        clearedStates.push_back(pos);
        return successors;
    }

     if(state->status() == rocksample::UNKNOWN && action->dir() != rocksample::SAMPLE && newstatus != rocksample::UNKNOWN){
         addSuccessor(state, successors, width_ + 1, state->y(),
                     state->x(), state->y(), state->sampledRocks(), newstatus,
                     1.0);
        return successors;
    }

    double probForward = 0.8;
    int numSuccessors = allDirections_ ? 3 : 2;
    double probSides = 0.2 / numSuccessors;
    if (action->dir() == rocksample::UP) {
        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     newstatus, probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->y(), 0,
                         state->x(), state->y() - 1, state->sampledRocks(),
                         newstatus, probSides);
        }
    }
        else if (action->dir() == rocksample::DOWN) {
        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     newstatus, probSides);

        addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, height_ - 1, state->y(),
                         state->x(), state->y() + 1, state->sampledRocks(),
                         newstatus, probSides);
        }
    } else if (action->dir() == rocksample::LEFT) {
        addSuccessor(state, successors, state->x(), 0,
                     state->x() - 1, state->y(), state->sampledRocks(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     newstatus, probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, width_ - 1, state->x(),
                         state->x() + 1, state->y(), state->sampledRocks(),
                         newstatus, probSides);
        }
    } else if (action->dir() == rocksample::RIGHT) {
         addSuccessor(state, successors, width_ - 1, state->x(),
                     state->x() + 1, state->y(), state->sampledRocks(),
                     newstatus, probForward);

        addSuccessor(state, successors, state->y(), 0,
                     state->x(), state->y() - 1, state->sampledRocks(),
                     newstatus, probSides);

        addSuccessor(state, successors, height_ - 1, state->y(),
                     state->x(), state->y() + 1, state->sampledRocks(),
                     newstatus, probSides);

        if (allDirections_) {
            addSuccessor(state, successors, state->x(), 0,
                         state->x() - 1, state->y(), state->sampledRocks(),
                         newstatus, probSides);
        }
    }

    return successors;
}

double RockSampleProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    RockSampleAction* rsa = static_cast<RockSampleAction *> (a);
    RockSampleState* rss = static_cast<RockSampleState *> (s);
    RockSampleProblem* rsp = const_cast<RockSampleProblem*> (this);

    if (RSGoal(rss))
        return 0;

    if (rsa->dir() == rocksample::SAMPLE)
    {
        if (rsp->getobservation(rss) > 0)
            return 2.0 * actionCost_;
        else
            return 10.0 * actionCost_;
    }

    if (holes.count(std::pair<int, int> (rss->x(), rss->y())) != 0)
        return holeCost_ * actionCost_;

   if(rsp->isPotentialGoal(rss) && rss->status() == rocksample::UNKNOWN)
        return 0.0;

    return actionCost_;
}


bool RockSampleProblem::applicable(mlcore::State* s, mlcore::Action* a) const
{
    RockSampleState* rss = static_cast<RockSampleState *> (s);
    RockSampleProblem* srp = const_cast<RockSampleProblem*> (this);
    RockSampleAction* sra = static_cast<RockSampleAction *> (a);
    std::pair<int,int> pos (rss->x(),rss->y());
    if (sra->dir() == rocksample::SAMPLE) // applicable in potential goals
    {
       if(srp->isPotentialGoal(rss)
       && rss->status() != rocksample::BAD && srp->getobservation(rss) > 0
       )
            return true;
        else
            return false;
    }
    // move action is always applicable
        return true;
}

void RockSampleProblem::addSuccessor(
    RockSampleState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, int newsamples, int newstatus, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
            RockSampleState *next = new RockSampleState(this, newx, newy, newsamples, newstatus);
            successors.push_front(mlcore::Successor(this->addState(next), prob));
     } else {
          successors.push_front(mlcore::Successor(state, prob));
    }
}
