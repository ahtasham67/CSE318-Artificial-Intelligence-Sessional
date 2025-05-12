#ifndef LOCALSEARCH_HPP
#define LOCALSEARCH_HPP
#include "graph.hpp"
#include <iostream>
#include <vector>
using namespace std;

#define ll long long


pair<vector<int>, int> LocalSearch(graph &g, vector<int> assignment) {
    vector<vector<pair<int, ll> > > adjList = g.getAdjList();
    ll maxcut = g.getMaxCut(assignment);
    vector<int> bestAssignment = assignment;

    int numOfIter = 0;
    bool improved = true;

    while (improved) {
        improved = false;
        numOfIter++;

        for (int u = 0; u < g.n; ++u) {
            ll delta = 0;
            for (const auto &neighbor : adjList[u]) {
                int v = neighbor.first;
                ll w = neighbor.second;
                if (assignment[u] == assignment[v]) delta += w;
                else delta -= w;
            }

            if (delta > 0) {
                assignment[u] = 1 - assignment[u];
                maxcut += delta;
                improved = true;
            }
        }

        if (improved) {
            bestAssignment = assignment;
        }
    }

    return make_pair(bestAssignment, numOfIter);
}

#endif