/*
 * ConceptNetQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_
#define SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <QJsonObject>

namespace cng
{

	class ConceptNetGui;
	class ConceptNetQueryCommand : public Command, public enable_shared_from_this<ConceptNetQueryCommand>
	{
	public:
		ConceptNetQueryCommand(ConceptNetGui* gui);
		virtual ~ConceptNetQueryCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_ */
