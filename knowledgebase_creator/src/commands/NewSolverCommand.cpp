#include "commands/NewSolverCommand.h"

#include "gui/SettingsDialog.h"

#include "containers/SolverSettings.h"

#include "handler/CommandHistoryHandler.h"

#include "gui/KnowledgebaseCreator.h"

#include <iostream>

#include <reasoner/asp/Solver.h>

namespace kbcr
{

NewSolverCommand::NewSolverCommand(KnowledgebaseCreator* gui, std::shared_ptr<SolverSettings> settings)
{
    this->type = "New Solver";
    this->settings = settings;
    this->gui = gui;
    if (settings == nullptr) {
        std::cout << "Using default params" << std::endl;
        getDefaultArguments();
    }
}

NewSolverCommand::~NewSolverCommand() {}

void NewSolverCommand::execute()
{
    this->gui->clear();
    this->gui->chHandler->addToCommandHistory(shared_from_this());
    this->gui->setSettings(this->settings);
    // vorher this->settings->args statt {}
    this->gui->setSolver(new reasoner::asp::Solver({}));
    this->gui->enableGui(true);
}

void NewSolverCommand::undo()
{
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
    this->gui->enableGui(false);
    this->gui->clear();
    delete this->gui->getSolver();
    this->gui->setSolver(nullptr);
}

QJsonObject NewSolverCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = "New Solver";
    ret["settingsString"] = QString(this->settings->argString.c_str());
    ret["name"] = QString(this->settings->name.c_str());
    return ret;
}

void NewSolverCommand::getDefaultArguments()
{
    this->settings = this->gui->settingsDialog->getDefaultSettings();
}

} /* namespace kbcr */
