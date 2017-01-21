/*
 * FactsQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/FactsQueryCommand.h"
#include "../include/gui/ConceptNetGui.h"

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
		this->gui->addToCommandHistory(shared_from_this());
	}

	void FactsQueryCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

} /* namespace cng */
