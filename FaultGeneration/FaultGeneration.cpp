#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <cstdio>
#include <regex>
#include <algorithm>
#include <set>
#include <random>
#include <cstdio>

using namespace std;
random_device rd;
mt19937 generator(rd());

void numberNodes(const string& inputFileName,
    map<string, int>& inputNodes,
    map<string, int>& outputNodes,
    map<string, int>& wireNodes)
{

    int count = 1;

    ifstream inputFile(inputFileName);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the input file." << endl;
        return;
    }

    string line;

    while (getline(inputFile, line))
    {
        // Use regular expressions to match input, output, and wire declarations
        smatch match;

        // Match input declarations (e.g., input [7:0] A, B, C;)
        if (regex_search(line, match, regex(R"(input\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string inputDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(inputDeclaration.begin(), inputDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                // inputPorts.push_back(wordIterator->str());
                string input = wordIterator->str();
                inputNodes[input] = count++;
            }
        }

        // Match output declarations (e.g., output [3:0] X, Y, Z;)
        if (regex_search(line, match, regex(R"(output\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string outputDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(outputDeclaration.begin(), outputDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                // outputPorts.push_back(wordIterator->str());
                string output = wordIterator->str();
                outputNodes[output] = count++;
            }
        }

        // Match wire declarations (e.g., wire W, X, Y, Z;)
        if (regex_search(line, match, regex(R"(wire\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string wireDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(wireDeclaration.begin(), wireDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                // wireNames.push_back(wordIterator->str());
                string wire = wordIterator->str();
                wireNodes[wire] = count++;
            }
        }
    }

    // Close input file
    inputFile.close();
}

int random(vector<int> const& numbers)
{

    uniform_int_distribution<int> dis(0, numbers.size());
    int r = dis(generator);
    return numbers[r];
}

// Function to find the key values
// according to given mapped value K
void printKey(map<string, int>& inputMap, map<string, int>& wireMap, map<string, int>& outputMap, vector<int>& K, vector<string>& faultnames)
{

    // If a is true, then we have
    // not key-value mapped to K
    bool a = true;

    // Traverse the map
    for (auto& it : inputMap)
    {

        // If mapped value is K,
        // then print the key value
        for (auto& it2 : K)
        {
            if (it.second == it2)
            {
                // cout << it.first << ' ';
                faultnames.push_back(it.first);
                a = false;
            }
        }
    }

    for (auto& it : wireMap)
    {

        // If mapped value is K,
        // then print the key value
        for (auto& it2 : K)
        {
            if (it.second == it2)
            {
                // cout << it.first << ' ';
                faultnames.push_back(it.first);
                a = false;
            }
        }
    }

    for (auto& it : outputMap)
    {

        // If mapped value is K,
        // then print the key value
        for (auto& it2 : K)
        {
            if (it.second == it2)
            {
                // cout << it.first << ' ';
                faultnames.push_back(it.first);
                a = false;
            }
        }
    }

    // If there is not key mapped with K,
    // then print -1
    if (a)
    {
        cout << "-1";
    }
}

void faultfile(const string& inputFileName, const string& outputFileName, vector<string>& faultnames, int value)
{

    ifstream inputfile(inputFileName);
    ofstream outputfile(outputFileName);

    bool startreading = false;
    // bool insideModule = false;
    vector<string> modulelines;
    // string newInput = "f";

    if (!inputfile.is_open())
    {
        cerr << "Error: Could not open the input file." << endl;
        return;
    }

    if (!outputfile.is_open())
    {
        cerr << "Error: Could not create the output file." << endl;
        return;
    }

    string line;
    while (getline(inputfile, line))
    {
       // bool startreading;
        if (line.find("AND") != string::npos || line.find("OR") != string::npos ||
            line.find("NAND") != string::npos || line.find("NOR") != string::npos ||
            line.find("XOR") != string::npos || line.find("NOT") != string::npos)
        {
            startreading = true;
        }

        if (startreading)
        {
            if (value == 1)
            {
                for (const auto& i : faultnames)
                {
                    if (line.find(i) != string::npos)
                    {
                        line.replace(line.find(i), i.length(), i + " | f");
                    }
                }
            }

            else
            {

                for (const auto& i : faultnames)
                {
                    if (line.find(i) != string::npos)
                    {
                        line.replace(line.find(i), i.length(), i + " & ~f");
                    }
                }
            }
        }

        outputfile << line << endl;
    }

    inputfile.close();
    //
    outputfile.close();
}

string extractModuleName(const string& inputFileName)
{

    ifstream inputFile(inputFileName);
    string line;
    while (getline(inputFile, line))
    {
        smatch match;
        if (regex_search(line, match, regex("module\\s+([^\\s(]+)\\s*\\(")))
        {
            return match[1];
        }
    }

    //
    return "";
}

void extractVerilogNames(const string& inputFileName,
    vector<string>& inputPorts,
    vector<string>& outputPorts,
    vector<string>& wireNames)
{
    ifstream inputFile(inputFileName);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the input file." << endl;
        return;
    }

    string line;

    while (getline(inputFile, line))
    {
        // Use regular expressions to match input, output, and wire declarations
        smatch match;

        // Match input declarations (e.g., input [7:0] A, B, C;)
        if (regex_search(line, match, regex(R"(input\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string inputDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(inputDeclaration.begin(), inputDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                inputPorts.push_back(wordIterator->str());
            }
        }

        // Match output declarations (e.g., output [3:0] X, Y, Z;)
        if (regex_search(line, match, regex(R"(output\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string outputDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(outputDeclaration.begin(), outputDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                outputPorts.push_back(wordIterator->str());
            }
        }

        // Match wire declarations (e.g., wire W, X, Y, Z;)
        if (regex_search(line, match, regex(R"(wire\s*(?:\[\d+(?::\d+)?\])?\s*([^,;]+)\s*[,;])")))
        {
            string wireDeclaration = match[1];
            // Split the declaration into individual names
            regex wordRegex(R"((?:\\?\w+(?:\[\d+(?::\d+)?\])*(?:\[\d+(?::\d+)?\])?)+)"); // Capture signal names with escape characters and array indices
            sregex_iterator wordIterator(wireDeclaration.begin(), wireDeclaration.end(), wordRegex);
            sregex_iterator end;
            for (; wordIterator != end; ++wordIterator)
            {
                wireNames.push_back(wordIterator->str());
            }
        }
    }

    // Close input file
    inputFile.close();
}

void writeVerilogModule(const string& moduleName, const vector<string>& inputs, const vector<string>& outputs, const vector<string>& wires, ofstream& outputFile)
{
    // Write module declaration
    outputFile << "module " << moduleName << " (" << endl;

    // Write input ports
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        outputFile << inputs[i];
        if (i < inputs.size() - 1)
            outputFile << ", ";
        else if (!outputs.empty() || !wires.empty())
            outputFile << ","; // Add a comma if there are outputs or wires
        outputFile << endl;
    }

    // Write output ports
    for (size_t i = 0; i < outputs.size(); ++i)
    {
        outputFile << outputs[i];
        if (i < outputs.size() - 1)
            outputFile << ", ";
        // else if (!wires.empty())
        //     outputFile << ","; // Add a comma if there are wires
        outputFile << endl;
    }

    // Close module declaration
    outputFile << ");" << endl;

    // Write wire declarations
    for (size_t i = 0; i < wires.size(); ++i)
    {
        outputFile << "  wire " << wires[i] << ";" << endl;
    }
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        outputFile << "  input " << inputs[i] << " ;" << endl;
    }
    for (size_t i = 0; i < outputs.size(); ++i)
    {
        outputFile << "  output " << outputs[i] << " ;" << endl;
    }

    // Close input file

    // outputFile.close();
}

void extractCombinationalGates(const string& inputFileName, const string& outputFileName)
{
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName, ios_base::app);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the input file." << endl;
        return;
    }

    if (!outputFile.is_open())
    {
        cerr << "Error: Could not create the output file." << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool startReading = false;   // Flag to indicate when to start reading
    bool insideDFFBlock = false; // Flag to indicate when inside a DFF block

    // Read input Verilog file line by line
    while (getline(inputFile, line))
    {
        // Check if the line contains "module" (indicating the module declaration)
        if (line.find("module") != string::npos)
        {
            // Skip module declaration
            startReading = false;
        }

        // Check if the line contains "input" or "output" (indicating ports)
        if (line.find("input") != string::npos || line.find("output") != string::npos || line.find("wire") != string::npos)
        {
            // Skip ports
        }

        // Check if the line contains "DFF" (indicating a D flip-flop gate)
        else if (line.find("DFF") != string::npos || line.find("DFFSR") != string::npos)
        {
            // Set the insideDFFBlock flag when entering a DFF block
            insideDFFBlock = true;
            // Skip the DFF line
            continue;
        }

        // Check if the line contains gates (e.g., "AND", "OR", "NAND", "NOR", "XOR")
        else if (line.find("AND") != string::npos || line.find("OR") != string::npos ||
            line.find("NAND") != string::npos || line.find("NOR") != string::npos ||
            line.find("XOR") != string::npos || line.find("NOT") != string::npos)
        {
            // Only start reading if not inside a DFF block
            if (!insideDFFBlock)
            {
                startReading = true; // Start reading from the first gate
            }
        }

        // Check if the line marks the end of a DFF block
        else if (insideDFFBlock && line.find(");") != string::npos)
        {
            insideDFFBlock = false; // Reset the insideDFFBlock flag
        }

        // Continue reading if the flag is set
        if (startReading)
        {
            // Pass through other lines without modification (combinational gates)
            outputFile << line << endl;

            // Check if the line ends with a semicolon and marks the end of a gate
            if (line.back() == ';')
            {
                startReading = false; // Stop reading after the gate
            }
        }
    }

    outputFile << "endmodule" << endl;
    // Close input file
    inputFile.close();

    // Close output file
    outputFile.close();
}

vector<int> ReadUniqueIntegersFromFile(const string& filename)
{
    vector<int> intVector;

    ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        return intVector; // Return an empty vector in case of an error
    }

    string line;

    while (getline(inputFile, line))
    {
        int value = atoi(line.c_str());
        intVector.push_back(value);
    }

    inputFile.close();

    return intVector;
}

int main()
{

    map<string, int> inputNodes;
    map<string, int> outputNodes;
    map<string, int> wireNodes;
    vector<int> inputs;
    vector<int> outputs;
    vector<int> wires;
    vector<int> allNodes;
    vector<string> testvector;
    vector<int> faultnodename;
    int value;

    const string inputFileName = "MUX32_Syn.v"; // Replace with your input Verilog file name
    const string outputFileName = "new.v";      // const string outputFileName = "EX_comb.v";   // Replace with the desired output Verilog file name
    const string newOutputFileName = "mux32fault.v";
    const string allNodesFile = "mux32faultnodes.txt";
    const string faultoutput = "muxoutfaultnodes.txt"; // const string outputFileName = "EX_comb.v";   // Replace with the desired output Verilog file name
    // const char[] name = "new.v";
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName);
    ofstream newoutputFile(newOutputFileName);
    ofstream output2(faultoutput);
    ofstream outputAllNodes(allNodesFile);

    numberNodes(inputFileName, inputNodes, outputNodes, wireNodes);

    map<string, int>::iterator itr;
    for (itr = inputNodes.begin(); itr != inputNodes.end(); itr++)
    {
        inputs.push_back(itr->second);
    }

    for (itr = outputNodes.begin(); itr != outputNodes.end(); itr++)
    {
        //cout << itr->first << " " << itr->second << endl;
        outputs.push_back(itr->second);
    }

    for (itr = wireNodes.begin(); itr != wireNodes.end(); itr++)
    {
        //cout << itr->first << " " << itr->second << endl;
        wires.push_back(itr->second);
    }

    allNodes.insert(allNodes.end(), inputs.begin(), inputs.end());
    allNodes.insert(allNodes.end(), inputs.begin(), inputs.end());
    allNodes.insert(allNodes.end(), wires.begin(), wires.end());

    if (!outputAllNodes.is_open())
    {
        cerr << "Error: Could not create the output file." << endl;
        return 1;
    }

    for (const auto& i : allNodes)
    {
        outputAllNodes << i << endl;
    }

    outputAllNodes.close();

    // int faults;
    //  int value;
    //   vector<int> faultnodename;
    string filename = "mux32faultnodes.txt"; // Replace with the path to your text file
    vector<int> uniqueIntegers = ReadUniqueIntegersFromFile(filename);

    // Use a set to remove duplicates
    set<int> uniqueInts(uniqueIntegers.begin(), uniqueIntegers.end());

    vector<int> faultsVector(uniqueInts.begin(), uniqueInts.end());

    if (!output2.is_open())
    {
        cerr << "Error: Could not create the output file." << endl;
        return 1;
    }

    for (const auto& i : faultsVector)
    {
        output2 << i << endl;
    }

    output2.close();

    // cout << "Enter the number of fault nodes: ";
    // cin >> faults;
    cout << "Enter the stuck at value: ";
    cin >> value;
    // for (size_t i = 0; i < faults; i++)
    // {
    //     int node = random(inputs);
    //     // cout << node << endl;
    //     faultnodename.push_back(node);
    // }

    vector<string> faultnames;
    printKey(inputNodes, wireNodes, outputNodes, faultsVector, faultnames);

    for (const auto& i : faultnames)
    {
        //cout << i << endl;
    }
    string modulename;
    vector<string> inputPorts;
    vector<string> outputPorts;
    vector<string> wirePorts;

    modulename = extractModuleName(inputFileName);
    extractVerilogNames(inputFileName, inputPorts, outputPorts, wirePorts);
    inputPorts.push_back("f");
    writeVerilogModule(modulename, inputPorts, outputPorts, wirePorts, outputFile);
    extractCombinationalGates(inputFileName, outputFileName);
    faultfile(outputFileName, newOutputFileName, faultnames, value);

    outputFile.close();
    if (remove("new.v") == 0)
    {
        cout << "Deleted successfully" << endl;
    }
    else
    {
        cout << "Not deleted";
    }
    if (remove("mux32faultnodes.txt") == 0)
    {
        cout << "Deleted successfully" << endl;
    }
    else
    {
        cout << "Not deleted";
    }

    return 0;
}