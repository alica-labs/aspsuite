#include <commands/Command.h>

#include <clingo.hh>

#include <QJsonObject>
#include <memory>

namespace kbcr
{

	class KnowledgebaseCreator;
	/**
	 * Class inheriting from Command interface used to call the solvers solve method
	 */
	class SolveCommand : public Command, public std::enable_shared_from_this<SolveCommand>
	{
		Q_OBJECT
	public:
		SolveCommand(KnowledgebaseCreator* gui);
		virtual ~SolveCommand();

		void execute();
		void undo();

		QJsonObject toJSON();
		/**
		 * Get models returned by solve call
		 */
		std::vector<Clingo::SymbolVector> getCurrentModels();
		/**
		 * Return solver result
		 */
		bool isSatisfiable();

		KnowledgebaseCreator* gui;

	private:
		bool satisfiable;
		std::vector<Clingo::SymbolVector> currentModels;
		/**
		 * Print models unsorted
		 */
		void printModels();
		/**
		 * Print models sorted
		 */
		void printSortedModels();
	};

} /* namespace kbcr */
