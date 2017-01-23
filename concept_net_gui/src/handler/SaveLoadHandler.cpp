/*
 * SaveLoadHandler.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#include "../include/handler/SaveLoadHandler.h"

#include "../include/gui/ConceptNetGui.h"
#include "../include/gui/SettingsDialog.h"
#include <ui_conceptnetgui.h>
#include <ui_settingsdialog.h>

#include "../include/commands/NewSolverCommand.h"
#include "../include/commands/ChangeSolverSettingsCommand.h"
#include "../include/commands/ConceptNetQueryCommand.h"
#include "../include/commands/Command.h"
#include "../include/commands/FactsQueryCommand.h"
#include "../include/commands/GroundCommand.h"
#include "../include/commands/LoadBackgroundKnowledgeCommand.h"
#include "../include/commands/LoadSavedProgramCommand.h"
#include "../include/commands/SolveCommand.h"
#include "../include/commands/VariableQueryCommand.h"

#include <QString>
#include <QFileDialog>

namespace cng
{

	SaveLoadHandler::SaveLoadHandler(ConceptNetGui* gui)
	{
		this->gui = gui;

	}

	SaveLoadHandler::~SaveLoadHandler()
	{
	}

	void SaveLoadHandler::saveProgram()
	{
		//Open save file dialog to save a program
		QString filename = QFileDialog::getSaveFileName(this->gui->parentWidget(), tr("Save Programm"), QDir::currentPath(),
														tr("ConceptNetGui Program File (*.cnlp)"), 0,
														QFileDialog::DontUseNativeDialog);

		if (!filename.endsWith(".cnlp"))
		{
			filename += ".cnlp";
		}

		//Check if the user selected a correct file
		if (!filename.isNull())
		{

			//Create file
			QFile file(filename);

			if (!file.open(QIODevice::WriteOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

			QJsonObject history;

			//JSON Array to hold the commandHistory
			QJsonArray jsonCommandHistory;
			for (auto cmd : this->gui->commandHistory)
			{
				//Skip load saved commandHistory to avoid recursive loading
				if(cmd->getType().compare("Load Saved Program") == 0)
				{
					continue;
				}
				jsonCommandHistory.append(cmd->toJSON());
			}
			history["commandHistory"] = jsonCommandHistory;
			//Write to file
			QJsonDocument saveDoc(history);
			file.write(saveDoc.toJson());
		}
	}

	void SaveLoadHandler::saveModels()
	{
		//Open save file dialog to save models
		QString filename = QFileDialog::getSaveFileName(this->gui->parentWidget(), tr("Save Current Models"),
														QDir::currentPath(), tr("ConceptNetGui Models File (*.txt)"), 0,
														QFileDialog::DontUseNativeDialog);

		if (!filename.endsWith(".txt"))
		{
			filename += ".txt";
		}

		//Check if the user selected a correct file
		if (!filename.isNull())
		{

			//Create file
			QFile file(filename);

			if (!file.open(QIODevice::WriteOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

			QString models;
			if (this->gui->settingsDialog->isSaveSortedChecked())
			{
				models = this->gui->getUi()->sortedModelsLabel->text();
			}
			else
			{
				models = this->gui->getUi()->currentModelsLabel->text();
			}
			// Point a QTextStream object at the file
			QTextStream outStream(&file);

			// Write the line to the file
			outStream << models;

			// Close the file
			file.close();
		}
	}

	void SaveLoadHandler::loadProgram()
	{
		//Open load file dialog to select a pregenerated wumpus world
		QString filename = QFileDialog::getOpenFileName(this->gui->parentWidget(), tr("Load Program"), QDir::currentPath(),
														tr("ConceptNetGui Program File (*.cnlp)"), 0,
														QFileDialog::DontUseNativeDialog);

		//Check if the user selected a correct file
		if (!filename.isNull())
		{
			//Open file
			QFile file(filename);
			if (!file.open(QIODevice::ReadOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

			QByteArray loadedData = file.readAll();
			shared_ptr<LoadSavedProgramCommand> cmd = make_shared<LoadSavedProgramCommand>(this->gui, loadedData);
			cmd->execute();
		}
	}

	void SaveLoadHandler::loadBackgroundKnowledge()
	{
		//Open load file dialog to select a pregenerated wumpus world
		this->gui->getUi()->aspRuleTextArea->clear();

		QString filename = QFileDialog::getOpenFileName(this->gui->parentWidget(), tr("Load Background Knowledge"),
														QDir::currentPath(), tr("Logic Program (*.lp)"), 0,
														QFileDialog::DontUseNativeDialog);
		//Check if the user selected a correct file
		if (!filename.isNull())
		{
			shared_ptr<LoadBackgroundKnowledgeCommand> cmd = make_shared<LoadBackgroundKnowledgeCommand>(this->gui, filename);
			cmd->execute();
		}
	}

} /* namespace cng */
