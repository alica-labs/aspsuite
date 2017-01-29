/*
 * FactsQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/FactsQueryCommand.h"
#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

namespace cng
{

	FactsQueryCommand::FactsQueryCommand(ConceptNetGui* gui)
	{
		this->type = "Facts Query";
		this->gui = gui;

	}

	FactsQueryCommand::~FactsQueryCommand()
	{
	}

	void FactsQueryCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void FactsQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject FactsQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Facts Query";
		return ret;
	}

} /* namespace cng */
