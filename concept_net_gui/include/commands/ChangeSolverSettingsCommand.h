/*
 * ChangeSolverSettingsCommand.h
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_CHANGESOLVERSETTINGSCOMMAND_H_
#define INCLUDE_COMMANDS_CHANGESOLVERSETTINGSCOMMAND_H_

#include <commands/Command.h>
#include <vector>

namespace cng
{

	class ConceptNetGui;
	class SettingsDialog;
	class ChangeSolverSettingsCommand : public Command
	{
	public:
		ChangeSolverSettingsCommand(vector<char const*> arguments, ConceptNetGui* gui, SettingsDialog* dialog, string currentSettings);
		virtual ~ChangeSolverSettingsCommand();

		void execute();
		void undo();

		vector<char const*> arguments;
		vector<char const*> previousArguments;
		string currentSettings;
		string previousSettings;
		ConceptNetGui* gui;
		SettingsDialog* dialog;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_CHANGESOLVERSETTINGSCOMMAND_H_ */
