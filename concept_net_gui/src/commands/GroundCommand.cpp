/*
 * GroundCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/GroundCommand.h"
#include "../include/gui/ConceptNetGui.h"

namespace cng
{

	GroundCommand::GroundCommand(ConceptNetGui* gui, QString program)
	{
		this->type = "Ground";
		this->gui = gui;
		this->program = program;
		this->programSection = extractProgramSection();
	}

	GroundCommand::~GroundCommand()
	{
	}

	void GroundCommand::execute()
	{
		this->gui->addToCommandHistory(shared_from_this());
	}

	void GroundCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

	string GroundCommand::extractProgramSection()
	{
		string ret;
		//TODO
		return ret;
	}

} /* namespace cng */
