#include "tree.hpp"

void processMissingValues(vector<Sample> &samples)
{
    if (samples.empty()) return;

    int numAttributes = samples[0].attributes.size();
    vector<bool> isNumeric(numAttributes, true);
    //Collectingg thre non-missing values for each attribute
    vector<vector<string> > attributeValues(numAttributes);
    for (const auto &sample : samples)
    {
        for (int i = 0; i < numAttributes; i++)
        {
            const string &val = sample.attributes[i];
            if (val != "?" && !val.empty())
            {
                attributeValues[i].push_back(val);
                if (!isNumber(val))
                    isNumeric[i] = false;
            }
        }
    }
    //replace korar jonno
    map<int, string> replacementValues;
    for (int i = 0; i < numAttributes; i++)
    {
        if (isNumeric[i])
        {
            double sum = 0.0;
            int count = 0;
            for (const string &val : attributeValues[i])
            {
                sum += stringToDouble(val);
                count++;
            }
            if (count > 0)
            {
                double mean = sum / count;
                replacementValues[i] = to_string(mean);
            }
        }
        else
        {
            // Calculating mode for nonnunmerics
            map<string, int> freq;
            for (const string &val : attributeValues[i])
                freq[val]++;
            string mode;
            int maxFreq = 0;
            for (const auto &pair : freq)
            {
                if (pair.second > maxFreq)
                {
                    maxFreq = pair.second;
                    mode = pair.first;
                }
            }
            replacementValues[i] = mode;
        }
    }
    for (auto &sample : samples)
    {
        for (int i = 0; i < numAttributes; i++)
        {
            string &val = sample.attributes[i];
            if (val == "?" || val.empty())
            {
                if (replacementValues.count(i))
                    val = replacementValues[i];
            }
        }
    }
}


vector<Sample> loadDataset(const string &filename, vector<string> &attributeNames)
{
    vector<Sample> samples;
    ifstream file(filename);
    string line;
    bool hasMissingValues = false;
    int cnt = 0;
    bool hasIdColumn = false;

    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return samples;
    }

    string firstLine;
    bool isHeader = true;
    vector<string> firstTokens;

    // Read the first line
    if (getline(file, firstLine))
    {
        stringstream ss(firstLine);
        string token;
        while (getline(ss, token, ','))
        {
            token.erase(remove(token.begin(), token.end(), '\r'), token.end());
            token.erase(remove(token.begin(), token.end(), '\n'), token.end());
            while (!token.empty() && isspace(token.back())) token.pop_back();
            while (!token.empty() && isspace(token.front())) token.erase(0, 1);
            firstTokens.push_back(token);
            if (isNumber(token)) isHeader = false;
            if (token == "?") { hasMissingValues = true; cnt++; }
        }
    }

    if (isHeader)
    {
        attributeNames = firstTokens;

        string firstAttrLower = attributeNames[0];
        if (firstAttrLower == "Id") {
            hasIdColumn = true;
            attributeNames.erase(attributeNames.begin()); 
        }
        //cout << "Header found with " << attributeNames.size() << " attributes." << endl;
        if (!attributeNames.empty()) attributeNames.pop_back();
    }
    else
    {
        // No header
        int attrCount = (int)firstTokens.size() - 1;
        attributeNames.clear();
        for (int i = 0; i < attrCount; ++i)
        {
            attributeNames.push_back("Attribute_" + to_string(i + 1));
        }
        //if (isNumber(firstTokens[0])) hasIdColumn = true;

        Sample firstSample;
        firstSample.attributes = vector<string>(firstTokens.begin(), firstTokens.end() - 1);
        //if (hasIdColumn) firstSample.attributes.erase(firstSample.attributes.begin());
        firstSample.label = firstTokens.back();
        samples.push_back(firstSample);
    }
    while (getline(file, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        string value;
        Sample sample;
        while (getline(ss, value, ','))
        {
            value.erase(remove(value.begin(), value.end(), '\r'), value.end());
            value.erase(remove(value.begin(), value.end(), '\n'), value.end());
            while (!value.empty() && isspace(value.back())) value.pop_back();
            while (!value.empty() && isspace(value.front())) value.erase(0, 1);
            sample.attributes.push_back(value);
            if (value == "?") { hasMissingValues = true; cnt++; }
        }

        if (!sample.attributes.empty())
        {
            sample.label = sample.attributes.back();
            sample.attributes.pop_back();
            if (hasIdColumn) {sample.attributes.erase(sample.attributes.begin());
            //cout << "Id column found and removed." << endl;
            }
            samples.push_back(sample);
        }
    }
    file.close();
    if (hasMissingValues) processMissingValues(samples);
    return samples;
}


pair<vector<Sample>, vector<Sample> > splitDataset(const vector<Sample> &samples, double trainRatio)
{
    vector<Sample> shuffled = samples;
    random_device rd;
    mt19937 g(rd());
    shuffle(shuffled.begin(), shuffled.end(), g);
    int trainSize = (int)(shuffled.size() * trainRatio);
    vector<Sample> trainSet(shuffled.begin(), shuffled.begin() + trainSize);
    vector<Sample> testSet(shuffled.begin() + trainSize, shuffled.end());
    return make_pair(trainSet, testSet);
}