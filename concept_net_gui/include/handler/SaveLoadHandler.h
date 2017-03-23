/*
 * SaveLoadHandler.h
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_HANDLER_SAVELOADHANDLER_H_
#define INCLUDE_HANDLER_SAVELOADHANDLER_H_

#include <QObject>
#include <memory>

namespace cng
{

	class LoadSavedProgramCommand;
	class ConceptNetGui;
	/**
	 * Handles Save and Load methods
	 */
	class SaveLoadHandler : public QObject
	{
	Q_OBJECT
	public:
		/**
		 * Constructor
		 * @param gui pointer to main window handler
		 */
		SaveLoadHandler(ConceptNetGui* gui);
		virtual ~SaveLoadHandler();

		std::shared_ptr<LoadSavedProgramCommand> currentLoadCmd;

	public slots:
		/**
		 * Save command history in Json
		 */
		void saveProgram();
		/**
		 * Save models depending on settings
		 */
		void saveModels();
		/**
		 * Load saved command history from Json
		 */
		void loadProgram();
		/**
		 * Load Back Ground Knowledge File
		 */
		void loadBackgroundKnowledge();


	private:
		/**
		 * Pointer to the main window handler
		 */
		ConceptNetGui* gui;

	};

} /* namespace cng */

#endif /* INCLUDE_HANDLER_SAVELOADHANDLER_H_ */
