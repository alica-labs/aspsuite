/*
 * VariableQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/VariableQueryCommand.h"
#include "handler/CommandHistoryHandler.h"
#include "gui/ConceptNetGui.h"
#include "gui/SettingsDialog.h"
#include <ui_conceptnetgui.h>

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/AnnotatedValVec.h>
#include <asp_commons/ASPQuery.h>
#include <asp_solver/ASPSolver.h>

#include <SystemConfig.h>

namespace cng
{

	VariableQueryCommand::VariableQueryCommand(ConceptNetGui* gui, QString program)
	{
		this->type = "Variable Query";
		this->gui = gui;
		this->program = program;
		this->toShow = QString("");
	}

	VariableQueryCommand::~VariableQueryCommand()
	{
	}

	void VariableQueryCommand::execute()
	{
		auto prgm = this->program.toStdString();
		std::string delimiter = "\n";

		size_t pos = 0;
		string token;
		vector<string> lines;
		while ((pos = prgm.find(delimiter)) != string::npos)
		{
			token = prgm.substr(0, pos);
			lines.push_back(token);
			prgm.erase(0, pos + delimiter.length());
		}
		lines.push_back(prgm);
		auto term = make_shared<reasoner::ASPCommonsTerm>();
		term->setType(reasoner::ASPQueryType::Variable);
		int queryId = this->gui->getSolver()->getQueryCounter();
		term->setId(queryId);
		term->setQueryId(queryId);
		if (this->gui->getUi()->numberOfModelsSpinBox->value() != -1)
		{
			term->setNumberOfModels(to_string(this->gui->getUi()->numberOfModelsSpinBox->value()));
		}
		for (int i = 0; i < lines.size(); i++)
		{
			if (i == 0)
			{
				if (lines.at(i).find(":-") == string::npos)
				{
					cout << "VariableQueryCommand: malformed query rule! Aborting!" << endl;
					return;
				}
				term->setQueryRule(lines.at(i));
				this->toShow = QString(lines.at(i).c_str());
				continue;
			}
			if (lines.at(i).find(":-") != string::npos)
			{
				term->addRule(lines.at(i));
				continue;
			}
			else if (lines.at(i).find(":-") == string::npos)
			{
				term->addFact(lines.at(i));
				continue;
			}
			else
			{
				cout << "VariableQueryCommand: string not recognized!" << endl;
			}
		}
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
				ss << "Variable Query: " << term->getQueryRule() << endl;
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

	void VariableQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
		this->gui->getUi()->queryResultsLabel->clear();
	}

	QJsonObject VariableQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Variable Query";
		ret["program"] = this->program;
		return ret;
	}

} /* namespace cng */
