#include "commands/FilterQueryCommand.h"

#include <ui_knowledgebasecreator.h>

#include "gui/ModelSettingDialog.h"

#include "handler/CommandHistoryHandler.h"
#include "gui/KnowledgebaseCreator.h"


#include <reasoner/asp/Term.h>
#include <reasoner/asp/Variable.h>
#include <reasoner/asp/Query.h>
#include <reasoner/asp/AnnotatedValVec.h>
#include <reasoner/asp/FilterQuery.h>
#include <reasoner/asp/Solver.h>

namespace kbcr
{

FilterQueryCommand::FilterQueryCommand(KnowledgebaseCreator* gui, QString factsString)
{
    this->type = "Filter Query";
    this->gui = gui;
    this->factsString = factsString;
}

FilterQueryCommand::~FilterQueryCommand() {}

void FilterQueryCommand::execute()
{
    auto prgm = this->factsString.trimmed();
    // handle Wrong input
    if (prgm.contains("\n")) {
        std::cout << "FilterQueryCommand: A facts query only contains one set of facts separated by commata." << std::endl;
        return;
    }
    // create ASP term
    auto term = new reasoner::asp::Term();
    term->setType(reasoner::asp::QueryType::Filter);
    int queryId = this->gui->getSolver()->getQueryCounter();
    term->setId(queryId);
    term->setQueryId(queryId);
    // get number of models from gui
    if (this->gui->modelSettingsDialog->getNumberOfModels() != -1) {
        term->setNumberOfModels(std::to_string(this->gui->modelSettingsDialog->getNumberOfModels()));
    }
    prgm = prgm.left(prgm.size() - 1);
    term->setQueryRule(prgm.toStdString());
    // prepare get solution
    std::vector<reasoner::asp::Variable*> vars;
    vars.push_back(new reasoner::asp::Variable());
    std::vector<reasoner::asp::Term*> terms;
    terms.push_back(term);
    std::vector<reasoner::asp::AnnotatedValVec*> result;
    // call get solution from solver
    this->gui->getSolver()->getSolution(vars, terms, result);
    // handle result if there is one
    if (result.size() > 0) {

        // empty result
        if (result.at(0)->factQueryValues.size() == 0) {
            this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
        } else {
            // print result
            std::stringstream ss;
            ss << "Facts Query: " << term->getQueryRule() << std::endl;
            ss << "Result contains the predicates: " << std::endl;
            for (int i = 0; i < result.size(); i++) {
                for (int j = 0; j < result.at(i)->factQueryValues.size(); j++) {
                    for (int k = 0; k < result.at(i)->factQueryValues.at(j).size(); k++) {
                        ss << result.at(i)->factQueryValues.at(j).at(k) << " ";
                    }
                }
            }
            ss << std::endl;
            // print models
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
    this->gui->getUi()->aspRuleTextArea->setText(this->factsString);

    // Clean Up memory
    for (auto valVec : result) {
        delete valVec;
    }
    for (auto term : terms) {
        delete term;
    }

    for (auto var : vars) {
        delete var;
    }
}

void FilterQueryCommand::undo()
{
    this->gui->chHandler->removeFromCommandHistory(shared_from_this());
    this->gui->getUi()->queryResultsLabel->clear();
    this->gui->getUi()->aspRuleTextArea->clear();
}

QJsonObject FilterQueryCommand::toJSON()
{
    QJsonObject ret;
    ret["type"] = QString(this->type.c_str());
    ret["factsString"] = this->factsString;
    return ret;
}

} /* namespace kbcr */
