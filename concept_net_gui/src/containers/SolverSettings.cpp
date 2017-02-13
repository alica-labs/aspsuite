/*
 * SolverSettings.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "containers/SolverSettings.h"
#include <Configuration.h>

namespace cng
{

	SolverSettings::SolverSettings(string name, string argString)
	{
		this->name = name;
		this->argString = argString;
		extractSettingsVector();

	}

	SolverSettings::~SolverSettings()
	{
		for(auto arg : this->args)
		{
			free((char*)arg);
		}
	}

	void SolverSettings::extractSettingsVector()
	{
		if (this->argString.find(",") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string parsedParam = "";
			while (start != string::npos)
			{
				end = this->argString.find(",", start);
				if (end == string::npos)
				{
					parsedParam = supplementary::Configuration::trim(this->argString.substr(start, this->argString.length() - start));
					this->argumentStrings.push_back(parsedParam);
					break;
				}
				parsedParam = supplementary::Configuration::trim(this->argString.substr(start, end - start));
				start = this->argString.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
				this->argumentStrings.push_back(parsedParam);
			}
		}
		else
		{
			this->argumentStrings.push_back(this->argString);
		}
		for (int i = 0; i < this->argumentStrings.size(); i++)
		{ // add params to startParams
			this->args.push_back(strdup(this->argumentStrings[i].c_str()));
		}
		this->args.push_back(nullptr);
	}

} /* namespace cng */
