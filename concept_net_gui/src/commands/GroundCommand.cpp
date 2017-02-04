/*
 * GroundCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/GroundCommand.h"
#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

#include <Configuration.h>

#include <asp_solver/ASPSolver.h>

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
		string aspString = program.toStdString();
		this->gui->getSolver()->add(this->programSection, {}, aspString);
		this->gui->getSolver()->ground( { {this->programSection, {}}}, nullptr);
	}

	void GroundCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		//TODO extend every rule with an external to remove them ?
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
			size_t end = tmp.find_first_of(".");
			tmp = tmp.substr(start, end - start);
			this->historyProgramSection = tmp;
			this->programSection = supplementary::Configuration::trim(tmp.substr(prefixLength, end - prefixLength));
		}
	}

} /* namespace cng */
