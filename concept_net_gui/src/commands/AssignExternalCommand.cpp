/*
 * AssignExternalCommand.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: stefan
 */

#include <commands/AssignExternalCommand.h>

namespace cng
{

	AssignExternalCommand::AssignExternalCommand(ConceptNetGui* gui, QString externalName, QString truthValue)
	{
		this->type = "Assign External";
		this->gui = gui;
		this->externalName = externalName;

	}

	AssignExternalCommand::~AssignExternalCommand()
	{
		// TODO Auto-generated destructor stub
	}

	void AssignExternalCommand::execute()
	{
		//TODO
	}

	void AssignExternalCommand::undo()
	{
		//TODO
	}

	QJsonObject AssignExternalCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Assign External";
		ret["external"] = this->externalName;
		ret["truthValue"] = this->truthValue;
		return ret;
	}

} /* namespace cng */
