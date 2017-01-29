/*
 * CommandHistoryHandler.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#include "handler/CommandHistoryHandler.h"

#include "gui/ConceptNetGui.h"

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

#include <ui_conceptnetgui.h>

namespace cng
{

	CommandHistoryHandler::CommandHistoryHandler(ConceptNetGui * gui)
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
		cout << "ConceptNetGui: undo: " << cmd->getType() << endl;
		cmd->undo();
	}

	void CommandHistoryHandler::removeFocus()
	{
		this->gui->getUi()->aspRuleTextArea->clearFocus();
	}

	void CommandHistoryHandler::fillCommandHistory()
		{
			drawHistoryTable();
			for (auto cmd : this->gui->commandHistory)
			{
				int pos = this->gui->getUi()->commandHistoryTable->rowCount();
				this->gui->getUi()->commandHistoryTable->insertRow(pos);

				auto tmp = new QTableWidgetItem(QString(cmd->getType().c_str()));
				tmp->setFlags(tmp->flags() ^ Qt::ItemIsEditable);
				this->gui->getUi()->commandHistoryTable->setItem(pos, 0, tmp);

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
				else
				{
					cout << "ConceptNetGui: Command with unknown type found!" << endl;
					auto tmp2 = new QTableWidgetItem(QString("Command with unknown type found!"));
					tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
					this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
				}
			}
			this->gui->getUi()->commandHistoryTable->scrollToBottom();
		}

	void CommandHistoryHandler::addNewSolverCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<NewSolverCommand>(cmd)->settings->argString.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addChangeSolverSettingsCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<ChangeSolverSettingsCommand>(cmd)->currentSettings->argString.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addConceptNetQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addGroundCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<GroundCommand>(cmd)->historyProgramSection.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addLoadSavedProgramCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString(dynamic_pointer_cast<LoadSavedProgramCommand>(cmd)->fileName));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addSolveCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		QString satisfiable;
		if(dynamic_pointer_cast<SolveCommand>(cmd)->isSatisfiable())
		{
			satisfiable = QString("satisfied");
		}
		else
		{
			satisfiable = QString("not satisfied");
		}
		auto tmp2 = new QTableWidgetItem(satisfiable);
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addVariableQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addLoadBackgroundKnowledgeCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString(dynamic_pointer_cast<LoadBackgroundKnowledgeCommand>(cmd)->fileName));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addFactsQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->gui->getUi()->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void CommandHistoryHandler::addToCommandHistory(shared_ptr<Command> cmd)
	{
		this->gui->commandHistory.push_back(cmd);
		emit this->gui->updateCommandList();
	}

	void CommandHistoryHandler::removeFromCommandHistory(shared_ptr<Command> cmd)
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

} /* namespace cng */
