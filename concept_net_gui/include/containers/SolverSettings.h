/*
 * SolverSettings.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_CONTAINERS_SOLVERSETTINGS_H_
#define INCLUDE_CONTAINERS_SOLVERSETTINGS_H_

#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace cng
{

	class SolverSettings
	{
	public:
		SolverSettings(string argString);
		virtual ~SolverSettings();

		vector<const char*> args;
		vector<string> argumentStrings;
		string argString;
	private:
		void extractSettingsVector();
	};

} /* namespace cng */

#endif /* INCLUDE_CONTAINERS_SOLVERSETTINGS_H_ */
