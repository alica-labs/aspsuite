/*
 * AddCommand.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: stefan
 */

#include "commands/AddCommand.h"

#include "handler/CommandHistoryHandler.h"

#include <asp_solver/ASPSolver.h>
#include <gui/KnowledgebaseCreator.h>

#include <ui_knowledgebasecreator.h>

namespace kbcr
{

AddCommand::AddCommand(KnowledgebaseCreator* gui, QString program)
{
    this->type = "Add";
    this->gui = gui;
    this->program = program;
    extractProgramSection();
}

AddCommand::~AddCommand() {}

void AddCommand::execute()
{
    std::string aspString = this->program.toStdString();
    // call add program on solver
    this->gui->getSolver()->add(this->programSection.toStdString().c_str(), {}, aspString.c_str());
    // show program string
    this->gui->getUi()->programLabel->setText(this->gui->getUi()->programLabel->text().append("\n").append(this->program));
    this->gui->chHandler->addToCommandHistory(shared_from_this());
}

void AddCommand::undo()
{
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
}

QJsonObject AddCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = "Add";
    ret["program"] = this->program;
    return ret;
}

void AddCommand::extractProgramSection()
{
    size_t prefixLength = std::string("#program").length();
    std::string tmp = this->program.toStdString();
    size_t start = tmp.find("#program");
    if (start != std::string::npos) {
        size_t end = tmp.find_first_of(".");
        tmp = tmp.substr(start, end - start);
        this->historyProgramSection = tmp;
        this->programSection = QString(tmp.substr(prefixLength, end - prefixLength).c_str()).trimmed();
    }
}

} /* namespace kbcr */
