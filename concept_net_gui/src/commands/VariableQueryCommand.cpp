/*
 * VariableQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include <commands/VariableQueryCommand.h>
#include "../include/gui/ConceptNetGui.h"

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
		this->gui->addToCommandHistory(shared_from_this());
	}

	void VariableQueryCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject VariableQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Variable Query";
		return ret;
	}

} /* namespace cng */
