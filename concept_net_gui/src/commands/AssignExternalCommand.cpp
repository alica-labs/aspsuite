/*
 * AssignExternalCommand.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: stefan
 */

#include <commands/AssignExternalCommand.h>

#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

#include <asp_solver/ASPSolver.h>

#include <QString>

namespace cng
{

	AssignExternalCommand::AssignExternalCommand(ConceptNetGui* gui, QString externalName, QString truthValue)
	{
		this->type = "Assign External";
		this->gui = gui;
		this->externalName = externalName;
		this->truthValue = truthValue;
		createHistoryString();
	}

	AssignExternalCommand::~AssignExternalCommand()
	{
	}

	void AssignExternalCommand::execute()
	{
		auto external = this->gui->getSolver()->getGringoModule()->parseValue(this->externalName.toStdString(), nullptr, 20);
		auto tmp = this->gui->getSolver()->clingo->lookup(external);
		if(!this->gui->getSolver()->clingo->external(tmp))
		{
			this->undo();
			return;
		}
//		this->gui->getSolver()->assignExternal()
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void AssignExternalCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject AssignExternalCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Assign External";
		ret["external"] = this->externalName;
		ret["truthValue"] = this->truthValue;
		return ret;
	}

	void AssignExternalCommand::createHistoryString()
	{
		QString tmp = "";
		tmp.append(this->externalName).append(" Value: ").append(this->truthValue);
		this->historyString = tmp;
	}

} /* namespace cng */
