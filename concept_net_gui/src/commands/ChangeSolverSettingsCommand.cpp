/*
 * ChangeSolverSettingsCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include <commands/ChangeSolverSettingsCommand.h>

#include "../include/gui/ConceptNetGui.h"
#include "../include/gui/SettingsDialog.h"

namespace cng
{

	ChangeSolverSettingsCommand::ChangeSolverSettingsCommand(vector<char const*> arguments, ConceptNetGui* gui, SettingsDialog* dialog, string currentSettings)
	{
		this->previousArguments = gui->getArguments();
		this->gui = gui;
		this->dialog = dialog;
		this->arguments = arguments;
		this->type = "Change Settings";
		this->previousSettings = gui->getArgumentString();
		this->currentSettings = currentSettings;
	}

	ChangeSolverSettingsCommand::~ChangeSolverSettingsCommand()
	{
		// TODO Auto-generated destructor stub
	}

	void ChangeSolverSettingsCommand::execute()
	{
		this->gui->setArguments(this->arguments);
		this->gui->setArgumentString(this->currentSettings);
	}

	void ChangeSolverSettingsCommand::undo()
	{
		this->gui->setArguments(this->previousArguments);
		this->gui->setArgumentString(this->previousSettings);
	}

} /* namespace cng */
