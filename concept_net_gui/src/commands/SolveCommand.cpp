/*
 * SolveCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/SolveCommand.h"
#include "../include/gui/ConceptNetGui.h"

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
		this->gui->addToCommandHistory(shared_from_this());
	}

	void SolveCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

} /* namespace cng */
