/*
 * ChangeSolverSettingsCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include "../include/commands/ChangeSolverSettingsCommand.h"

#include "../include/containers/SolverSettings.h"

#include "../include/gui/ConceptNetGui.h"
#include "../include/gui/SettingsDialog.h"

namespace cng
{

	ChangeSolverSettingsCommand::ChangeSolverSettingsCommand(ConceptNetGui* gui, SettingsDialog* dialog, string currentSettings)
	{
		this->previousSettings = gui->getSettings();
		this->gui = gui;
		this->dialog = dialog;
		this->currentSettings = make_shared<SolverSettings>(currentSettings);
		this->type = "Change Settings";
	}

	ChangeSolverSettingsCommand::~ChangeSolverSettingsCommand()
	{
	}

	void ChangeSolverSettingsCommand::execute()
	{
		this->gui->setSettings(this->currentSettings);
	}

	void ChangeSolverSettingsCommand::undo()
	{
		this->gui->setSettings(this->previousSettings);
	}

} /* namespace cng */
