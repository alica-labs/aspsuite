/*
 * SolverSettings.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/containers/SolverSettings.h"
#include <SystemConfig.h>

namespace cng
{

	SolverSettings::SolverSettings(string argString)
	{
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
					break;
				}
				parsedParam = supplementary::Configuration::trim(this->argString.substr(start, end - start));
				start = this->argString.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
				this->args.push_back(parsedParam.c_str());
			}
		}
		else
		{
			this->args.push_back(this->argString.c_str());
		}
	}

} /* namespace cng */
