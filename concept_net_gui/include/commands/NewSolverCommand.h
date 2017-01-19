/*
 * NewSolverCommand.h
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_
#define INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_

#include "../include/commands/Command.h"
#include <vector>

namespace cng
{

	class ConceptNetGui;
	class NewSolverCommand : public Command
	{
	public:
		NewSolverCommand(vector<char const*> arguments, ConceptNetGui* gui, string argumentString);
		virtual ~NewSolverCommand();

		void execute();
		void undo();

		vector<char const*> arguments;
		string argumentString;
		ConceptNetGui* gui;

	private:
		void getDefaultArguments();
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_ */
