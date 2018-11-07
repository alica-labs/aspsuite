/*
 * ASPRCCGenerator.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: stefan
 */

#include "asp_rcc_generator/ASPRCCGenerator.h"

using namespace std;

namespace alica
{
namespace reasoner
{

ASPRCCGenerator::ASPRCCGenerator(int endCount, int step, int percentageStep)
{
    this->endCount = endCount;
    this->step = step;
    this->percentageStep = percentageStep;
}

ASPRCCGenerator::~ASPRCCGenerator() {}

void ASPRCCGenerator::generateLPFile()
{
    stringstream ss;
    for (int i = step; i <= this->endCount; i += step) {
        for (int j = this->percentageStep; j <= 100; j += this->percentageStep) {
            ofstream myfile;
            cout << "ASPRCCGenerator: generating: "
                 << "asprcc_" << i << "_" << this->endCount << "_" << j << ".lp" << endl;
            ss << "asprcc_" << i << "_" << this->endCount << "_" << j << ".lp";
            myfile.open(ss.str());
            ss.str("");
            myfile << "#program asprccgen.\n";
            myfile << "\n";
            myfile << "room(0"
                   << ".." << i - 1 << ").\n";
            myfile << "\n";
            myfile << "reachable(X, Y) :- reachable(Y, X), X!=Y.\n";
            myfile << "reachable(X, Z) :- reachable(X, Y), reachable(Y,Z), X!=Y, Y!=Z, X!=Z.\n";
            myfile << "reachable(X, Y) :- partialOverlapping(X, Y), room(X), room(Y).\n";
            myfile << createRandomConnections(i, j);
            myfile.close();
        }
    }
}

string ASPRCCGenerator::createRandomConnections(int size, int percentage)
{
    stringstream ss;
    auto nodes = selectRandomNodes(size, percentage);
    for (int i = 0; i < nodes.size() - 1; i++) {
        ss << "partialOverlapping(" << nodes.at(i) << ", " << nodes.at(i + 1) << ").\n";
    }
    return ss.str();
}

vector<int> ASPRCCGenerator::selectRandomNodes(int size, int percentage)
{
    vector<int> ret;
    int count = (int) (size * (percentage / 100.0));
    srand(time(NULL));
    while (ret.size() != count) {
        int r = rand() % size;
        if (find(ret.begin(), ret.end(), r) == ret.end()) {
            ret.push_back(r);
        }
    }
    return ret;
}

} /* namespace reasoner */
} /* namespace alica */

void printUsage()
{
    cout << "Usage: ./asp_rcc_generator -p \"Percentage step for predicates  \" -s \"step size\" \" -e \"max number of rooms\"" << endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage();
        return 0;
    }

    cout << "Parsing command line parameters:" << endl;

    string percent = "";
    string stop = "";
    string step = "";

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-p") {
            percent = argv[i + 1];
            i++;
        }

        if (string(argv[i]) == "-s") {
            step = argv[i + 1];
            i++;
        }

        if (string(argv[i]) == "-e") {
            stop = argv[i + 1];
            i++;
        }
    }
    if (percent.size() == 0 || stop.size() == 0 || step.size() == 0) {
        printUsage();
        return 0;
    }
    cout << "\tPercentage is:       \"" << percent << "\"" << endl;
    cout << "\tStep is:           \"" << step << "\"" << endl;
    cout << "\tNumber is:           \"" << stop << "\"" << endl;

    alica::reasoner::ASPRCCGenerator* gen = new alica::reasoner::ASPRCCGenerator(std::stoi(stop), std::stoi(step), std::stoi(percent));
    gen->generateLPFile();

    return 0;
}
