#include "data.hpp"


int main(int argc, char *argv[])
{
    bool print = true;
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <criterion> <maxDepth>" << endl;
        cout << "Criterion: IG, IGR, or NWIG" << endl;
        cout << "MaxDepth: Maximum depth of tree (0 for no pruning)" << endl;
        return 1;
    }

    string criterion = argv[1];
    int maxDepth = stoi(argv[2]);
    if (criterion != "IG" && criterion != "IGR" && criterion != "NWIG")
    {
        cerr << "Error: Invalid criterion. Use IG, IGR, or NWIG." << endl;
        return 1;
    }
    //cout << "Enter dataset filename: ";
    string filename = "Datasets/Iris.csv";
    // cin >> filename;

    vector<string> attributeNames;
    vector<Sample> dataset = loadDataset(filename, attributeNames);

    if (dataset.empty())
    {
        cerr << "Error: Could not load dataset or dataset is empty." << endl;
        return 1;
    }

    if (!dataset.empty() && print)
    {
        // cout << "Dataset loaded successfully. Total samples: " << dataset.size() << endl;
        // cout << "Number of attributes: " << attributeNames.size() << endl;
        // cout << "Sample attributes: " << dataset[0].attributes.size() << endl;
        // cout << "First sample label: '" << dataset[0].label << "'" << endl;
        // cout << "First sample values: ";

        set<string> uniqueLabels;
        for (const auto &sample : dataset)
        {
            uniqueLabels.insert(sample.label);
        }
        // cout << "Unique labels (" << uniqueLabels.size() << "): ";
        // for (const string &label : uniqueLabels)
        // {
        //     cout << "'" << label << "' ";
        // }
        // cout << endl;
    }
    const int numExperiments = 20;
    double totalAccuracy = 0.0;
    int totalNodes = 0;
    int totalDepth = 0;
    int maxTreeDepth = 0;
    cout << "\nRunning " << numExperiments << " experiments with criterion: " << criterion
        << ", ARGmaxDepth: " << maxDepth << endl;

    for (int i = 0; i < numExperiments; i++)
    {
        auto split = splitDataset(dataset, 0.8);
        vector<Sample> trainSet = split.first;
        vector<Sample> testSet = split.second;
        DecisionTree tree(criterion, maxDepth);
        tree.setAttributes(attributeNames, trainSet);
        tree.train(trainSet);
        double accuracy = tree.evaluate(testSet);
        totalAccuracy += accuracy;

        if (print)
        {
            totalNodes += tree.getNodeCount();
            totalDepth += tree.getTreeDepth();
            maxTreeDepth = max(maxTreeDepth, tree.getTreeDepth());
        }
        if(print) cout << "Experiment " << (i + 1) << ": Accuracy = " << fixed << setprecision(4) << accuracy*100 << "%" << endl;
    }

    double avgAccuracy = totalAccuracy / numExperiments;
    //cout << "\nResults for criterion " << criterion << " with maxDepth " << maxTreeDepth << ":" << endl;
    cout << "\nResults for criterion " << criterion <<endl;
    cout << "Average Accuracy: " << fixed << setprecision(4) << avgAccuracy*100 << "%" << endl;

    if (print)
    {
        double avgNodes = (double)totalNodes / numExperiments;
        double avgDepth = (double)totalDepth / numExperiments;
        cout << "Average Number of Nodes: " << fixed << setprecision(2) << avgNodes << endl;
        cout << "Average Tree Depth: " << fixed << setprecision(2) << avgDepth << endl;
        cout << "Maximum Tree Depth: " << maxTreeDepth << endl;
    }
    return 0;
}