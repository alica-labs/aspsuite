#include "commands/GroundCommand.h"
#include "handler/CommandHistoryHandler.h"

#include "gui/KnowledgebaseCreator.h"

#include <asp_solver/ASPSolver.h>

#include <QString>

#include <ui_knowledgebasecreator.h>

namespace kbcr
{

GroundCommand::GroundCommand(KnowledgebaseCreator* gui, QString program)
{
    this->type = "Ground";
    this->gui = gui;
    this->program = program;
    extractProgramSection();
}

GroundCommand::~GroundCommand() {}

void GroundCommand::execute()
{
    this->gui->chHandler->addToCommandHistory(shared_from_this());
    std::string aspString = this->program.toStdString();
    if (this->program.contains("\n") || !this->program.contains("#program")) {
        this->gui->getSolver()->add(this->programSection.toStdString().c_str(), {}, aspString.c_str());
    }
    if (this->programSection.contains("(") && this->programSection.contains(")")) {
        auto indexLeft = this->programSection.indexOf("(");
        auto indexRight = this->programSection.indexOf(")");
        auto tmp = this->programSection.mid(indexLeft + 1, indexRight - indexLeft - 1);
        this->programSection = this->programSection.left(indexLeft);
        auto symVec = Clingo::SymbolVector();
        auto paramList = tmp.split(",", QString::SplitBehavior::SkipEmptyParts);
        for (auto it : paramList) {
            symVec.push_back(this->gui->getSolver()->parseValue(it.toStdString().c_str()));
        }
        this->gui->getSolver()->ground({{this->programSection.toStdString().c_str(), symVec}}, nullptr);
    } else {
        this->gui->getSolver()->ground({{this->programSection.toStdString().c_str(), {}}}, nullptr);
    }
    emit this->gui->updateExternalList();
}

void GroundCommand::undo()
{
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
}

QJsonObject GroundCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = "Ground";
    ret["program"] = this->program;
    return ret;
}

void GroundCommand::extractProgramSection()
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
