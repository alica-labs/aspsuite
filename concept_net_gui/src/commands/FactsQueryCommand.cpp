/*
 * FactsQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/FactsQueryCommand.h"

#include <ui_conceptnetgui.h>

#include "gui/ConceptNetGui.h"
#include "gui/ModelSettingDialog.h"

#include "handler/CommandHistoryHandler.h"

#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/AnnotatedValVec.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_solver/ASPSolver.h>

namespace cng
{

	FactsQueryCommand::FactsQueryCommand(ConceptNetGui* gui, QString factsString)
	{
		this->type = "Facts Query";
		this->gui = gui;
		this->factsString = factsString;

	}

	FactsQueryCommand::~FactsQueryCommand()
	{
	}

	void FactsQueryCommand::execute()
	{
		auto prgm = this->factsString.trimmed();
		//handle Wrong input
		if (prgm.contains("\n"))
		{
			std::cout << "FactsQueryCommand: A facts query only contains one set of facts separated by commata." << std::endl;
			return;
		}
		//create ASP term
		auto term = make_shared<reasoner::ASPCommonsTerm>();
		term->setType(reasoner::ASPQueryType::Facts);
		int queryId = this->gui->getSolver()->getQueryCounter();
		term->setId(queryId);
		term->setQueryId(queryId);
		//get number of models from gui
		if (this->gui->modelSettingsDialog->getNumberOfModels() != -1)
		{
			term->setNumberOfModels(to_string(this->gui->modelSettingsDialog->getNumberOfModels()));
		}
		prgm = prgm.left(prgm.size() - 1);
		term->setQueryRule(prgm.toStdString());
		//prepare get solution
		std::vector<std::shared_ptr<reasoner::ASPCommonsVariable>> vars;
		vars.push_back(make_shared<reasoner::ASPCommonsVariable>());
		std::vector<std::shared_ptr<reasoner::ASPCommonsTerm>> terms;
		terms.push_back(term);
		std::vector<void*> result;
		//call get solution from solver
		this->gui->getSolver()->getSolution(vars, terms, result);
		std::vector<reasoner::AnnotatedValVec> castedResults;
		//handle result if there is one
		if (result.size() > 0)
		{
			castedResults.push_back(*((reasoner::AnnotatedValVec*)result.at(0)));
			//empty result
			if (castedResults.at(0).factQueryValues.size() == 0)
			{
				this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
			}
			else
			{
				//print result
				std::stringstream ss;
				ss << "Facts Query: " << term->getQueryRule() << std::endl;
				ss << "Result contains the predicates: " << std::endl;
				for (int i = 0; i < castedResults.size(); i++)
				{
					for (int j = 0; j < castedResults.at(i).factQueryValues.size(); j++)
					{
						for (int k = 0; k < castedResults.at(i).factQueryValues.at(j).size(); k++)
						{
							ss << castedResults.at(i).factQueryValues.at(j).at(k) << " ";
						}
					}
				}
				ss << std::endl;
				//print models
				if (this->gui->modelSettingsDialog->isShowModelsInQuery())
				{
					ss << "The queried model contains the following predicates: " << std::endl;
					for (int i = 0; i < castedResults.at(0).query->getCurrentModels()->at(0).size(); i++)
					{
						ss << castedResults.at(0).query->getCurrentModels()->at(0).at(i) << " ";
					}
					ss << std::endl;
				}
				this->gui->getUi()->queryResultsLabel->setText(QString(ss.str().c_str()));
			}
		}
		else
		{
			this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
		}
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		this->gui->getUi()->aspRuleTextArea->setText(this->factsString);
	}

	void FactsQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		this->gui->getUi()->queryResultsLabel->clear();
		this->gui->getUi()->aspRuleTextArea->clear();
	}

	QJsonObject FactsQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Facts Query";
		ret["factsString"] = this->factsString;
		return ret;
	}

} /* namespace cng */
