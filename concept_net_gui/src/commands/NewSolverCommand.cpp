/*
 * NewSolverCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include <commands/NewSolverCommand.h>

#include "../include/gui/ConceptNetGui.h"

#include <SystemConfig.h>
#include <iostream>

using namespace std;

namespace cng
{

	NewSolverCommand::NewSolverCommand(vector<char const*> arguments, ConceptNetGui* gui, string argumentString)
	{
		this->type = "New Solver";
		this->arguments = arguments;
		this->gui = gui;
		this->argumentString = argumentString;
		if (arguments.size() == 0)
		{
			getDefaultArguments();
		}
	}

	NewSolverCommand::~NewSolverCommand()
	{
	}

	void NewSolverCommand::execute()
	{
		cout << "NewSolverCommand: create new solver" << endl;
		//TODO create solver and pass it to the conceptnet gui
	}

	void NewSolverCommand::undo()
	{
		//delete solver ?
	}

	void NewSolverCommand::getDefaultArguments()
	{
		supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
		this->argumentString = (*sc)["ConceptNetGui"]->get<string>("ConceptNetGui", "Default", "Parameters", NULL);
		if (this->argumentString.find(",") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string parsedParam = "";
			while (start != string::npos)
			{
				end = this->argumentString.find(",", start);
				if (end == string::npos)
				{
					parsedParam = this->argumentString.substr(start, this->argumentString.length() - start);
					break;
				}
				parsedParam += this->argumentString.substr(start, end - start);
				start = this->argumentString.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
			}
			this->arguments.push_back(parsedParam.c_str());
		}
		else
		{
			this->arguments.push_back(this->argumentString.c_str());
		}
	}

} /* namespace cng */
