/*
 * VariableQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/VariableQueryCommand.h"

#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

namespace cng
{

	VariableQueryCommand::VariableQueryCommand(ConceptNetGui* gui)
	{
		this->type = "Variable Query";
		this->gui = gui;
	}

	VariableQueryCommand::~VariableQueryCommand()
	{
	}

	void VariableQueryCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void VariableQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject VariableQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Variable Query";
		return ret;
	}

} /* namespace cng */
