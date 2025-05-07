#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

#define ll long long

class graph
{
public:
    int n, m;
    string name;
    vector<pair<ll, pair<int, int> > > edges;
    vector<map<int, ll> > adj;              

    graph() {}

    graph(int n, int m, string name, const vector<pair<ll, pair<int, int> > > &edges)
    {
        this->n = n;
        this->m = m;
        this->name = name;
        this->edges = edges;
        adj.resize(n);

        for (int i = 0; i < m; i++)
        {
            int u = edges[i].second.first;
            int v = edges[i].second.second;
            ll w = edges[i].first;
            adj[u][v] = w;
            adj[v][u] = w;
        }
    }

    ll getMaxCut(const vector<int> &NodeAssignment)
    {
        ll maxcut = 0;
        for (int i = 0; i < m; i++)
        {
            int u = edges[i].second.first;
            int v = edges[i].second.second;
            if (NodeAssignment[u] != NodeAssignment[v])
            {
                maxcut += edges[i].first;
            }
        }
        return maxcut;
    }

    ll getDeltaMaxCut(int NodeToSwap, const vector<int> &prevAssignment)
    {
        ll delta = 0;
        for (const auto &edge : edges)
        {
            int u = edge.second.first;
            int v = edge.second.second;
            int other = -1;

            if (NodeToSwap == u)
                other = v;
            else if (NodeToSwap == v)
                other = u;
            else
                continue;

            if (prevAssignment[u] != prevAssignment[v])
                delta -= edge.first;
            else
                delta += edge.first;
        }
        return delta;
    }

    pair<ll, pair<int, int> > getMaxEdge()
    {
        pair<ll, pair<int, int> > maxedge = edges[0];
        for (int i = 1; i < m; i++)
        {
            if (edges[i].first > maxedge.first)
                maxedge = edges[i];
        }
        return maxedge;
    }
    void printGraph()
    {
        cout << "Graph: " << name << endl;
        cout << "Number of nodes: " << n << endl;
        cout << "Number of edges: " << m << endl;
        cout << "Edges:" << endl;
        for (const auto &edge : edges)
        {
            cout << edge.first << " " << edge.second.first << " " << edge.second.second << endl;
        }
    }

    int getN() const
    {
        return n;
    }

    vector<pair<ll, pair<int, int> > > getEdges() const
    {
        return edges;
    }
    bool hasEdge(int u, int v) const
    {
        auto it = adj[u].find(v);
        return it != adj[u].end();
    }

    ll getWeight(int u, int v) const
    {
        auto it = adj[u].find(v);
        return it != adj[u].end() ? it->second : 0;
    }
};

#endif // GRAPH_HPP
