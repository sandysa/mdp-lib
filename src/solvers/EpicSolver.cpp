#include <ctime>
#include <list>
#include <utility>

#include "../../include/domains/WrapperProblem.h"

#include "../../include/solvers/EpicSolver.h"
#include "../../include/solvers/VISolver.h"

using namespace std;
using namespace mlcore;

namespace mlsolvers
{


double
EpicSolver::computeProbabilityGoalMonteCarlo(Problem* problem, State* start)
{
    State* currentState = nullptr;
    int numSims = 1000;
    int countSuccesses = 0;
    for (int i = 0; i < numSims; i++) {
        int steps = 0;
        bool goalSeen = false;
        currentState = start;
        while (steps++ < mdplib::dead_end_cost) {
            if (problem->goal(currentState)) {
                goalSeen = true;
                break;
            }
            if (currentState->bestAction() == nullptr) {
                break;
            }
            currentState = randomSuccessor(problem,
                                           currentState,
                                           currentState->bestAction());
        }

        if (goalSeen)
            countSuccesses++;
    }
    return double(countSuccesses) / numSims;
}


StateDoubleMap
EpicSolver::computeProbabilityTerminals(Problem* problem,
                                        State* start,
                                        StateSet& envelope)
{
    StateSet visited;
    StateDoubleMap sdMap1, sdMap2;
    StateDoubleMap* previousProbabilities = &sdMap1;
    StateDoubleMap* nextProbabilities = &sdMap2;
    (*previousProbabilities)[start] = 1.0;
    double maxDiff = 1.0;
    while (true) {
        maxDiff = 0.0;
        list<State*> queue;
        queue.push_front(start);
        while (!queue.empty()) {
            State* currentState = queue.back();
            queue.pop_back();
            if (!visited.insert(currentState).second)
                continue;
            if (envelope.count(currentState) == 0 ||
                    problem->goal(currentState) ||
                    currentState->deadEnd()) {
                (*nextProbabilities)[currentState] +=
                    (*previousProbabilities)[currentState];
                continue;
            }
            for (auto const & successor :
                    problem->transition(currentState,
                                        currentState->bestAction())) {
                (*nextProbabilities)[successor.su_state] +=
                    (*previousProbabilities)[currentState] * successor.su_prob;
                queue.push_front(successor.su_state);
            }
        }

        for (auto s : visited) {
            maxDiff = max(maxDiff,
                          fabs((*nextProbabilities)[s] -
                              (*previousProbabilities)[s]));

        }
        if (maxDiff < 0.01)
            break;
        swap(previousProbabilities, nextProbabilities);
        nextProbabilities->clear();
        visited.clear();
    }
    return *nextProbabilities;
}


void EpicSolver::trial(State* start)
{
    State* currentState = start;
    list<State*> visitedStack;
    while (true) {
        if (problem_->goal(currentState))
            break;
        visitedStack.push_front(currentState);
        bellmanUpdate(problem_, currentState);
        if (currentState->deadEnd())
            break;
        currentState =
            randomSuccessor(problem_, currentState, currentState->bestAction());
    }

    visited_.clear();
    while (!visitedStack.empty()) {
        currentState = visitedStack.front();
        visitedStack.pop_front();
        for (int i = 0; i < expansions_; i++)
            expandDepthLimited(currentState, horizon_);
    }

//    WrapperProblem* wrapper = new WrapperProblem(problem_);
//    StateSet reachableStates, goalTargets;
//    wrapper->overrideStates(&reachableStates);
//    wrapper->overrideGoals(&goalTargets);
//    while (!visitedStack.empty()) {
//        currentState = visitedStack.front();
//        visitedStack.pop_front();
//        solveDepthLimited(currentState, wrapper);
//    }

//                                                                                double probGoal = computeProbabilityGoalMonteCarlo(problem_, start);
//                                                                                dprint2("prob", probGoal);

//    wrapper->cleanup();
//    delete wrapper;
}


void EpicSolver::expandDepthLimited(State* state, int depth) {
    if (!visited_.insert(state).second ||
            state->deadEnd() ||
            problem_->goal(state))
        return;
    if (depth == 0) {
        bellmanUpdate(problem_, state);
        return;
    }
    Action* action = greedyAction(problem_, state);
    for (auto const & successors : problem_->transition(state, action)) {
        expandDepthLimited(successors.su_state, depth - 1);
    }
    bellmanUpdate(problem_, state);
}


// Below we use the term "solved" loosely. The values computed are only an
// admissible heuristic, not necessarily the optimal values.
void EpicSolver::solveDepthLimited(State* state, WrapperProblem* wrapper)
{
    VISolver viSolver(wrapper);
    StateSet tipStates;

    if (wrapper->overrideGoals()->count(state) > 0) {
        return; // This state has already been solved.
    }

    // We first find all states reachable up to a predetermined horizon and the
    // corresponding tip states (states that were solved before or that are at
    // the horizon). Note that states that have already been solved have also
    // been added to wrapper->overrideGoals(), so they will be considered
    // tip states by getReachableStates().
    wrapper->setNewInitialState(state);
    wrapper->overrideStates()->clear();
    assert(getReachableStates(wrapper,
                              *wrapper->overrideStates(),
                              tipStates,
                              horizon_));


    // We now find the best way to reach one of the previously solved
    // states. WrapperProblem is implemented so that states in
    // wrapper->overrideGoals_transition to an absorbing state
    // with a cost equal to their estimated cost so far.
    wrapper->overrideGoals()->insert(tipStates.begin(), tipStates.end());
//    viSolver.solve();

                                                                                bellmanUpdate(wrapper, state);

    // And finally we add to the set of goals all states that were
    // solved during this iteration.
    wrapper->overrideGoals()->insert(wrapper->overrideStates()->begin(),
                                     wrapper->overrideStates()->end());
}


Action* EpicSolver::solve(State* start)
{
    for (int i = 0; i < 100; i++) {
        trial(start);
    }

//    StateIntMap indices, low;
//    list<State*> stateStack;
//    strongConnect(start, indices, low, stateStack);
//    dprint2("size bpsg", low.size());
//    for (auto const & stateLowPair: low) {
//        if (stateLowPair.second != 0)
//            dprint2("can't reach goal", stateLowPair.first);
//    }

    return start->bestAction();
}


void EpicSolver::strongConnect(State* state,
                               StateIntMap& indices,
                               StateIntMap& low,
                               list<State*>& stateStack)
{
    static int index = 0;
    indices[state] = index++;
    stateStack.push_back(state);
    state->setBits(mdplib::CLOSED);

    if (problem_->goal(state)) {
        low[state] = 0;
        return;
    }

    mlcore::Action* action = greedyAction(problem_, state);
    for (auto const & successor : problem_->transition(state, action)) {
        State* next = successor.su_state;
        if (indices.count(next) == 0) {
            strongConnect(next, indices, low, stateStack);
            low[state] = min(low[state], low[next]);
        } else if (next->checkBits(mdplib::CLOSED)) {
            low[state] = min(low[state], indices[next]);
        }
    }
}

} //namespace mlsolvers
