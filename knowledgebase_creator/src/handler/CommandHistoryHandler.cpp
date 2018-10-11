/*
 * CommandHistoryHandler.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#include <gui/KnowledgebaseCreator.h>
#include "handler/CommandHistoryHandler.h"

#include "containers/SolverSettings.h"

#include "commands/NewSolverCommand.h"
#include "commands/ChangeSolverSettingsCommand.h"
#include "commands/ConceptNetQueryCommand.h"
#include "commands/Command.h"
#include "commands/FactsQueryCommand.h"
#include "commands/GroundCommand.h"
#include "commands/LoadBackgroundKnowledgeCommand.h"
#include "commands/LoadSavedProgramCommand.h"
#include "commands/SolveCommand.h"
#include "commands/VariableQueryCommand.h"
#include "commands/AddCommand.h"
#include "commands/AssignExternalCommand.h"
#include "commands/OfferServicesCommand.h"

#include <ui_knowledgebasecreator.h>

namespace kbcr
{

	CommandHistoryHandler::CommandHistoryHandler(KnowledgebaseCreator* gui)
	{
		this->gui = gui;
	}

	CommandHistoryHandler::~CommandHistoryHandler()
	{
	}

	void CommandHistoryHandler::drawHistoryTable()
	{
		this->gui->getUi()->commandHistoryTable->clear();
		this->gui->getUi()->commandHistoryTable->setColumnCount(2);
		this->gui->getUi()->commandHistoryTable->setHorizontalHeaderLabels(QStringList {"Command", "Parameters", NULL});
		this->gui->getUi()->commandHistoryTable->setColumnWidth(0, 200);
		this->gui->getUi()->commandHistoryTable->horizontalHeader()->stretchLastSection();
		this->gui->getUi()->commandHistoryTable->setRowCount(0);
	}

	void CommandHistoryHandler::undoHistory()
	{
		auto lastIndex = this->gui->commandHistory.size() - 1;
		auto cmd = this->gui->commandHistory.at(lastIndex);
		std::cout << "KnowledgebaseCreator: undo: " << cmd->getType() << std::endl;
		cmd->undo();
	}

	void CommandHistoryHandler::removeFocus()
	{
		this->gui->getUi()->aspRuleTextArea->clearFocus();
	}

	void CommandHistoryHandler::fillCommandHistory()
	{
		/*drawHistoryTable();
		//fill history table with right commands
		for (auto cmd : this->gui->commandHistory)
		{
			int pos = this->gui->getUi()->commandHistoryTable->rowCount();
			this->gui->getUi()->commandHistoryTable->insertRow(pos);

			auto item = new QTableWidgetItem(QString(cmd->getType().c_str()));
			item->setFlags(item->flags() ^ Qt::ItemIsEditable);
			this->gui->getUi()->commandHistoryTable->setItem(pos, 0, item);

			if (cmd->getType().compare("New Solver") == 0)
			{
				this->addNewSolverCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Change Settings") == 0)
			{
				this->addChangeSolverSettingsCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Concept Net") == 0)
			{
				this->addConceptNetQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Facts Query") == 0)
			{
				this->addFactsQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Ground") == 0)
			{
				this->addGroundCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Load Logic Program") == 0)
			{
				this->addLoadBackgroundKnowledgeCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Load Saved Program") == 0)
			{
				this->addLoadSavedProgramCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Variable Query") == 0)
			{
				this->addVariableQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Solve") == 0)
			{
				this->addSolveCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Add") == 0)
			{
				this->addAddCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Assign External") == 0)
			{
				this->addAssignExternalCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Offer Services") == 0)
			{
				this->addOfferServiceCommandToHistory(cmd, pos);
				continue;
			}
			else
			{
				std::cout << "KnowledgebaseCreator: Command with unknown type found!" << std::endl;
				auto item2 = new QTableWidgetItem(QString("Command with unknown type found!"));
				item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
				this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item2);
			}
		}
		this->gui->getUi()->commandHistoryTable->scrollToBottom();*/
	}

	void CommandHistoryHandler::addNewSolverCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(
				QString(std::dynamic_pointer_cast<NewSolverCommand>(cmd)->settings->argString.c_str()));
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addChangeSolverSettingsCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item =
				new QTableWidgetItem(
						QString(std::dynamic_pointer_cast<ChangeSolverSettingsCommand>(cmd)->currentSettings->argString.c_str()));
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addConceptNetQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<ConceptNetQueryCommand>(cmd)->query);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addGroundCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(
				QString(std::dynamic_pointer_cast<GroundCommand>(cmd)->historyProgramSection.c_str()));
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addLoadSavedProgramCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<LoadSavedProgramCommand>(cmd)->fileName);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addSolveCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		QString satisfiable;
		if (std::dynamic_pointer_cast<SolveCommand>(cmd)->isSatisfiable())
		{
			satisfiable = QString("satisfied");
		}
		else
		{
			satisfiable = QString("not satisfied");
		}
		auto item = new QTableWidgetItem(satisfiable);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addVariableQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<VariableQueryCommand>(cmd)->toShow);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addLoadBackgroundKnowledgeCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<LoadBackgroundKnowledgeCommand>(cmd)->fileName);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addFactsQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<FactsQueryCommand>(cmd)->factsString);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addAddCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(
				QString(std::dynamic_pointer_cast<AddCommand>(cmd)->historyProgramSection.c_str()));
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addAssignExternalCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<AssignExternalCommand>(cmd)->historyString);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addOfferServiceCommandToHistory(std::shared_ptr<Command> cmd, int pos)
	{
		auto item = new QTableWidgetItem(std::dynamic_pointer_cast<OfferServicesCommand>(cmd)->queryString);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, item);
	}

	void CommandHistoryHandler::addToCommandHistory(std::shared_ptr<Command> cmd)
	{
		//this->gui->commandHistory.push_back(cmd);
		//emit this->gui->updateCommandList();
	}

	void CommandHistoryHandler::removeFromCommandHistory(std::shared_ptr<Command> cmd)
	{
		auto it = std::find_if(this->gui->commandHistory.begin(), this->gui->commandHistory.end(),
								[cmd](std::shared_ptr<Command> const& i)
								{	return i.get() == cmd.get();});
		if (it != this->gui->commandHistory.end())
		{
			this->gui->commandHistory.erase(it);
		}

		emit this->gui->updateCommandList();
	}

} /* namespace kbcr */

