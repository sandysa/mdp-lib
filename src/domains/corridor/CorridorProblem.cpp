#include <vector>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "../../../include/domains/corridor/CorridorProblem.h"
#include "../../../include/domains/corridor/CorridorState.h"
#include "../../../include/domains/corridor/CorridorAction.h"


void CorridorProblem::addAllActions()
{
    mlcore::Action* move_slow = new CorridorAction(grid_corridor::SLOW);
    mlcore::Action* move_fast = new CorridorAction(grid_corridor::FAST);

    actions_.push_front(move_slow);
    actions_.push_front(move_fast);
}
CorridorProblem::CorridorProblem(const char* filename)
{
    std::ifstream myfile (filename);

    goals_ = new PairDoubleMap();

    // Once the file is finished parsing, these will have correct values
    width_ = 0, height_ = 0;
    if (myfile.is_open()) {
        std::string line;
        while ( std::getline (myfile, line) ) {
            for (width_ = 0; width_ < line.size(); width_++) {
                if (line.at(width_) == 'x') {
                    walls.insert(std::pair<int, int>(width_, height_));
                } else if (line.at(width_) == 'N') {
                    narrow.insert(std::pair<int, int>(width_, height_));
                } else if (line.at(width_) == 'G') {
                    goals_->insert(
                        std::make_pair(
                            std::pair<int,int> (width_, height_), 0.0));
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

    s0 = new CorridorState(x0_, y0_, true,this);
    absorbing = new CorridorState(-1, -1, true,this);
    this->addState(s0);
    this->addState(absorbing);
    addAllActions();
}


bool CorridorProblem::corridorGoal(CorridorState* gws) const
{
    std::pair<int,int> pos(gws->x(),gws->y());
    return goals_->find(pos) != goals_->end();
}


bool CorridorProblem::goal(mlcore::State* s) const
{
    return s == absorbing;
}

double CorridorProblem::cost(mlcore::State* s, mlcore::Action* a) const
{
    if (s == absorbing)
        return 0.0;
    CorridorState* gws = static_cast<CorridorState *> (s);
    if (corridorGoal(gws)) {
        std::pair<int,int> pos(gws->x(),gws->y());
        return (*goals_)[pos];
    }
//    if (narrow.count(std::pair<int, int> (gws->x(), gws->y())) != 0)
//        return holeCost_ * actionCost_;
    CorridorAction* cta = static_cast<CorridorAction*>(a);
    if(!gws->alive())
        return 100.0;


    if(cta->speed() == grid_corridor::SLOW)
        return 2;

    return 1.0;
}


bool CorridorProblem::applicable(mlcore::State* s, mlcore::Action* a) const
{
    return true;
}

std::list<mlcore::Successor>
CorridorProblem::transition(mlcore::State *s, mlcore::Action *a)
{
    CorridorState* state = static_cast<CorridorState *> (s);
    CorridorAction* action = static_cast<CorridorAction *> (a);

    std::list<mlcore::Successor> successors;

    if (s == absorbing || corridorGoal(state)) {
        successors.push_front(mlcore::Successor(absorbing, 1.0));
        return successors;
    }
    /** Modified to pass as height since 1D. Otherwise, it should be height_-1. **/

    if (narrow.count(std::pair<int, int> (state->x(), state->y())) != 0)
    {
        if(action->speed() == grid_corridor::FAST){
        addSuccessor(state, successors, height_ , state->y(),
                     state->x()+1, state->y(), true, 0.85);
        addSuccessor(state, successors, height_, state->y(),
                     state->x()+1, state->y(), false, 0.15);

        }
        else{
            addSuccessor(state, successors, height_ , state->y(),
                     state->x()+1, state->y(), true, 0.99);
            addSuccessor(state, successors, height_ , state->y(),
                     state->x()+1, state->y(), false, 0.01);

        }
    }
    else{
        addSuccessor(state, successors, height_ , state->y(),
                     state->x()+1, state->y(), true, 1.0);
    }
    return successors;
}

void CorridorProblem::addSuccessor(
    CorridorState* state, std::list<mlcore::Successor>& successors,
    int val, int limit, int newx, int newy, bool alive, double prob)
{
    bool isWall = (walls.count(std::pair<int, int> (newx, newy)) != 0);
    if (val > limit && !isWall) {
        CorridorState *next = new CorridorState(newx, newy, alive, this);
        successors.push_front(mlcore::Successor(this->addState(next), prob));
    } else {
        successors.push_front(mlcore::Successor(state, prob));
    }
}
