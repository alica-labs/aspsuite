/*
 * NewSolverCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include <commands/NewSolverCommand.h>

#include "../include/gui/ConceptNetGui.h"

#include "../include/containers/SolverSettings.h"

#include <SystemConfig.h>
#include <iostream>

using namespace std;

namespace cng
{

	NewSolverCommand::NewSolverCommand(ConceptNetGui* gui, shared_ptr<SolverSettings> settings)
	{
		this->type = "New Solver";
		this->settings = settings;
		this->gui = gui;
		if (settings == nullptr)
		{
			getDefaultArguments();
		}
	}

	NewSolverCommand::~NewSolverCommand()
	{
	}

	void NewSolverCommand::execute()
	{
		this->gui->addToCommandHistory(shared_from_this());
		cout << "NewSolverCommand: create new solver" << endl;
		this->gui->setSettings(this->settings);
		//TODO create solver and pass it to the conceptnet gui
		this->gui->enableGui(true);
	}

	void NewSolverCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
		this->gui->enableGui(false);
		this->gui->clear();
		//delete solver ?
	}

	QJsonObject NewSolverCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "New Solver";
		ret["SettingsString"] = QString(this->settings->argString.c_str());
		return ret;
	}

	void NewSolverCommand::getDefaultArguments()
	{
		supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
		string argumentString = (*sc)["ConceptNetGui"]->get<string>("ConceptNetGui", "Default", "Parameters", NULL);
		this->settings = make_shared<SolverSettings>(argumentString);
	}

} /* namespace cng */
