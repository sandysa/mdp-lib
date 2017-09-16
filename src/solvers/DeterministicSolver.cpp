#include <queue>

#include "../../include/solvers/DeterministicSolver.h"
#include "../../test/reduced/CostEstimate.cpp"

namespace mlsolvers
{

mlcore::Action* DeterministicSolver::solve(mlcore::State* s0)
{
    NodeComparer comp();
    std::priority_queue<Node*, std::vector<Node*>, NodeComparer> frontier(comp);
    Node* init = new Node(nullptr, s0, nullptr, 0.0, heuristic_);
    frontier.push(init);
    std::list<Node*> allNodes;  // for memory clean-up later
    allNodes.push_back(init);
    Node* final;
    while (!frontier.empty()) {
        Node* node = frontier.top();
        frontier.pop();

        if (node->state()->checkBits(mdplib::VISITED_ASTAR))
            continue;   // valid because this is using path-max
        node->state()->setBits(mdplib::VISITED_ASTAR);

        if (problem_->goal(node->state())) {
            final = node;
            break;
        }

        for (mlcore::Action* a : problem_->actions()) {
            if (!problem_->applicable(node->state(), a))
                continue;

            mlcore::State* nextState;
            if (choice_ == det_most_likely)
                nextState = mostLikelyOutcome(problem_, node->state(), a);

           //double cost = problem_->cost(node->state(), a);
            
             double cost = acad::new_cost_blocksworld(node->state(),a,problem_);
          //   cout<<"cost = "<<cost<<endl;
         
            Node* next = new Node(node, nextState, a, cost, heuristic_, true);
            frontier.push(next);
            allNodes.push_back(next);
        }
    }
 double final_f =0.0;
 int index = 1;
    mlcore::Action* optimal;
    while (final->parent() != nullptr) {
        optimal = final->action();
        final = final->parent();
           final_f = final->f();
           if(index==1){
               cout<<"final f value = ";
           cout<<final_f<<endl;
           }
           
           index++;
    }

    for (Node* node : allNodes) {
        node->state()->clearBits(mdplib::VISITED_ASTAR);
        delete node;
    }

     
    return optimal;
}

}
