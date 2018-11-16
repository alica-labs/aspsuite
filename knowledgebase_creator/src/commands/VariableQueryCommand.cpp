#include "commands/VariableQueryCommand.h"

#include "handler/CommandHistoryHandler.h"

#include "gui/ModelSettingDialog.h"
#include "gui/SettingsDialog.h"
#include "gui/KnowledgebaseCreator.h"

#include <ui_knowledgebasecreator.h>

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/AnnotatedValVec.h>

#include <asp_solver/ASPSolver.h>

#include <SystemConfig.h>

namespace kbcr
{

VariableQueryCommand::VariableQueryCommand(KnowledgebaseCreator* gui, QString program)
{
    this->type = "Variable Query";
    this->gui = gui;
    this->program = program;
    this->toShow = QString("");
}

VariableQueryCommand::~VariableQueryCommand() {}

void VariableQueryCommand::execute()
{
    // Separate program into lines by given delimiter
    auto prgm = this->program.toStdString();
    std::string delimiter = "\n";
    size_t pos = 0;
    std::vector<std::string> lines;
    while ((pos = prgm.find(delimiter)) != std::string::npos) {
        lines.push_back(prgm.substr(0, pos));
        prgm.erase(0, pos + delimiter.length());
    }
    lines.push_back(prgm);
    // Define asp term
    auto term = new reasoner::ASPCommonsTerm();
    term->setType(reasoner::ASPQueryType::Variable);
    int queryId = this->gui->getSolver()->getQueryCounter();
    term->setId(queryId);
    term->setQueryId(queryId);
    // Set number of shown models according to the gui
    if (this->gui->modelSettingsDialog->getNumberOfModels() != -1) {
        term->setNumberOfModels(to_string(this->gui->modelSettingsDialog->getNumberOfModels()));
    }
    for (int i = 0; i < lines.size(); i++) {
        // empty lines would result in a constraint for the query external statement
        if (lines.at(i).empty()) {
            continue;
        }
        // comments aren't needed
        if (lines.at(i).find("%") != std::string::npos) {
            continue;
        }
        // query rule
        if (i == 0) {
            if (lines.at(i).find(":-") == std::string::npos) {
                cout << "VariableQueryCommand: malformed query rule! Aborting!" << endl;
                return;
            }
            term->setQueryRule(lines.at(i));
            this->toShow = QString(lines.at(i).c_str());
            continue;
        }
        // general rules
        if (lines.at(i).find(":-") != std::string::npos) {
            term->addRule(lines.at(i));
            continue;
        }
        // facts
        else if (lines.at(i).find(":-") == std::string::npos) {
            term->addFact(lines.at(i));
            continue;
        } else {
            cout << "VariableQueryCommand: string not recognized!" << endl;
        }
    }
    // Prepare getSolution
    std::vector<reasoner::ASPCommonsVariable*> vars;
    vars.push_back(new reasoner::ASPCommonsVariable());
    std::vector<reasoner::ASPCommonsTerm*> terms;
    terms.push_back(term);
    std::vector<reasoner::AnnotatedValVec*> result;
    // Call getSolution on solver
    this->gui->getSolver()->getSolution(vars, terms, result);
    // handle result if there is one
    if (result.size() > 0) {
        // Handle empty result
        if (result.at(0)->variableQueryValues.size() == 0) {
            this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
        }
        // handle proper result
        else {
            std::stringstream ss;
            ss << "Variable Query: " << term->getQueryRule() << std::endl;
            ss << "Result contains the predicates: " << std::endl;
            // Handle query answer
            for (int i = 0; i < result.size(); i++) {
                for (int j = 0; j < result.at(i)->variableQueryValues.size(); j++) {
                    for (int k = 0; k < result.at(i)->variableQueryValues.at(j).size(); k++) {
                        ss << result.at(i)->variableQueryValues.at(j).at(k) << " ";
                    }
                }
            }
            ss << std::endl;
            // handle used models
            if (this->gui->modelSettingsDialog->isShowModelsInQuery()) {
                ss << "The queried model contains the following predicates: " << std::endl;
                for (int i = 0; i < result.at(0)->query->getCurrentModels()->at(0).size(); i++) {
                    ss << result.at(0)->query->getCurrentModels()->at(0).at(i) << " ";
                }
                ss << std::endl;
            }
            this->gui->getUi()->queryResultsLabel->setText(QString(ss.str().c_str()));
        }
    } else {
        this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
    }
    this->gui->chHandler->addToCommandHistory(shared_from_this());
    this->gui->getUi()->aspRuleTextArea->setText(this->program);

    // clean up memory
    for (auto valVec : result) {
        delete valVec;
    }
    for (auto var : vars) {
        delete var;
    }
    for (auto term : terms) {
        delete term;
    }
}

void VariableQueryCommand::undo()
{
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
    this->gui->getUi()->queryResultsLabel->clear();
    this->gui->getUi()->aspRuleTextArea->clear();
}

QJsonObject VariableQueryCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = "Variable Query";
    ret["program"] = this->program;
    return ret;
}

} /* namespace kbcr */
