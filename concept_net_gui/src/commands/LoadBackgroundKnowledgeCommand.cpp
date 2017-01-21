/*
 * LoadLogicProgramCommand.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: stefan
 */

#include <commands/LoadBackgroundKnowledgeCommand.h>
#include "../include/gui/ConceptNetGui.h"
#include <ui_conceptnetgui.h>

namespace cng
{

	LoadBackgroundKnowledgeCommand::LoadBackgroundKnowledgeCommand(ConceptNetGui* gui, QByteArray file, QString fileName)
	{
		this->type = "Load Logic Program";
		this->gui = gui;
		this->fileName = fileName;
		this->file = file;
	}

	LoadBackgroundKnowledgeCommand::~LoadBackgroundKnowledgeCommand()
	{
	}

	void LoadBackgroundKnowledgeCommand::execute()
	{
		this->gui->getUi()->aspRuleTextArea->setText(this->file);
		this->gui->addToCommandHistory(shared_from_this());
	}

	void LoadBackgroundKnowledgeCommand::undo()
	{
		this->gui->getUi()->aspRuleTextArea->clear();
		this->gui->removeFromCommandHistory(shared_from_this());
	}

} /* namespace cng */
