/*
 * SolveCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/SolveCommand.h"
#include "../include/gui/ConceptNetGui.h"

#include "../include/handler/CommandHistoryHandler.h"

namespace cng
{

	SolveCommand::SolveCommand(ConceptNetGui* gui)
	{
		this->type = "Solve";
		this->gui = gui;
	}

	SolveCommand::~SolveCommand()
	{
	}

	void SolveCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void SolveCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject SolveCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Solve";
		return ret;
	}

} /* namespace cng */
