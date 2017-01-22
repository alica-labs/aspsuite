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

	LoadSavedProgramCommand::LoadSavedProgramCommand(ConceptNetGui* gui, QByteArray loadedData)
	{
		this->type = "Load Saved Program";
		this->gui = gui;
		this->loadedData = loadedData;
	}

	LoadSavedProgramCommand::~LoadSavedProgramCommand()
	{
	}

	void LoadSavedProgramCommand::execute()
	{
		this->gui->addToCommandHistory(shared_from_this());
		QJsonDocument loadDoc(QJsonDocument::fromJson(this->loadedData));
	}

	void LoadSavedProgramCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject LoadSavedProgramCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Load Saved Program";
		return ret;
	}

} /* namespace cng */
