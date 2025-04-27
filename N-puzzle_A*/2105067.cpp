#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <fstream>
using namespace std;

vector<int> node;
vector<int> goal;
vector<vector<int> > states;
map<vector<int>, int> stateID;
map<int, int> level;      
map<int, int> parent;        
int k;
int dx[] = {-1, 0, 1, 0};     
int dy[] = {0, -1, 0, 1};
int explored, expanded;

pair<int, int> coord2D(int n) {
    return make_pair(n / k, n % k);
}

int getLinearCoord(pair<int, int> cord) {
    return cord.first * k + cord.second;
}

int getZeroIndex(vector<int>& node) {
    for (int i = 0; i < node.size(); i++) {
        if (node[i] == 0) return i;
    }
    return -1;
}

int hamming(vector<int>& node, vector<int>& goal) {
    int hamming = 0;
    for (int i = 0; i < node.size(); i++) {
        if (node[i] != 0 && node[i] != goal[i]) {
            hamming++;
        }
    }
    return hamming;
}

int manhattan(vector<int>& node, vector<int>& goal) {
    int dist = 0;
    map<int, int> goalPos;
    for (int i = 0; i < goal.size(); i++) {
        goalPos[goal[i]] = i;
    }

    for (int i = 0; i < node.size(); i++) {
        int val = node[i];
        if (val == 0) continue;
        int goalIdx = goalPos[val];
        pair<int, int> goalCoord = coord2D(goalIdx);
        pair<int, int> nodeCoord = coord2D(i);
        dist += abs(goalCoord.first - nodeCoord.first) + abs(goalCoord.second - nodeCoord.second);
    }
    return dist;
}
int euclidean(vector<int>& node, vector<int>& goal) {
    int dist = 0;
    map<int, int> goalPos;
    for (int i = 0; i < goal.size(); i++) {
        goalPos[goal[i]] = i;
    }

    for (int i = 0; i < node.size(); i++) {
        int val = node[i];
        if (val == 0) continue;
        int goalIdx = goalPos[val];
        pair<int, int> goalCoord = coord2D(goalIdx);
        pair<int, int> nodeCoord = coord2D(i);
        dist += sqrt(pow(goalCoord.first - nodeCoord.first, 2) + pow(goalCoord.second - nodeCoord.second, 2));
    }
    return dist;
}

int LinearConflict(vector<int>& node, vector<int>& goal) {
    int conflict = 0;
    map<int, int> goalPos;
    for (int i = 0; i < goal.size(); i++) {
        goalPos[goal[i]] = i;
    }

    for (int row = 0; row < k; row++) {
        vector<int> currentRowTiles;
        for (int col = 0; col < k; col++) {
            int val = node[getLinearCoord(make_pair(row, col))];
            if (val == 0) continue;
            pair<int, int> goalCoord = coord2D(goalPos[val]);
            if (goalCoord.first == row) {
                currentRowTiles.push_back(val);
            }
        }

        for (int i = 0; i < currentRowTiles.size(); i++) {
            for (int j = i + 1; j < currentRowTiles.size(); j++) {
                int val1 = currentRowTiles[i];
                int val2 = currentRowTiles[j];
                int goalCol1 = coord2D(goalPos[val1]).second;
                int goalCol2 = coord2D(goalPos[val2]).second;

                if (goalCol1 > goalCol2) { 
                    conflict++;
                }
            }
        }
    }

    for (int col = 0; col < k; col++) {
        vector<int> currentColTiles;
        for (int row = 0; row < k; row++) {
            int val = node[getLinearCoord(make_pair(row, col))];
            if (val == 0) continue;
            pair<int, int> goalCoord = coord2D(goalPos[val]);
            if (goalCoord.second == col) { 
                currentColTiles.push_back(val);
            }
        }

        for (int i = 0; i < currentColTiles.size(); i++) {
            for (int j = i + 1; j < currentColTiles.size(); j++) {
                int val1 = currentColTiles[i];
                int val2 = currentColTiles[j];
                int goalRow1 = coord2D(goalPos[val1]).first;
                int goalRow2 = coord2D(goalPos[val2]).first;

                if (goalRow1 > goalRow2) { 
                    conflict++;
                }
            }
        }
    }

    return manhattan(node, goal) + 2 * conflict;
}


bool isSolvable(vector<int>& node) {
    int inversion = 0;
    for (int i = 0; i < node.size(); i++) {
        for (int j = i + 1; j < node.size(); j++) {
            if (node[i] > node[j] && node[i] != 0 && node[j] != 0) {
                inversion++;
            }
        }
    }
    if (k % 2 == 0) {
        int rowFromTop = 0;
        for (int i = 0; i < node.size(); i++) {
            if (node[i] == 0) {
                rowFromTop = i / k;
                break;
            }
        }
        int rowFromBottom = k - rowFromTop;
        return (inversion + rowFromBottom) % 2 == 1;
    } else {
        return inversion % 2 == 0;
    }
}

void reconstructPath(int goalID) {
    vector<vector<int> > path;
    for (int at = goalID; at != -1; at = parent[at]) {
        path.push_back(states[at]);
    }
    reverse(path.begin(), path.end());

    cout << "\nSteps to solve the puzzle:\n";
    for (auto& state : path) {
        for (int i = 0; i < state.size(); i++) {
            cout << state[i] << ' ';
            if ((i + 1) % k == 0) cout << endl;
        }
        cout << "\n";
    }
    cout << "minimum moves: " << path.size() - 1 << endl;
}

void ASTERsearch(int (*heuristic)(vector<int>&, vector<int>&)) {
    priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > pq;
    
    stateID.clear();
    level.clear();
    parent.clear();
    states.clear();
    explored = expanded = 0;

    set<vector<int> > closed; 

    vector<int> start = node;
    stateID[start] = 0;
    states.push_back(start);
    level[0] = 0;
    parent[0] = -1;
    int f = heuristic(start, goal);
    pq.push(make_pair(f, 0));

    while (!pq.empty()) {
        pair<int, int> top = pq.top();
        int id = top.second;
        pq.pop();
        expanded++;

        if (closed.count(states[id])) {
            continue;
        }
        closed.insert(states[id]);

        if (states[id] == goal) {
            reconstructPath(id);
            cout << "Explored: " << explored << ", Expanded: " << expanded << endl;
            return;
        }

        int zeroPos = getZeroIndex(states[id]);
        pair<int, int> zeroCoord = coord2D(zeroPos);

        for (int dir = 0; dir < 4; dir++) {
            int nx = zeroCoord.first + dx[dir];
            int ny = zeroCoord.second + dy[dir];

            if (nx >= 0 && nx < k && ny >= 0 && ny < k) {
                int newZeroPos = getLinearCoord(make_pair(nx, ny));
                vector<int> nextState = states[id];
                swap(nextState[zeroPos], nextState[newZeroPos]);

                if (closed.count(nextState)) {
                    continue;
                }

                if (stateID.count(nextState) == 0) {
                    explored++;
                    int newID = states.size();
                    stateID[nextState] = newID;
                    states.push_back(nextState);
                    level[newID] = level[id] + 1;
                    parent[newID] = id;

                    int g = level[newID];
                    int h = heuristic(nextState, goal);
                    pq.push(make_pair(g + h, newID));
                }
            }
        }
    }
    cout << "No solution found!" << endl;
}

int main(int argc, char* argv[]) {
    ifstream input("input.txt");
    if (!input) {
        cerr << "Error opening input file!" << endl;
        return 1;
    }

    input >> k;
    node.resize(k * k);
    goal.resize(k * k);

    for (int i = 0; i < k * k; i++) {
        input >> node[i];
    }

    input.close(); // Close input after reading

    for (int i = 0; i < (k * k) - 1; i++) {
        goal[i] = i + 1;
    }
    goal[(k * k) - 1] = 0;

    if (!isSolvable(node)) {
        cout << "Puzzle is NOT solvable.\n";
        return 0;
    }

    cout << "Puzzle is solvable.\n";

    int heuristicChoice = 0;
    if (argc >= 2) {
        heuristicChoice = atoi(argv[1]);
    }

    switch (heuristicChoice) {
        case 0:
            ASTERsearch(hamming);
            break;
        case 1:
            ASTERsearch(manhattan);
            break;
        case 2:
            ASTERsearch(euclidean);
            break;
        case 3:
            ASTERsearch(LinearConflict); 
            break;
        default:
            cout << "Invalid heuristic option! Using default (hamming).\n";
            ASTERsearch(hamming);
            break;
    }

    return 0;
}
