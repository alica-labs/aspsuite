/*
 * AssignExternalCommand.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: stefan
 */

#include <commands/AssignExternalCommand.h>
#include <commands/SolveCommand.h>

#include "handler/CommandHistoryHandler.h"

#include <asp_solver/ASPSolver.h>
#include <gui/KnowledgebaseCreator.h>

#include <QString>

#include <ui_knowledgebasecreator.h>

namespace kbcr
{

	AssignExternalCommand::AssignExternalCommand(KnowledgebaseCreator* gui, QString externalName, QString truthValue)
	{
		this->type = "Assign External";
		this->gui = gui;
		this->externalName = externalName;
		this->truthValue = truthValue;
		this->previousTruthValue = false;
		createHistoryString();
	}

	AssignExternalCommand::~AssignExternalCommand()
	{
	}

	void AssignExternalCommand::execute()
	{
		auto external = this->gui->getSolver()->getGringoModule()->parseValue(this->externalName.toStdString(), nullptr,
																				20);
		auto symbolPos = this->gui->getSolver()->clingo->lookup(external);
		if (!this->gui->getSolver()->clingo->valid(symbolPos) || !this->gui->getSolver()->clingo->external(symbolPos))
		{
			this->gui->getUi()->externalTextEdit->setText(QString("Invalid or Released External!"));
			this->undo();
			return;
		}
		if (external.sign() == 0)
		{
			this->previousTruthValue = true;
		}
		if (this->truthValue.compare("True") == 0)
		{
			this->gui->getSolver()->assignExternal(external, Potassco::Value_t::True);
		}
		else if (this->truthValue.compare("False") == 0)
		{
			this->gui->getSolver()->assignExternal(external, Potassco::Value_t::False);
		}
		else
		{
			this->gui->getSolver()->assignExternal(external, Potassco::Value_t::Release);
		}
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		std::shared_ptr<SolveCommand> sc = std::make_shared<SolveCommand>(this->gui);
		sc->execute();
		emit this->gui->updateExternalList();
	}

	void AssignExternalCommand::undo()
	{
		if (!this->truthValue.contains("Release"))
		{
			if (this->previousTruthValue)
			{
				this->gui->getSolver()->assignExternal(
						this->gui->getSolver()->getGringoModule()->parseValue(this->externalName.toStdString(), nullptr,
																				20), Potassco::Value_t::True);
			}
			else
			{
				this->gui->getSolver()->assignExternal(
						this->gui->getSolver()->getGringoModule()->parseValue(this->externalName.toStdString(), nullptr,
																				20), Potassco::Value_t::False);
			}
			std::shared_ptr<SolveCommand> sc = std::make_shared<SolveCommand>(this->gui);
			sc->execute();
			emit this->gui->updateExternalList();
		}
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

} /* namespace kbcr */
