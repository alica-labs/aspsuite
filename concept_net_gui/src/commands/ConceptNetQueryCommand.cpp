/*
 * ConceptNetQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/ConceptNetQueryCommand.h"
#include "../include/gui/ConceptNetGui.h"

namespace cng
{

	ConceptNetQueryCommand::ConceptNetQueryCommand(ConceptNetGui* gui)
	{
		this->type = "Concept Net";
		this->gui = gui;
	}

	ConceptNetQueryCommand::~ConceptNetQueryCommand()
	{
	}

	void ConceptNetQueryCommand::execute()
	{
		this->gui->addToCommandHistory(shared_from_this());
	}

	void ConceptNetQueryCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

} /* namespace cng */
