#include "sample.hpp"
struct TreeNode
{
    bool isLeaf;
    string label;         // For leaf nodes
    int attributeIndex;   // For internal nodes
    string attributeName; // For internal nodes
    map<string, TreeNode *> children;
    TreeNode() : isLeaf(false), attributeIndex(-1) {}
    ~TreeNode()
    {
        for (auto &child : children)
        {
            delete child.second;
        }
    }
};

class DecisionTree
{
private:
    TreeNode *root;
    vector<AttributeInfo> attributes;
    string criterion;
    int maxDepth;
    vector<Sample> trainingData;
    void discretizeNumericalAttributes(vector<Sample> &samples)
    {
        for (int attrIdx = 0; attrIdx < attributes.size(); attrIdx++)
        {
            if (!attributes[attrIdx].isNumerical)
                continue;
            // unique numerical values
            set<double> values;
            for (const auto &sample : samples)
            {
                if (isNumber(sample.attributes[attrIdx]))
                {
                    values.insert(stringToDouble(sample.attributes[attrIdx]));
                }
            }
            if (values.empty())
                continue;
            vector<double> sortedValues(values.begin(), values.end());
            sort(sortedValues.begin(), sortedValues.end());
            // int numBins = min(10, max(5, (int)sqrt(values.size())));
            int numBins = 10;
            //cout << values.size() << endl;
            if (values.size() <= numBins)
            {
                attributes[attrIdx].thresholds = sortedValues; // for too less unique values
            }
            else
            {
                double minVal = sortedValues[0];
                double maxVal = sortedValues.back();
                double binWidth = (maxVal - minVal) / numBins;

                attributes[attrIdx].thresholds.clear();
                for (int i = 1; i < numBins; i++)
                {
                    attributes[attrIdx].thresholds.push_back(minVal + i * binWidth);
                }
            }

            // Convert numerical values to categorical bins
            for (auto &sample : samples)
            {
                if (isNumber(sample.attributes[attrIdx]))
                {
                    double value = stringToDouble(sample.attributes[attrIdx]);
                    int bin = 0;
                    for (double threshold : attributes[attrIdx].thresholds)
                    {
                        if (value <= threshold)
                            break;
                        bin++;
                    }
                    sample.attributes[attrIdx] = to_string(bin);
                }
            }
        }
    }
    double calculateEntropy(const vector<Sample> &samples)
    {
        if (samples.empty())
            return 0.0;

        map<string, int> labelCounts;
        for (const auto &sample : samples)
        {
            labelCounts[sample.label]++;
        }

        double entropy = 0.0;
        int totalSamples = samples.size();
        for (const auto &pair : labelCounts)
        {
            double probability = (double)pair.second / totalSamples;
            if (probability > 0)
            {
                entropy -= probability * log2(probability);
            }
        }
        return entropy;
    }
    vector<Sample> splitDataset(const vector<Sample> &samples, int attributeIndex, const string &value)
    {
        vector<Sample> subset;
        for (const auto &sample : samples)
        {
            if (sample.attributes[attributeIndex] == value)
            {
                subset.push_back(sample);
            }
        }
        return subset;
    }

    set<string> getUniqueValues(const vector<Sample> &samples, int attributeIndex)
    {
        set<string> uniqueValues;
        for (const auto &sample : samples)
        {
            uniqueValues.insert(sample.attributes[attributeIndex]);
        }
        return uniqueValues;
    }

    double calculateIG(const vector<Sample> &samples, int attributeIndex)
    {
        double originalEntropy = calculateEntropy(samples);
        set<string> uniqueValues = getUniqueValues(samples, attributeIndex);

        double weightedEntropy = 0.0;
        int totalSamples = samples.size();

        for (const string &value : uniqueValues)
        {
            vector<Sample> subset = splitDataset(samples, attributeIndex, value);
            if (!subset.empty())
            {
                double weight = (double)subset.size() / totalSamples;
                weightedEntropy += weight * calculateEntropy(subset);
            }
        }

        return originalEntropy - weightedEntropy;
    }

    double calculateIGR(const vector<Sample> &samples, int attributeIndex)
    {
        double ig = calculateIG(samples, attributeIndex);
        set<string> uniqueValues = getUniqueValues(samples, attributeIndex);

        // Calculate iv
        double iv = 0.0;
        int totalSamples = samples.size();
        for (const string &value : uniqueValues)
        {
            vector<Sample> subset = splitDataset(samples, attributeIndex, value);
            if (!subset.empty())
            {
                double ratio = (double)subset.size() / totalSamples;
                if (ratio > 0)
                {
                    iv -= ratio * log2(ratio);
                }
            }
        }
        if (iv == 0.0)
            return 0.0;

        return ig / iv;
    }

    double calculateNWIG(const vector<Sample> &samples, int attributeIndex)
    {
        double ig = calculateIG(samples, attributeIndex);
        set<string> uniqueValues = getUniqueValues(samples, attributeIndex);

        int k = uniqueValues.size();
        int datasetSize = samples.size();

        double normalizationFactor = log2(k + 1);
        double weightingFactor = 1.0 - (double)(k - 1) / datasetSize;
        if (weightingFactor < 0)
            weightingFactor = 0;

        return (ig / normalizationFactor) * weightingFactor;
    }
    int selectBestAttribute(const vector<Sample> &samples, const vector<int> &availableAttributes)
    {
        if (availableAttributes.empty())
            return -1;

        int bestAttribute = availableAttributes[0];
        double bestScore = -1.0;

        for (int attrIndex : availableAttributes)
        {
            double score = 0.0;
            if (criterion == "IG")
            {
                score = calculateIG(samples, attrIndex);
            }
            else if (criterion == "IGR")
            {
                score = calculateIGR(samples, attrIndex);
            }
            else if (criterion == "NWIG")
            {
                score = calculateNWIG(samples, attrIndex);
            }

            if (score > bestScore)
            {
                bestScore = score;
                bestAttribute = attrIndex;
            }
        }

        return bestAttribute;
    }
    string getMajorityLabel(const vector<Sample> &samples)
    {
        if (samples.empty())
            return "";

        map<string, int> labelCounts;
        for (const auto &sample : samples)
        {
            labelCounts[sample.label]++;
        }

        string majorityLabel = samples[0].label;
        int maxCount = 0;
        for (const auto &pair : labelCounts)
        {
            if (pair.second > maxCount)
            {
                maxCount = pair.second;
                majorityLabel = pair.first;
            }
        }
        return majorityLabel;
    }

    bool allSameLabel(const vector<Sample> &samples)
    {
        if (samples.empty())
            return true;

        string firstLabel = samples[0].label;
        for (const auto &sample : samples)
        {
            if (sample.label != firstLabel)
            {
                return false;
            }
        }
        return true;
    }
    TreeNode *buildTree(const vector<Sample> &samples, vector<int> availableAttributes, int currentDepth)
    {
        TreeNode *node = new TreeNode();

        if (samples.empty())
        {
            node->isLeaf = true;
            node->label = "";
            return node;
        }

        if (allSameLabel(samples) || availableAttributes.empty() ||
            (maxDepth > 0 && currentDepth >= maxDepth))
        {
            node->isLeaf = true;
            node->label = getMajorityLabel(samples);
            return node;
        }

        int bestAttribute = selectBestAttribute(samples, availableAttributes);
        if (bestAttribute == -1)
        {
            node->isLeaf = true;
            node->label = getMajorityLabel(samples);
            return node;
        }

        node->attributeIndex = bestAttribute;
        node->attributeName = attributes[bestAttribute].name;

        vector<int> remainingAttributes;
        for (int attr : availableAttributes)
        {
            if (attr != bestAttribute)
            {
                remainingAttributes.push_back(attr);
            }
        }

        // Create children for each unique value
        set<string> uniqueValues = getUniqueValues(samples, bestAttribute);
        for (const string &value : uniqueValues)
        {
            vector<Sample> subset = splitDataset(samples, bestAttribute, value);
            node->children[value] = buildTree(subset, remainingAttributes, currentDepth + 1);
        }
        return node;
    }

    string predict(TreeNode *node, const Sample &sample, const vector<Sample> &trainingSamples)
    {
        if (!node)
            return getMajorityLabel(trainingSamples);

        if (node->isLeaf)
        {
            return node->label;
        }

        string attributeValue = sample.attributes[node->attributeIndex];
        if (node->children.find(attributeValue) != node->children.end())
        {
            return predict(node->children[attributeValue], sample, trainingSamples);
        }

        // If attribute value not seen in training, return the most common label from children
        map<string, int> childLabelCounts;
        for (const auto &child : node->children)
        {
            if (child.second->isLeaf && !child.second->label.empty())
            {
                childLabelCounts[child.second->label]++;
            }
        }

        if (!childLabelCounts.empty())
        {
            string bestLabel = childLabelCounts.begin()->first;
            int maxCount = childLabelCounts.begin()->second;
            for (const auto &pair : childLabelCounts)
            {
                if (pair.second > maxCount)
                {
                    maxCount = pair.second;
                    bestLabel = pair.first;
                }
            }
            return bestLabel;
        }
        return getMajorityLabel(trainingSamples);
    }
    int countNodes(TreeNode *node)
    {
        if (!node)
            return 0;

        int count = 1;
        for (const auto &child : node->children)
        {
            count += countNodes(child.second);
        }
        return count;
    }
    int calculateDepth(TreeNode *node)
    {
        if (!node || node->isLeaf)
            return 0;

        int maxDepth = 0;
        for (const auto &child : node->children)
        {
            maxDepth = max(maxDepth, calculateDepth(child.second));
        }
        return maxDepth + 1;
    }

public:
    DecisionTree(const string &crit, int maxD) : root(nullptr), criterion(crit), maxDepth(maxD) {}

    ~DecisionTree()
    {
        delete root;
    }

    void setAttributes(const vector<string> &attributeNames, const vector<Sample> &samples)
    {
        attributes.clear();
        for (int i = 0; i < attributeNames.size(); i++)
        {
            AttributeInfo attr;
            attr.name = attributeNames[i];

            // check if numerical
            for (const auto &sample : samples)
            {
                if (i < sample.attributes.size())
                {
                    if (isNumber(sample.attributes[i]))
                    {
                        attr.isNumerical = true;
                    }
                }
            }
            attributes.push_back(attr);
        }
    }
    void train(const vector<Sample> &trainingSamples)
    {
        if (trainingSamples.empty())
            return;

        vector<Sample> processedSamples = trainingSamples;
        discretizeNumericalAttributes(processedSamples);
        trainingData = processedSamples;
        vector<int> availableAttributes;
        for (int i = 0; i < attributes.size(); i++)
        {
            availableAttributes.push_back(i);
        }

        delete root;
        root = buildTree(processedSamples, availableAttributes, 0);
    }
    double evaluate(const vector<Sample> &testSamples)
    {
        if (testSamples.empty())
            return 0.0;

        int correct = 0;
        for (const auto &sample : testSamples)
        {
            // Processinagg thre test sample the same way as training samples
            Sample processedSample = sample;
            processTestSample(processedSample);

            string predicted = predict(root, processedSample, trainingData);
            if (predicted == sample.label)
            {
                correct++;
            }
        }

        return (double)correct / testSamples.size();
    }

    // Processing a single test sample for prediction
    void processTestSample(Sample &sample)
    {
        for (int attrIdx = 0; attrIdx < attributes.size() && attrIdx < sample.attributes.size(); attrIdx++)
        {
            if (!attributes[attrIdx].isNumerical)
                continue;

            if (isNumber(sample.attributes[attrIdx]))
            {
                double value = stringToDouble(sample.attributes[attrIdx]);
                int bin = 0;
                for (double threshold : attributes[attrIdx].thresholds)
                {
                    if (value <= threshold)
                        break;
                    bin++;
                }
                sample.attributes[attrIdx] = to_string(bin);
            }
        }
    }

    int getNodeCount()
    {
        return countNodes(root);
    }

    int getTreeDepth()
    {
        return calculateDepth(root);
    }
    void printTree(TreeNode *node = nullptr, int depth = 0)
    {
        if (node == nullptr)
            node = root;
        if (!node)
            return;

        string indent(depth * 2, ' ');

        if (node->isLeaf)
        {
            cout << indent << "Leaf: " << node->label << endl;
        }
        else
        {
            cout << indent << "Node: " << node->attributeName << endl;
            for (const auto &child : node->children)
            {
                cout << indent << "  " << child.first << " -> " << endl;
                printTree(child.second, depth + 2);
            }
        }
    }
};