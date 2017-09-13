/*
 * ChangeSolverSettingsCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include <gui/KnowledgebaseCreator.h>
#include "commands/ChangeSolverSettingsCommand.h"

#include "containers/SolverSettings.h"

#include "handler/CommandHistoryHandler.h"

#include "gui/SettingsDialog.h"

namespace kbcr
{

	ChangeSolverSettingsCommand::ChangeSolverSettingsCommand(KnowledgebaseCreator* gui, SettingsDialog* dialog, std::shared_ptr<SolverSettings> settings)
	{
		this->previousSettings = gui->getSettings();
		this->gui = gui;
		this->dialog = dialog;
		this->currentSettings = settings;
		this->type = "Change Settings";
	}

	ChangeSolverSettingsCommand::~ChangeSolverSettingsCommand()
	{
	}

	void ChangeSolverSettingsCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		this->gui->setSettings(this->currentSettings);
	}

	void ChangeSolverSettingsCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		this->gui->setSettings(this->previousSettings);
	}

	QJsonObject kbcr::ChangeSolverSettingsCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Change Settings";
		ret["settingsString"] = QString(this->currentSettings->argString.c_str());
		ret["name"] = QString(this->currentSettings->name.c_str());
		return ret;
	}

} /* namespace kbcr */

