#ifndef GRASP_HPP
#define GRASP_HPP
#include "graph.hpp"
#include "ConstractiveAlgo.hpp"
#include <iostream>
#include <vector>
using namespace std;

#define ll long long

vector<int> GRASP(graph g, int iter, double alpha) {
    vector<int> bestAssignment;
    ll bestCutWeight = -1;

    for (int i = 0; i < iter; i++) {
        vector<int> assignment = semiGreedyHeuristic(g, alpha);
        //cout<<g.getMaxCut(assignment)<<endl;
        auto assignmentIter = LocalSearch(g, assignment);
        ll cutWeight = g.getMaxCut(assignmentIter.first);

        if (cutWeight > bestCutWeight) {
            bestCutWeight = cutWeight;
            bestAssignment = assignmentIter.first;
        }
    }

    return bestAssignment;
}

    
#endif