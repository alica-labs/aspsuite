/*
 * AddCommand.h
 *
 *  Created on: Mar 9, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_ADDCOMMAND_H_
#define SRC_COMMANDS_ADDCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <QJsonObject>

namespace cng
{

	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to call the solvers Add method
	 */
	class AddCommand : public Command, public std::enable_shared_from_this<AddCommand>
	{
	Q_OBJECT
	public:
		AddCommand(ConceptNetGui* gui, QString program);
		virtual ~AddCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString program;
		QString programSection;
		std::string historyProgramSection;

	private:
		void extractProgramSection();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_ADDCOMMAND_H_ */
