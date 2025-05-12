#include "graph.hpp"
#include "localSearch.hpp"
#include "ConstractiveAlgo.hpp"
#include "GRASP.hpp"
#include "statistics.hpp"
#include <iostream>
#include <vector>
#include<fstream>
#include <filesystem>
using namespace std;

#define ll long long
int main(int argc, char *argv[]) {
    string file_name = argv[1];
    ifstream in(file_name);
    string name = file_name.substr(file_name.find_last_of("/\\") + 1);
    name = name.substr(0, name.find_last_of("."));
    int n, m;
    in >> n >> m;
    vector<pair<long long, pair<int, int> > > edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        long long w;
        //cout<< u <<v <<w<<endl;
        in >> u >> v >> w;
        edges.emplace_back(w, make_pair(u - 1, v - 1));
    }
    graph g(n, m, name, edges);


        vector<int>greedyassignment;
        vector<int>semigreedyassignment;
        string option = argv[2];
        if(option=="1"){
            ll maxcutValue = RandomizedHeuristic(g,10);
            cout << "maxcut value for random: "<< maxcutValue <<endl;
        }
        else if(option=="2"){
            greedyassignment = greedyHeuristic(g);
            ll maxcutValue = g.getMaxCut(greedyassignment);
            cout << "maxcut value for greedy: "<< maxcutValue <<endl;
        }
        else if(option == "3"){
            double alpha = 0.6;
            semigreedyassignment = semiGreedyHeuristic(g, alpha);
            ll maxcutValue = g.getMaxCut(semigreedyassignment);
            cout << "maxcut value for semigreedy for alpha("<<alpha<<"): "<< maxcutValue <<endl;
        }
        else if(option == "4"){
            double alpha = 0.6;
            semigreedyassignment = semiGreedyHeuristic(g, alpha);
            auto local = LocalSearch(g,semigreedyassignment);
            cout << "maxcut value for local search: "<< g.getMaxCut(local.first)<<" with iter no. "<<local.second<<endl;
        }
        else if(option == "5"){

            int iter;
            cout<<"enter iteration number: ";
            cin>>iter;
            double alpha = 0.6;
            ll maxcut = g.getMaxCut(GRASP(g,iter,alpha));
            cout << "maxcut value for GRASP: "<< maxcut <<" with iter no. "<<iter<<endl;
        }

    return 0;
}