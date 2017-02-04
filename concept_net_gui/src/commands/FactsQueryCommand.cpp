/*
 * FactsQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/FactsQueryCommand.h"
#include "gui/ConceptNetGui.h"

#include <ui_conceptnetgui.h>

#include "handler/CommandHistoryHandler.h"

#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/AnnotatedValVec.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_solver/ASPSolver.h>
#include <Configuration.h>

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
		auto prgm = this->factsString.toStdString();
		std::string delimiter = "\n";

		size_t pos = 0;
		string token;
		if((pos = prgm.find(delimiter)) != string::npos)
		{
			cout << "FactsQueryCommand: A facts query only contains one set of facts separated by commata." << endl;
			return;
		}
		auto term = make_shared<reasoner::ASPCommonsTerm>();
		term->setType(reasoner::ASPQueryType::Facts);
		int queryId = this->gui->getSolver()->getQueryCounter();
		term->setId(queryId);
		term->setQueryId(queryId);
		if (this->gui->getUi()->numberOfModelsSpinBox->value() != -1)
		{
			term->setNumberOfModels(to_string(this->gui->getUi()->numberOfModelsSpinBox->value()));
		}
		prgm = supplementary::Configuration::trim(prgm);
		prgm = prgm.substr(0, prgm.size() - 1);
		term->setQueryRule(prgm);
		vector<shared_ptr<reasoner::ASPCommonsVariable>> vars;
		vars.push_back(make_shared<reasoner::ASPCommonsVariable>());
		vector<shared_ptr<reasoner::ASPCommonsTerm>> terms;
		terms.push_back(term);
		vector<void*> result;
		this->gui->getSolver()->getSolution(vars, terms, result);
		vector<reasoner::AnnotatedValVec> castedResults;
		if (result.size() > 0)
		{
			castedResults.push_back(*((reasoner::AnnotatedValVec*)result.at(0)));
			if (castedResults.at(0).values.size() == 0)
			{
				this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
			}
			else
			{
				stringstream ss;
				ss << "Facts Query: " << term->getQueryRule() << endl;
				ss << "Result contains the predicates: " << endl;
				ss << "\t";
				for (int i = 0; i < castedResults.size(); i++)
				{
					for (int j = 0; j < castedResults.at(i).values.size(); j++)
					{
						for (int k = 0; k < castedResults.at(i).values.at(j).size(); k++)
						{
							ss << castedResults.at(i).values.at(j).at(k) << " ";
						}
					}
				}
				ss << endl;
				ss << "The queried model contains the following predicates: " << endl;
				ss << "\t";
				for (int i = 0; i < castedResults.at(0).query->getCurrentModels()->at(0).size(); i++)
				{
					ss << castedResults.at(0).query->getCurrentModels()->at(0).at(i) << " ";
				}
				ss << endl;
				this->gui->getUi()->queryResultsLabel->setText(QString(ss.str().c_str()));
			}
		}
		else
		{
			this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
		}
		this->gui->chHandler->addToCommandHistory(shared_from_this());
	}

	void FactsQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		this->gui->getUi()->queryResultsLabel->clear();
	}

	QJsonObject FactsQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Facts Query";
		ret["factsString"] = this->factsString;
		return ret;
	}

} /* namespace cng */
