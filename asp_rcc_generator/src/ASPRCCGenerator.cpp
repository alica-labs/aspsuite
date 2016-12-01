/*
 * ASPRCCGenerator.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: stefan
 */

#include "asp_rcc_generator/ASPRCCGenerator.h"
#include "ros/ros.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		ASPRCCGenerator::ASPRCCGenerator(int numberOfRooms, int percentageStep)
		{
			this->numberOfRooms = numberOfRooms;
			this->percentageStep = percentageStep;
		}

		ASPRCCGenerator::~ASPRCCGenerator()
		{
		}

		void ASPRCCGenerator::generateLPFile()
		{
			ofstream myfile;
			myfile.open("example.txt");
			myfile << "Writing this to a file.\n";
			myfile.close();
		}

	} /* namespace reasoner */
} /* namespace alica */

void printUsage()
{
	cout << "Usage: ./asp_rcc_generator -p \"Percentage step for predicates\" -n \"number of rooms to connect\"" << endl;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printUsage();
		return 0;
	}

	cout << "Parsing command line parameters:" << endl;

	string p = "";
	string n = ".";

	for (int i = 1; i < argc; i++)
	{
		if (string(argv[i]) == "-p")
		{
			p = argv[i + 1];
			i++;
		}

		if (string(argv[i]) == "-n")
		{
			n = argv[i + 1];
			i++;
		}
	}
	if (p.size() == 0 || n.size() == 0)
	{
		printUsage();
		return 0;
	}
	cout << "\tPercentage is:       \"" << p << "\"" << endl;
	cout << "\tNumber is:           \"" << n << "\"" << endl;

	alica::reasoner::ASPRCCGenerator* gen = new alica::reasoner::ASPRCCGenerator(std::stoi(n), std::stoi(p));
	gen->generateLPFile();

	return 0;
}

