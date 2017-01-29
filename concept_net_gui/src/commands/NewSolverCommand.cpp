/*
 * NewSolverCommand.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include <commands/NewSolverCommand.h>

#include "gui/ConceptNetGui.h"

#include "containers/SolverSettings.h"

#include "handler/CommandHistoryHandler.h"

#include <SystemConfig.h>
#include <iostream>

#include <asp_commons/IASPSolver.h>
#include <asp_solver/ASPSolver.h>

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
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		this->gui->setSettings(this->settings);
		std::vector<char const *> args {"clingo", "-W", "no-atom-undefined", "--number=1", nullptr};
		this->gui->setSolver(new reasoner::ASPSolver(this->settings->args));
		this->gui->enableGui(true);
	}

	void NewSolverCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		this->gui->enableGui(false);
		this->gui->clear();
		delete this->gui->getSolver();
		this->gui->setSolver(nullptr);
	}

	QJsonObject NewSolverCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "New Solver";
		ret["settingsString"] = QString(this->settings->argString.c_str());
		return ret;
	}

	void NewSolverCommand::getDefaultArguments()
	{
		supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
		string argumentString = (*sc)["ConceptNetGui"]->get<string>("ConceptNetGui", "Default", "Parameters", NULL);
		this->settings = make_shared<SolverSettings>(argumentString);
	}

} /* namespace cng */
