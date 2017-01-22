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
#include <memory>
#include <QJsonObject>

namespace cng
{

	class SolverSettings;
	class ConceptNetGui;
	class SettingsDialog;
	class ChangeSolverSettingsCommand : public Command, public enable_shared_from_this<ChangeSolverSettingsCommand>
	{
	public:
		ChangeSolverSettingsCommand(ConceptNetGui* gui, SettingsDialog* dialog, string currentSettings);
		virtual ~ChangeSolverSettingsCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		shared_ptr<SolverSettings> previousSettings;
		shared_ptr<SolverSettings> currentSettings;
		ConceptNetGui* gui;
		SettingsDialog* dialog;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_CHANGESOLVERSETTINGSCOMMAND_H_ */
