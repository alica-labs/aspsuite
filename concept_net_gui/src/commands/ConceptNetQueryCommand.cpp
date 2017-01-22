/*
 * ConceptNetQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "../include/commands/ConceptNetQueryCommand.h"
#include "../include/gui/ConceptNetGui.h"

#include <ui_conceptnetgui.h>

namespace cng
{

	ConceptNetQueryCommand::ConceptNetQueryCommand(ConceptNetGui* gui)
	{
		this->type = "Concept Net";
		this->gui = gui;
	}

	ConceptNetQueryCommand::~ConceptNetQueryCommand()
	{
	}

	void ConceptNetQueryCommand::execute()
	{
		//TODO just for testing remove later
		this->gui->getUi()->currentModelsLabel->setText(QString("Testing save models using currentModelsLabel."));
		this->gui->getUi()->sortedModelsLabel->setText(QString("Testing save models using sortedModelsLabel."));
		this->gui->addToCommandHistory(shared_from_this());
	}

	void ConceptNetQueryCommand::undo()
	{
		this->gui->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject ConceptNetQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Concept Net";
		return ret;
	}

} /* namespace cng */
