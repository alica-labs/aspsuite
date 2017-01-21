/*
 * LoadSavedProgramCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/LoadSavedProgramCommand.h"
#include "../include/gui/ConceptNetGui.h"

namespace cng
{

	LoadSavedProgramCommand::LoadSavedProgramCommand(ConceptNetGui* gui)
	{
		this->type = "Load Saved Program";
		this->gui = gui;
	}

	LoadSavedProgramCommand::~LoadSavedProgramCommand()
	{
		// TODO Auto-generated destructor stub
	}

	void LoadSavedProgramCommand::execute()
	{
		this->gui->addToCommandHistory(shared_from_this());
	}

	void LoadSavedProgramCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

} /* namespace cng */
