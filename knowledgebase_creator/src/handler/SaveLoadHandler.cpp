#include "handler/SaveLoadHandler.h"
#include "gui/KnowledgebaseCreator.h"

#include "gui/ModelSettingDialog.h"
#include "gui/SettingsDialog.h"

#include "commands/AddCommand.h"
#include "commands/AssignExternalCommand.h"
#include "commands/ChangeSolverSettingsCommand.h"
#include "commands/Command.h"
#include "commands/ConceptNetQueryCommand.h"
#include "commands/FactsQueryCommand.h"
#include "commands/GroundCommand.h"
#include "commands/LoadBackgroundKnowledgeCommand.h"
#include "commands/LoadSavedProgramCommand.h"
#include "commands/NewSolverCommand.h"
#include "commands/SolveCommand.h"
#include "commands/VariableQueryCommand.h"

#include <ui_knowledgebasecreator.h>
#include <ui_settingsdialog.h>

#include <QFileDialog>
#include <QString>

namespace kbcr
{

SaveLoadHandler::SaveLoadHandler(KnowledgebaseCreator* gui)
{
    this->gui = gui;
    this->currentLoadCmd = nullptr;
}

SaveLoadHandler::~SaveLoadHandler() {}

void SaveLoadHandler::saveProgram()
{
    // Open save file dialog to save a program
    QString filename = QFileDialog::getSaveFileName(this->gui->parentWidget(), tr("Save Programm"), QDir::currentPath(),
            tr("KnowledgebaseCreator Program File (*.cnlp)"), 0, QFileDialog::DontUseNativeDialog);

    if (!filename.endsWith(".cnlp")) {
        filename += ".cnlp";
    }

    // Check if the user selected a correct file
    if (!filename.isNull()) {

        // Create file
        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open file.");
            return;
        }

        QJsonObject history;

        // JSON Array to hold the commandHistory
        QJsonArray jsonCommandHistory;
        for (auto cmd : this->gui->commandHistory) {
            // Skip load saved commandHistory to avoid recursive loading
            if (cmd->getType().compare("Load Saved Program") == 0) {
                continue;
            }
            jsonCommandHistory.append(cmd->toJSON());
        }
        history["commandHistory"] = jsonCommandHistory;
        // Write to file
        QJsonDocument saveDoc(history);
        file.write(saveDoc.toJson());
    }
}

void SaveLoadHandler::saveModels()
{
    // Open save file dialog to save models
    QString filename = QFileDialog::getSaveFileName(this->gui->parentWidget(), tr("Save Current Models"), QDir::currentPath(),
            tr("KnowledgebaseCreator Models File (*.txt)"), 0, QFileDialog::DontUseNativeDialog);

    if (!filename.endsWith(".txt")) {
        filename += ".txt";
    }

    // Check if the user selected a correct file
    if (!filename.isNull()) {

        // Create file
        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open file.");
            return;
        }

        QString models;
        if (this->gui->modelSettingsDialog->isSaveSortedChecked()) {
            models = this->gui->getUi()->sortedModelsLabel->text();
        } else {
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
    // Open load file dialog to select a pregenerated wumpus world
    QString filename = QFileDialog::getOpenFileName(this->gui->parentWidget(), tr("Load Program"), QDir::currentPath(),
            tr("KnowledgebaseCreator Program File (*.cnlp)"), 0, QFileDialog::DontUseNativeDialog);

    // Check if the user selected a correct file
    if (!filename.isNull()) {
        // Open file
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open file.");
            return;
        }

        QByteArray loadedData = file.readAll();
        std::shared_ptr<LoadSavedProgramCommand> cmd = std::make_shared<LoadSavedProgramCommand>(this->gui, filename, loadedData);
        this->currentLoadCmd = cmd;
        cmd->execute();
    }
}

void SaveLoadHandler::loadBackgroundKnowledge()
{
    // Open load file dialog to select a pregenerated wumpus world
    this->gui->getUi()->aspRuleTextArea->clear();

    QString filename = QFileDialog::getOpenFileName(
            this->gui->parentWidget(), tr("Load Background Knowledge"), QDir::currentPath(), tr("Logic Program (*.lp)"), 0, QFileDialog::DontUseNativeDialog);
    // Check if the user selected a correct file
    if (!filename.isNull()) {
        std::shared_ptr<LoadBackgroundKnowledgeCommand> cmd = std::make_shared<LoadBackgroundKnowledgeCommand>(this->gui, filename);
        cmd->execute();
    }
}

} /* namespace kbcr */
