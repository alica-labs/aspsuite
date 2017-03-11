/*
 * AssignExternalCommand.h
 *
 *  Created on: Mar 11, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_
#define INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <QJsonObject>

namespace cng
{

	class ConceptNetGui;
	class AssignExternalCommand : public Command, public std::enable_shared_from_this<AssignExternalCommand>
	{
	public:
		AssignExternalCommand(ConceptNetGui* gui, QString externalName, QString truthValue);
		virtual ~AssignExternalCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString externalName;
		QString truthValue;
		QString historyString;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_ */
