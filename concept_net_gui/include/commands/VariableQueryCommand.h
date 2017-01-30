/*
 * VariableQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_
#define INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <string>

#include <QJsonObject>

using namespace std;

namespace cng
{

	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to create a VariableQuery
	 */
	class VariableQueryCommand : public Command, public enable_shared_from_this<VariableQueryCommand>
	{
	public:
		VariableQueryCommand(ConceptNetGui* gui);
		virtual ~VariableQueryCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString program;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_ */
