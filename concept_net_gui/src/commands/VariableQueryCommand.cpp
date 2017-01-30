/*
 * VariableQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/VariableQueryCommand.h"
#include "handler/CommandHistoryHandler.h"
#include "gui/ConceptNetGui.h"
#include <ui_conceptnetgui.h>

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_solver/ASPSolver.h>

#include <SystemConfig.h>

namespace cng
{

	VariableQueryCommand::VariableQueryCommand(ConceptNetGui* gui)
	{
		this->type = "Variable Query";
		this->gui = gui;
		this->program = program;
	}

	VariableQueryCommand::~VariableQueryCommand()
	{
	}

	void VariableQueryCommand::execute()
	{
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		this->program = this->gui->getUi()->aspRuleTextArea->toPlainText();
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
		for (auto line : lines)
		{
			if(line.find("#query") != string::npos)
			{
				continue;
			}
			else if(line.find("#lifeTime") != string::npos)
			{
				line.erase(0, string("#lifeTime").length());
				line.erase(line.find("."));
				line = supplementary::Configuration::trim(line);
				term->setLifeTime(stoi(line));
				continue;
			}
			else if(line.find(":-") != string::npos)
			{
				term->addRule(line);
				continue;
			}
			else if(line.find(":-") == string::npos)
			{
				term->addFact(line);
				continue;
			}
			else
			{
				cout << "VariableQuery: string not recognized!" << endl;
			}
		}
		//TODO call getSolution
	}

	void VariableQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject VariableQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Variable Query";
		ret["program"] = this->program;
		return ret;
	}

} /* namespace cng */
