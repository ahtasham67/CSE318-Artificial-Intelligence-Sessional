#ifndef GRASP_HPP
#define GRASP_HPP
#include "graph.hpp"
#include "ConstractiveAlgo.hpp"
#include <iostream>
#include <vector>
using namespace std;

#define ll long long

vector<int> GRASP(graph g, int iter, double alpha, int localiter) {
    vector<int> bestAssignment;
    ll bestCutWeight = -1;

    for (int i = 0; i < iter; i++) {
        vector<int> assignment = semiGreedyHeuristic(g, alpha);

        assignment = LocalSearch(g, assignment, localiter);
        ll cutWeight = g.getMaxCut(assignment);
        
        if (cutWeight > bestCutWeight) {
            bestCutWeight = cutWeight;
            bestAssignment = assignment;
        }
    }

    return bestAssignment;
}

    
#endif