#ifndef CONSTRUCTIVE_ALGORITHM
#define CONSTRUCTIVE_ALGORITHM

#include "graph.hpp"
#include "localSearch.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <set>

using namespace std;
#define ll long long

ll RandomizedHeuristic(graph g, int trial)
{
    srand(time(0));
    ll totalcutweight = 0;
    for (int i = 0; i < trial; i++)
    {
        vector<int> NodeAssignment(g.getN());
        for (int j = 0; j < g.getN(); j++)
        {
            NodeAssignment[j] = rand() % 2;
        }
        ll cutweight = g.getMaxCut(NodeAssignment);
        totalcutweight += cutweight;
    }
    return totalcutweight / trial;
}

vector<int> greedyHeuristic(graph& g) {
    int n = g.getN();
    vector<int> assignment(n, -1);

    vector<vector<pair<int, ll> > > adj = g.getAdjList();
    // for (auto& edge : g.getEdges()) {
    //     int u = edge.second.first;
    //     int v = edge.second.second;
    //     ll w = edge.first;
    //     adj[u].emplace_back(v, w);
    //     adj[v].emplace_back(u, w);
    // }
    auto maxEdge = g.getMaxEdge();
    int u = maxEdge.second.first;
    int v = maxEdge.second.second;
    assignment[u] = 0;
    assignment[v] = 1;

    for (int i = 0; i < n; ++i) {
        if (assignment[i] != -1) continue;

        ll sigmaX = 0, sigmaY = 0;
        for (auto& [neighbor, weight] : adj[i]) {
            if (assignment[neighbor] == 0) sigmaX += weight;
            else if (assignment[neighbor] == 1) sigmaY += weight;
        }

        assignment[i] = (sigmaX > sigmaY) ? 1 : 0;
    }

    return assignment;
}

vector<int> semiGreedyHeuristic(graph& g, double alpha) {
    int n = g.getN();
    vector<int> assignment(n, -1);
    vector<pair<ll, ll> > sigma(n, make_pair(0, 0));

    // Step 1: Get max edge and initialize
    pair<ll, pair<int, int> > maxEdge = g.getMaxEdge();
    int u = maxEdge.second.first;
    int v = maxEdge.second.second;
    assignment[u] = 0;
    assignment[v] = 1;

    // Step 2: Initialize sigma values
    for (int i = 0; i < n; i++) {
        if (assignment[i] == -1) {
            if (g.hasEdge(i, u)) sigma[i].first += g.getWeight(i, u);
            if (g.hasEdge(i, v)) sigma[i].second += g.getWeight(i, v);
        }
    }

    for (int step = 0; step < n - 2; step++) {
        vector<int> candidates;
        vector<ll> scores;

        for (int i = 0; i < n; i++) {
            if (assignment[i] == -1) {
                ll valX = sigma[i].first;
                ll valY = sigma[i].second;
                ll val = max(valX, valY);
                candidates.push_back(i);
                scores.push_back(val);
            }
        }

        if (candidates.empty()) break;

        // Compute wmin and wmax from σX and σY values over candidates
        ll wmin = LLONG_MAX, wmax = LLONG_MIN;
        for (int i : candidates) {
            wmin = min(wmin, min(sigma[i].first, sigma[i].second));
            wmax = max(wmax, max(sigma[i].first, sigma[i].second));
        }

        ll cutoff = wmin + alpha * (wmax - wmin);

        // Construct RCL
        vector<int> RCL;
        for (int i : candidates) {
            ll score = max(sigma[i].first, sigma[i].second);
            if (score >= cutoff) {
                RCL.push_back(i);
            }
        }

        if (RCL.empty()) continue; // skip step or break if necessary

        // Randomly select from RCL
        int selected_node = RCL[rand() % RCL.size()];

        // Assign to partition
        if (sigma[selected_node].first > sigma[selected_node].second)
            assignment[selected_node] = 1;
        else
            assignment[selected_node] = 0;

        // Update neighbors
        for (auto& [neighbor, weight] : g.adj[selected_node]) {
            if (assignment[neighbor] == -1) {
                if (assignment[selected_node] == 0)
                    sigma[neighbor].first += weight;
                else
                    sigma[neighbor].second += weight;
            }
        }
    }

    return assignment;
}
#endif