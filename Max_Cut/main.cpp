#include "graph.hpp"
#include "localSearch.hpp"
#include "ConstractiveAlgo.hpp"
#include "GRASP.hpp"
#include "statistics.hpp"
#include <iostream>
#include <vector>
#include<fstream>
using namespace std;

#define ll long long
int main() {
    // int n = 4;
    // int m = 4;
     //string name = "example_graph";
    // vector<pair<ll, pair<int, int> > > edges;
    // edges.push_back(make_pair(1, make_pair(0, 1)));
    // edges.push_back(make_pair(3, make_pair(0, 2)));
    // edges.push_back(make_pair(2, make_pair(1, 2)));
    // //edges.push_back(make_pair(4, make_pair(1, 3)));
    // edges.push_back(make_pair(4, make_pair(2, 3)));
    // //edges.push_back(make_pair(6, make_pair(3, 4)));
    // // Create a graph object    
    // graph g(n, m, name, edges);
    // graph g2(n,m, name, edges);
    // graph g3(n,m , name, edges);
    // //g.printGraph();
    // // Example NodeAssignment
    // vector<int> NodeAssignment(5);
    // NodeAssignment[0] = 0;
    // NodeAssignment[1] = 0;
    // NodeAssignment[2] = 1;
    // NodeAssignment[3] = 1;
    // //NodeAssignment[4] = 1;
    // // Calculate max cut
    // ll maxcut = g.getMaxCut(NodeAssignment);
    // cout << "Max Cut: " << maxcut << endl;
    // // Perform local search
    // localSearch ls(g);
    // vector<int> bestAssignment = ls.LocalSearch(NodeAssignment, 10);
    // cout << "Best Assignment: ";
    // for (int i = 0; i < bestAssignment.size(); i++) {
    //     cout << bestAssignment[i] << " " << NodeAssignment[i] << endl;
    // }
    // //test for local randomized heuristic
    // ll random1 = RandomizedHeuristic(g, 4);
    // cout << "Average value of randomized heuristic: " << random1 << endl;
    // ll greedy1 = greedyHeuristic(g);
    // cout << "Average value of greedy heuristic: " << greedy1 << endl;
    // ll semigreedy = semiGreedyHeuristic(g, 0.5);
    // cout << "Average value of semi greedy heuristic: " << semigreedy << endl;
    string input_file = "set1/g54.rud";
    string output_file = "statistics.csv";
    generate_statistics_from_file(input_file, output_file);
    return 0;
}