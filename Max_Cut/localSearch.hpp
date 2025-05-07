#ifndef LOCALSEARCH_HPP
#define LOCALSEARCH_HPP
#include "graph.hpp"
#include <iostream>
#include <vector>
using namespace std;

#define ll long long


    vector<int> LocalSearch(graph g,vector<int>assignment, int maxIter) {
        //vector<int> assignment(g.getN(), -1);
        ll maxcut = g.getMaxCut(assignment);
        vector<int> bestAssignment = assignment;
    
        for (int i = 0; i < maxIter; i++) {
            bool improved = false;
    
            for (int j = 0; j < g.getN(); j++) {
                ll delta = g.getDeltaMaxCut(j, assignment);
                if (maxcut + delta > maxcut) {
                    assignment[j] = 1 - assignment[j];
                    maxcut += delta;
                    improved = true;
                }
            }
    
            if (!improved) break;
            bestAssignment = assignment;
        }
    
        return bestAssignment;
    }

#endif