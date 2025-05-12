#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include "GRASP.hpp"
#include "ConstractiveAlgo.hpp"
#include "localSearch.hpp"

#define INF 100000000

void generate_statistics(graph &g, const string &output_file_name, int number_of_iterations, ofstream &output_file, ll knownbest) {
    long long random_avg = RandomizedHeuristic(g, 5);
    
    long long greedy_avg1 = g.getMaxCut(greedyHeuristic(g));

    long long semi_greedy_avg1 = 0;
    vector<int> assignment = semiGreedyHeuristic(g, 0.9);
        semi_greedy_avg1 = g.getMaxCut(assignment);
    

    int totaliter=0;
    long long random_local=0;
    for(int i=0; i<number_of_iterations; i++){
        vector<int> assignment = semiGreedyHeuristic(g, 0.9);
        auto assignmentIter = LocalSearch(g, assignment);
        random_local += g.getMaxCut(assignmentIter.first);
        totaliter += assignmentIter.second;
    }
    ll radomavg = (random_local/number_of_iterations);
    int iter = 10;
    //int localiter = 10;
    long long grasp_random = -INF;
    //for (int i = 0; i < number_of_iterations; ++i) {
        assignment = GRASP(g, number_of_iterations, 0.9);
        grasp_random = max(grasp_random, g.getMaxCut(assignment));
    //}

    output_file << g.name << "," << g.getN() << "," << g.m << ","
                << random_avg << "," << greedy_avg1 << "," << semi_greedy_avg1 << ","
                << totaliter << "," << radomavg << ","
                << number_of_iterations << "," << grasp_random;

    if (knownbest != -1) {
        output_file << "," << knownbest;
    }
    else output_file << "," ;
    output_file << endl;
}

void generate_statistics_from_file(const string &_input_file_name, const string &_output_file_name, int number_of_iterations, ofstream &output_file, map<string, ll> known) {
    ifstream in(_input_file_name);
    string name = _input_file_name.substr(_input_file_name.find_last_of("/\\") + 1);
    name = name.substr(0, name.find_last_of("."));

    int n, m;
    in >> n >> m;
    vector<pair<long long, pair<int, int> > > edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        long long w;
        in >> u >> v >> w;
        edges.emplace_back(w, make_pair(u - 1, v - 1));
    }

    graph g(n, m, name, edges);
     ll known_best = -1;
    if (known.count(name)) {
        known_best = known.at(name);
    }
    generate_statistics(g, _output_file_name, number_of_iterations, output_file,known_best);
}
