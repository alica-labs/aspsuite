/*
 * LoadLogicProgramCommand.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: stefan
 */

#include "commands/LoadBackgroundKnowledgeCommand.h"

#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

#include <ui_conceptnetgui.h>

namespace cng
{

	LoadBackgroundKnowledgeCommand::LoadBackgroundKnowledgeCommand(ConceptNetGui* gui, QString fileName)
	{
		this->type = "Load Logic Program";
		this->gui = gui;
		this->fileName = fileName;
	}

	LoadBackgroundKnowledgeCommand::~LoadBackgroundKnowledgeCommand()
	{
	}

	void LoadBackgroundKnowledgeCommand::execute()
	{
		//Open file
		QFile file(this->fileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			qWarning("Couldn't open file.");
			return;
		}
		this->fileContent = file.readAll();
		file.close();
		this->gui->getUi()->aspRuleTextArea->setText(fileContent);
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void LoadBackgroundKnowledgeCommand::undo()
	{
		this->gui->getUi()->aspRuleTextArea->clear();
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject LoadBackgroundKnowledgeCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Load Logic Program";
		ret["fileName"] = this->fileName;
		return ret;
	}

} /* namespace cng */
