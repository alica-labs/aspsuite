/*
 * GroundCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/GroundCommand.h"
#include "../include/gui/ConceptNetGui.h"

#include "../include/handler/CommandHistoryHandler.h"

#include <SystemConfig.h>

namespace cng
{

	GroundCommand::GroundCommand(ConceptNetGui* gui, QString program)
	{
		this->type = "Ground";
		this->gui = gui;
		this->program = program;
		extractProgramSection();
	}

	GroundCommand::~GroundCommand()
	{
	}

	void GroundCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void GroundCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject GroundCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Ground";
		ret["program"] = this->program;
		return ret;
	}

	void GroundCommand::extractProgramSection()
	{
		size_t prefixLength = string("#program").length();
		string tmp = this->program.toStdString();
		size_t start = tmp.find("#program");
		if (start != string::npos)
		{
			tmp = tmp.substr(start, tmp.length() - start - 1);
			this->historyProgramSection = tmp;
			size_t end = tmp.find_first_of(".");
			this->programSection = supplementary::Configuration::trim(tmp.substr(prefixLength, end - prefixLength));
		}
	}

} /* namespace cng */
