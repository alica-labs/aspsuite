#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <vector>
#include <memory>

namespace kbcr
{

	class SolverSettings;
	class KnowledgebaseCreator;
	/**
	 * Class inheriting from Command interface used to create a new solver
	 */
	class NewSolverCommand : public Command, public std::enable_shared_from_this<NewSolverCommand>
	{
		Q_OBJECT
	public:
		NewSolverCommand(KnowledgebaseCreator* gui, std::shared_ptr<SolverSettings> settings);
		virtual ~NewSolverCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		std::shared_ptr<SolverSettings> settings;
		KnowledgebaseCreator* gui;

	private:
		/**
		 * Gets default setting from settings dialog
		 */
		void getDefaultArguments();
	};

} /* namespace kbcr */

