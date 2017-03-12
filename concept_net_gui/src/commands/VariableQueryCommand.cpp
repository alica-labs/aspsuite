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
#include "gui/ModelSettingDialog.h"

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
		//Separate program into lines by given delimiter
		auto prgm = this->program.toStdString();
		std::string delimiter = "\n";
		size_t pos = 0;
		std::vector<std::string> lines;
		while ((pos = prgm.find(delimiter)) != std::string::npos)
		{
			lines.push_back(prgm.substr(0, pos));
			prgm.erase(0, pos + delimiter.length());
		}
		lines.push_back(prgm);
		//Define asp term
		auto term = make_shared<reasoner::ASPCommonsTerm>();
		term->setType(reasoner::ASPQueryType::Variable);
		int queryId = this->gui->getSolver()->getQueryCounter();
		term->setId(queryId);
		term->setQueryId(queryId);
		// Set number of shown models according to the gui
		if (this->gui->modelSettingsDialog->getNumberOfModels() != -1)
		{
			term->setNumberOfModels(to_string(this->gui->modelSettingsDialog->getNumberOfModels()));
		}
		for (int i = 0; i < lines.size(); i++)
		{
			// empty lines would result in a constraint for the query external statement
			if (lines.at(i).empty())
			{
				continue;
			}
			// comments aren't needed
			if (lines.at(i).find("%") != std::string::npos)
			{
				continue;
			}
			//query rule
			if (i == 0)
			{
				if (lines.at(i).find(":-") == std::string::npos)
				{
					cout << "VariableQueryCommand: malformed query rule! Aborting!" << endl;
					return;
				}
				term->setQueryRule(lines.at(i));
				this->toShow = QString(lines.at(i).c_str());
				continue;
			}
			// general rules
			if (lines.at(i).find(":-") != std::string::npos)
			{
				term->addRule(lines.at(i));
				continue;
			}
			//facts
			else if (lines.at(i).find(":-") == std::string::npos)
			{
				term->addFact(lines.at(i));
				continue;
			}
			else
			{
				cout << "VariableQueryCommand: string not recognized!" << endl;
			}
		}
		//Prepare getSolution
		std::vector<std::shared_ptr<reasoner::ASPCommonsVariable>> vars;
		vars.push_back(make_shared<reasoner::ASPCommonsVariable>());
		std::vector<std::shared_ptr<reasoner::ASPCommonsTerm>> terms;
		terms.push_back(term);
		std::vector<void*> result;
		//Call getSolution on solver
		this->gui->getSolver()->getSolution(vars, terms, result);
		std::vector<reasoner::AnnotatedValVec*> castedResults;
		//handle result if there is one
		if (result.size() > 0)
		{
			//result returned but can be empty
			castedResults.push_back((reasoner::AnnotatedValVec*)result.at(0));
			//Handle empty result
			if (castedResults.at(0)->variableQueryValues.size() == 0)
			{
				this->gui->getUi()->queryResultsLabel->setText(QString("No result found!"));
			}
			//handle proper result
			else
			{
				std::stringstream ss;
				ss << "Variable Query: " << term->getQueryRule() << std::endl;
				ss << "Result contains the predicates: " << std::endl;
				//Handle query answer
				for (int i = 0; i < castedResults.size(); i++)
				{
					for (int j = 0; j < castedResults.at(i)->variableQueryValues.size(); j++)
					{
						for (int k = 0; k < castedResults.at(i)->variableQueryValues.at(j).size(); k++)
						{
							ss << castedResults.at(i)->variableQueryValues.at(j).at(k) << " ";
						}
					}
				}
				ss << std::endl;
				//handle used models
				if (this->gui->modelSettingsDialog->isShowModelsInQuery())
				{
					ss << "The queried model contains the following predicates: " << std::endl;
					for (int i = 0; i < castedResults.at(0)->query->getCurrentModels()->at(0).size(); i++)
					{
						ss << castedResults.at(0)->query->getCurrentModels()->at(0).at(i) << " ";
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
		this->gui->getUi()->aspRuleTextArea->setText(this->program);
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

} /* namespace cng */
