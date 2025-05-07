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

pair< ll, ll >getSigmaXY(graph g, int i, vector<int> assignment){
    ll sigmaX=0, sigmaY=0;
    vector<pair<ll, pair<int, int> > > edges = g.getEdges();
    for (auto edge : edges){
        int other=-1;
        if(edge.second.first==i){
            other=edge.second.second;
        }
        else if(edge.second.second==i){
            other=edge.second.first;
        }
        if(other==-1) continue;
        if(assignment[other]==0) sigmaX+=edge.first;
        else if(assignment[other]==1) sigmaY+=edge.first;
    }
    return make_pair(sigmaX, sigmaY);
}

vector<int> greedyHeuristic(graph g){
    vector<int> assignment(g.getN(), -1);
    ll maxweight = -1;
    pair<ll, pair<int, int> > maxedge = g.getMaxEdge();
    int u = maxedge.second.first;
    int v = maxedge.second.second;
    assignment[u] = 0;
    assignment[v] = 1;  
    for (int i = 0; i < g.getN(); i++)
    {
        if (assignment[i] != -1) continue;
        ll sigmaX=0, sigmaY=0;
        pair<ll, ll> sigma = getSigmaXY(g, i, assignment);
        sigmaX = sigma.first;
        sigmaY = sigma.second;
        assignment[i] = (sigmaX>sigmaY)? 0 : 1;
        //cout<<i;
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

    multiset<ll> sigma_set;
    map<ll, vector<int> > sigma_map;
    for (int i = 0; i < n; i++) {
        if (assignment[i] == -1) {
            ll maxVal = max(sigma[i].first, sigma[i].second);
            sigma_set.insert(maxVal);
            sigma_map[maxVal].push_back(i);
        }
    }

    for (int step = 0; step < n - 2; step++) {
        ll wmin = *sigma_set.begin();
        ll wmax = *sigma_set.rbegin();
        ll cutoff = wmin + alpha * (wmax - wmin);

        auto it = sigma_set.lower_bound(cutoff);
        if (it == sigma_set.end()) it--;

        ll selected_score = *it;
        int selected_node = sigma_map[selected_score].back();

        if (sigma[selected_node].first > sigma[selected_node].second)
            assignment[selected_node] = 1;
        else
            assignment[selected_node] = 0;

        sigma_set.erase(sigma_set.find(selected_score));
        sigma_map[selected_score].pop_back();

        // Update neighbors
        for (auto& [neighbor, weight] : g.adj[selected_node]) {
            if (assignment[neighbor] == -1) {
                ll old_score = max(sigma[neighbor].first, sigma[neighbor].second);
                sigma_set.erase(sigma_set.find(old_score));

                auto& vec = sigma_map[old_score];
                vec.erase(find(vec.begin(), vec.end(), neighbor));

                if (assignment[selected_node] == 0)
                    sigma[neighbor].first += weight;
                else
                    sigma[neighbor].second += weight;

                ll new_score = max(sigma[neighbor].first, sigma[neighbor].second);
                sigma_set.insert(new_score);
                sigma_map[new_score].push_back(neighbor);
            }
        }
    }

    return assignment;
}

#endif