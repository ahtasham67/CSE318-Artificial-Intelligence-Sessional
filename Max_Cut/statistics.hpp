#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include "GRASP.hpp"
#include "ConstractiveAlgo.hpp"
#include "localSearch.hpp"

#define INF 100000000

void generate_statistics(graph &g, const string &output_file_name, int number_of_iterations = 5) {
    ofstream output_file(output_file_name, ios::app);
    
    // output_file << "Problem,,,Constructive algorithm,,,Local search,,GRASP,,Known best solution or upper bound" << endl;
    // output_file << "Name,|V| or n,|E| or m,Simple Randomized or Ranomized-1,Simple Greedy or Greedy-1,Semi-greedy-1,Simple local or local-1,,,GRASP-1," << endl;
    // output_file << ",,,,,,No. of iterations,Average value,No. of iterations,Best value," << endl;

    long long random_avg = 0;
    for (int i = 0; i < number_of_iterations; ++i)
        random_avg += RandomizedHeuristic(g, 10);
    random_avg /= number_of_iterations;

    long long greedy_avg1 = 0;
    for (int i = 0; i < number_of_iterations; ++i)
        greedy_avg1 += g.getMaxCut(greedyHeuristic(g));
    greedy_avg1 /= number_of_iterations;

    long long semi_greedy_avg1 = 0;
    vector<int> assignment(g.getN(), -1);
    for (int i = 0; i < number_of_iterations; ++i){
        assignment = semiGreedyHeuristic(g, 0.5);
        semi_greedy_avg1 += g.getMaxCut(assignment);
    }
    semi_greedy_avg1 /= number_of_iterations;

    int iter1 = 10;
    assignment = LocalSearch(g, assignment, iter1);
    long long random_local = g.getMaxCut(assignment);

    int iter = 10;
    int localiter = 10;
    long long grasp_random = -INF;
    for (int i = 0; i < number_of_iterations; ++i) {
        assignment = GRASP(g, iter, 0.5, localiter);
        grasp_random = max(grasp_random, g.getMaxCut(assignment));
    }

    output_file << g.name << "," << g.getN() << "," << g.m << ","
                << random_avg << "," << greedy_avg1 << "," << semi_greedy_avg1 << ","
                << iter1 << "," << random_local << ","
                << number_of_iterations << "," << grasp_random << ","
                << endl;
}

void generate_statistics_from_file(const string &_input_file_name, const string &_output_file_name, int number_of_iterations = 10) {
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
    graph graph(n, m, name, edges);

    generate_statistics(graph, _output_file_name, number_of_iterations);
}
