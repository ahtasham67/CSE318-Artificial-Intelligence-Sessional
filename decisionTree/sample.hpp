#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

struct Sample
{
    vector<string> attributes;
    string label;
};

struct AttributeInfo
{
    string name;
    bool isNumerical;
    vector<double> thresholds;
};

bool isNumber(const string &str)
{
    if (str.empty())
        return false;
    char *end;
    strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0';
}

double stringToDouble(const string &str)
{
    return strtod(str.c_str(), nullptr);
}