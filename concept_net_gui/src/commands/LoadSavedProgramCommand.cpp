/*
 * LoadSavedProgramCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "gui/ConceptNetGui.h"
#include "gui/SettingsDialog.h"

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

namespace cng
{

	LoadSavedProgramCommand::LoadSavedProgramCommand(ConceptNetGui* gui, QString fileName, QByteArray loadedData)
	{
		this->type = "Load Saved Program";
		this->gui = gui;
		this->loadedData = loadedData;
		this->fileName = fileName;
	}

	LoadSavedProgramCommand::~LoadSavedProgramCommand()
	{
	}

	void LoadSavedProgramCommand::execute()
	{
		this->gui->commandHistory.clear();
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		QJsonDocument loadDoc(QJsonDocument::fromJson(this->loadedData));
		QJsonObject savedObject = loadDoc.object();
		QJsonArray cmds = savedObject["commandHistory"].toArray();
		for (int i = 0; i < cmds.size(); i++)
		{

			QJsonObject cmd = cmds[i].toObject();
			cout << "LoadSavedProgramCommand: " << cmd["type"].toString().toStdString() << " " << i + 1 << "/"
					<< cmds.size() << endl;
			if (cmd["type"].toString().toStdString().compare("New Solver") == 0)
			{
				shared_ptr<NewSolverCommand> c = make_shared<NewSolverCommand>(
						this->gui,
						make_shared<SolverSettings>(cmd["name"].toString().toStdString(),
													cmd["settingsString"].toString().toStdString()));
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Change Settings") == 0)
			{
				shared_ptr<ChangeSolverSettingsCommand> c = make_shared<ChangeSolverSettingsCommand>(
						this->gui,
						this->gui->settingsDialog,
						make_shared<SolverSettings>(cmd["name"].toString().toStdString(),
													cmd["settingsString"].toString().toStdString()));
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Concept Net") == 0)
			{
				shared_ptr<ConceptNetQueryCommand> c = make_shared<ConceptNetQueryCommand>(this->gui);
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Facts Query") == 0)
			{
				shared_ptr<FactsQueryCommand> c = make_shared<FactsQueryCommand>(this->gui);
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Ground") == 0)
			{
				shared_ptr<GroundCommand> c = make_shared<GroundCommand>(this->gui, cmd["program"].toString());
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Load Logic Program") == 0)
			{
				shared_ptr<LoadBackgroundKnowledgeCommand> c = make_shared<LoadBackgroundKnowledgeCommand>(
						this->gui, cmd["fileName"].toString());
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Load Saved Program") == 0)
			{
				/**
				 * This command should never be part of a saved program,
				 * since it will result in a recursive load call!
				 */
//				shared_ptr<LoadSavedProgramCommand> c = make_shared<LoadSavedProgramCommand>(
//						this->gui, this->fileName ,this->loadedData));
//				c->execute();
//				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Variable Query") == 0)
			{
				shared_ptr<VariableQueryCommand> c = make_shared<VariableQueryCommand>(this->gui, cmd["program"].toString());
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else if (cmd["type"].toString().toStdString().compare("Solve") == 0)
			{
				shared_ptr<SolveCommand> c = make_shared<SolveCommand>(this->gui);
				c->execute();
				emit this->gui->updateCommandList();
				continue;
			}
			else
			{
				cout << "LoadSavedProgramCommand: Command with unknown type found!" << endl;
			}
		}
	}

	void LoadSavedProgramCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject LoadSavedProgramCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Load Saved Program";
		ret["fileName"] = this->fileName;
		return ret;
	}

} /* namespace cng */
