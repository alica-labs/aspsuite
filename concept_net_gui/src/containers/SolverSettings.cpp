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
					this->args.push_back(parsedParam.c_str());
					this->argumentStrings.push_back(parsedParam);
					break;
				}
				parsedParam = supplementary::Configuration::trim(this->argString.substr(start, end - start));
				start = this->argString.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
				this->args.push_back(parsedParam.c_str());
				this->args.push_back(nullptr);
				this->argumentStrings.push_back(parsedParam);
			}
		}
		else
		{
			this->args.push_back(this->argString.c_str());
			this->args.push_back(nullptr);
			this->argumentStrings.push_back(this->argString);
		}
	}

} /* namespace cng */
