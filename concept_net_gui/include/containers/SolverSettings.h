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
	/**
	 * Class to encapsulate settings for the Solver
	 */
	class SolverSettings
	{
	public:
		/**
		 * Constructor
		 * @param argString string containing the params
		 */
		SolverSettings(string argString);
		virtual ~SolverSettings();

		/**
		 * Parsed params for solver const char*
		 */
		vector<const char*> args;
		/**
		 * Parsed params for solver vector<string>
		 */
		vector<string> argumentStrings;
		/**
		 * Not parsed param string
		 */
		string argString;
	private:
		/**
		 * Extracts both settings vectors from argString
		 */
		void extractSettingsVector();
	};

} /* namespace cng */

#endif /* INCLUDE_CONTAINERS_SOLVERSETTINGS_H_ */
