/*
 * LoadLogicProgramCommand.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: stefan
 */

#include "commands/LoadBackgroundKnowledgeCommand.h"
#include <gui/KnowledgebaseCreator.h>

#include "handler/CommandHistoryHandler.h"

#include <ui_knowledgebasecreator.h>

namespace kbcr
{

LoadBackgroundKnowledgeCommand::LoadBackgroundKnowledgeCommand(KnowledgebaseCreator* gui, QString fileName)
{
    this->type = "Load Logic Program";
    this->gui = gui;
    this->fileName = fileName;
}

LoadBackgroundKnowledgeCommand::~LoadBackgroundKnowledgeCommand() {}

void LoadBackgroundKnowledgeCommand::execute()
{
    // Open file
    QFile file(this->fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return;
    }
    // Read data from file
    this->fileContent = file.readAll();
    file.close();
    // PRint data into gui
    this->gui->getUi()->aspRuleTextArea->setText(fileContent);
    this->gui->chHandler->addToCommandHistory(shared_from_this());
}

void LoadBackgroundKnowledgeCommand::undo()
{
    this->gui->getUi()->aspRuleTextArea->clear();
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
}

QJsonObject LoadBackgroundKnowledgeCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = "Load Logic Program";
    ret["fileName"] = this->fileName;
    return ret;
}

} /* namespace kbcr */
