/*
 * SolveCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/SolveCommand.h"

#include "gui/ModelSettingDialog.h"

#include <ui_knowledgebasecreator.h>

#include "handler/CommandHistoryHandler.h"

#include <asp_solver/ASPSolver.h>
#include <gui/KnowledgebaseCreator.h>

namespace kbcr
{

	SolveCommand::SolveCommand(KnowledgebaseCreator* gui)
	{
		this->type = "Solve";
		this->gui = gui;
		this->satisfiable = false;
	}

	SolveCommand::~SolveCommand()
	{
	}

	void SolveCommand::printModels()
	{
		stringstream ss;
		for (int i = 0; i < this->currentModels.size(); i++)
		{
			ss << "Model number " << i + 1 << ":" << endl;
			for (auto atom : this->currentModels.at(i))
			{
				ss << atom << " ";
			}
			ss << endl;
		}
		this->gui->getUi()->currentModelsLabel->setText(QString(ss.str().c_str()));
	}

	void SolveCommand::execute()
	{
		if (this->gui->modelSettingsDialog->getNumberOfModels() != -1)
		{
			auto conf = &this->gui->getSolver()->clingo->getConf();
			auto root = conf->getRootKey();
			auto modelsKey = conf->getSubKey(root, "solve.models");
			conf->setKeyValue(modelsKey, std::to_string(this->gui->modelSettingsDialog->getNumberOfModels()).c_str());
		}
		this->satisfiable = this->gui->getSolver()->solve();
		this->currentModels = this->gui->getSolver()->getCurrentModels();
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		printModels();
		printSortedModels();
	}

	void SolveCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject SolveCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Solve";
		return ret;
	}

	vector<Gringo::SymVec> SolveCommand::getCurrentModels()
	{
		return this->currentModels;
	}

	bool SolveCommand::isSatisfiable()
	{
		return this->satisfiable;
	}

	void SolveCommand::printSortedModels()
	{
		std::stringstream ss;
		vector<vector<string>> sorted = std::vector<std::vector<std::string>>(this->currentModels.size());
		for (int i = 0; i < this->currentModels.size(); i++)
		{
			for (auto atom : this->currentModels.at(i))
			{
				ss << atom;
				sorted.at(i).push_back(ss.str());
				ss.str("");
			}
			std::sort(sorted.at(i).begin(), sorted.at(i).end());
		}
		ss.str("");
		for (int i = 0; i < sorted.size(); i++)
		{
			ss << "Model number " << i + 1 << ":" << std::endl;
			for (auto atom : sorted.at(i))
			{
				ss << atom << " ";
			}
			ss << std::endl;
		}
		this->gui->getUi()->sortedModelsLabel->setText(QString(ss.str().c_str()));
	}

} /* namespace kbcr */
