#include <gui/KnowledgebaseCreator.h>
#include "gui/SettingsDialog.h"

#include "handler/CommandHistoryHandler.h"
#include "handler/SaveLoadHandler.h"

#include "containers/SolverSettings.h"

#include "commands/NewSolverCommand.h"
#include "commands/ChangeSolverSettingsCommand.h"
#include "commands/ConceptNetQueryCommand.h"
#include "commands/FactsQueryCommand.h"
#include "commands/GroundCommand.h"
#include "commands/LoadBackgroundKnowledgeCommand.h"
#include "commands/LoadSavedProgramCommand.h"
#include "commands/SolveCommand.h"
#include "commands/VariableQueryCommand.h"
#include "commands/AddCommand.h"
#include "commands/AssignExternalCommand.h"

namespace kbcr {

    LoadSavedProgramCommand::LoadSavedProgramCommand(KnowledgebaseCreator *gui, QString fileName,
                                                     QByteArray loadedData) {
        this->type = "Load Saved Program";
        this->gui = gui;
        this->loadedData = loadedData;
        this->fileName = fileName;
        this->outFS = std::ofstream("outfile.txt", std::ios::app);
        std::cout << "load created" << std::endl;
    }

    LoadSavedProgramCommand::~LoadSavedProgramCommand() {
    }

    void LoadSavedProgramCommand::execute() {
        this->gui->commandHistory.clear();
        this->gui->chHandler->addToCommandHistory(shared_from_this());
        //Parse loaded data to JSON
        QJsonDocument loadDoc(QJsonDocument::fromJson(this->loadedData));
        QJsonObject savedObject = loadDoc.object();
        //Handle all commands
        std::cout << "Start loading" << std::endl;

        QJsonArray cmds = savedObject["commandHistory"].toArray();

        for (int j = 0; j < 10000; j++) {
            std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < cmds.size(); i++) {
                QJsonObject cmd = cmds[i].toObject();
                // Show progress to user
//			std::cout << "LoadSavedProgramCommand: " << cmd["type"].toString().toStdString() << " " << i + 1 << "/"
//					<< cmds.size() << std::endl;

                //Handle new Solver command
                if (cmd["type"].toString().toStdString().compare("New Solver") == 0) {
                    std::shared_ptr<NewSolverCommand> c = std::make_shared<NewSolverCommand>(
                            this->gui,
                            std::make_shared<SolverSettings>(cmd["name"].toString().toStdString(),
                                                             cmd["settingsString"].toString().toStdString()));
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle change settings command
                else if (cmd["type"].toString().toStdString().compare("Change Settings") == 0) {
                    std::shared_ptr<ChangeSolverSettingsCommand> c = std::make_shared<ChangeSolverSettingsCommand>(
                            this->gui,
                            this->gui->settingsDialog,
                            std::make_shared<SolverSettings>(cmd["name"].toString().toStdString(),
                                                             cmd["settingsString"].toString().toStdString()));
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle concept net command
                else if (cmd["type"].toString().toStdString().compare("Concept Net") == 0) {
                    std::shared_ptr<ConceptNetQueryCommand> c = std::make_shared<ConceptNetQueryCommand>(
                            this->gui, cmd["queryString"].toString());
                    c->execute();
                    QEventLoop loop;
                    this->connect(this, SIGNAL(cn5CallFinished()), &loop, SLOT(quit()));
                    loop.exec();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle facts query command
                else if (cmd["type"].toString().toStdString().compare("Facts Query") == 0) {
                    std::shared_ptr<FactsQueryCommand> c = std::make_shared<FactsQueryCommand>(
                            this->gui, cmd["factsString"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle ground command
                else if (cmd["type"].toString().toStdString().compare("Ground") == 0) {
                    std::shared_ptr<GroundCommand> c = std::make_shared<GroundCommand>(this->gui,
                                                                                       cmd["program"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle load background knowledge command
                else if (cmd["type"].toString().toStdString().compare("Load Logic Program") == 0) {
                    std::shared_ptr<LoadBackgroundKnowledgeCommand> c = std::make_shared<LoadBackgroundKnowledgeCommand>(
                            this->gui, cmd["fileName"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle load program command not used
                else if (cmd["type"].toString().toStdString().compare("Load Saved Program") == 0) {
                    /**
                     * This command should never be part of a saved program,
                     * since it will result in a recursive load call!
                     */
                    continue;
                }
                    //Handle Variable query command
                else if (cmd["type"].toString().toStdString().compare("Variable Query") == 0) {
                    std::shared_ptr<VariableQueryCommand> c = std::make_shared<VariableQueryCommand>(
                            this->gui, cmd["program"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle solve command
                else if (cmd["type"].toString().toStdString().compare("Solve") == 0) {
                    std::shared_ptr<SolveCommand> c = std::make_shared<SolveCommand>(this->gui);
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle add command
                else if (cmd["type"].toString().toStdString().compare("Add") == 0) {
                    std::shared_ptr<AddCommand> c = std::make_shared<AddCommand>(this->gui, cmd["program"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle assignExternal command
                else if (cmd["type"].toString().toStdString().compare("Assign External") == 0) {
                    std::shared_ptr<AssignExternalCommand> c = std::make_shared<AssignExternalCommand>(
                            this->gui, cmd["external"].toString(), cmd["truthValue"].toString());
                    c->execute();
                    emit this->gui->updateCommandList();
                    continue;
                }
                    //Handle unknown command
                else {
//				std::cout << "LoadSavedProgramCommand: Command with unknown type found!" << std::endl;
                }

            }
            std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
            outFS << "Measured Time: \t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
        }
        //outFS.close();
//		std::cout << "LoadSavedProgramCommand: Loading finished!" << std::endl;
        this->gui->slHandler->currentLoadCmd = nullptr;
    }

    void LoadSavedProgramCommand::undo() {
        this->gui->chHandler->removeFromCommandHistory(shared_from_this());
    }

    QJsonObject LoadSavedProgramCommand::toJSON() {
        QJsonObject ret;
        ret["type"] = "Load Saved Program";
        ret["fileName"] = this->fileName;
        return ret;
    }

} /* namespace kbcr */
