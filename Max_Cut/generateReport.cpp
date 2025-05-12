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
namespace fs = filesystem;


#define ll long long
int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_directory> <output_file>" << endl;
        return 1;
    }

    string input_directory = argv[1];
    string output_file_name = argv[2];

    ofstream output_file(output_file_name, ios::app);
    if (!output_file.is_open()) {
        cerr << "Error opening output file: " << output_file_name << endl;
        return 1;
    }
    map<string, ll> known;
    ifstream fin("knownBest.txt");
    string name;
    ll val;
    while (fin >> name) {
        if (fin >> val) {
            known[name] = val;
        }
    }

    output_file << "Problem,,,Constructive algorithm,,,Local search,,GRASP,,Known best solution or upper bound" << endl;
    output_file << "Name,|V| or n,|E| or m,Simple Randomized,Simple Greedy,Semi-greedy-1,Simple local,,GRASP-1," << endl;
    output_file << ",,,,,,No. of iterations,Average value,No. of iterations,Best value," << endl;

    vector<fs::directory_entry> files;
    for (const auto &entry : fs::directory_iterator(input_directory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry);
        }
    }
    
    sort(files.begin(), files.end(), [](const fs::directory_entry &a, const fs::directory_entry &b) {
        return a.path().filename().string() < b.path().filename().string();
    });
    
    for (const auto &entry : files) {
        string input_file = entry.path().string();
        generate_statistics_from_file(input_file, output_file_name, 10, output_file, known);
    }
    
    return 0;
}