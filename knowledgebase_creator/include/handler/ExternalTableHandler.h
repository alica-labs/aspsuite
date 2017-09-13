/*
 * ExternalTableHandler.h
 *
 *  Created on: Mar 12, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_HANDLER_EXTERNALTABLEHANDLER_H_
#define INCLUDE_HANDLER_EXTERNALTABLEHANDLER_H_

#include <QObject>
#include <memory>

namespace kbcr
{

	class KnowledgebaseCreator;
	/**
	 * Handles the external statement table
	 */
	class ExternalTableHandler : public QObject
	{
	Q_OBJECT
	public:
		/**
		 * Constructor
		 */
		ExternalTableHandler(KnowledgebaseCreator * gui);
		/**
		 * Destructor
		 */
		virtual ~ExternalTableHandler();

	public slots:
		/**
		 * Slot to fill command history table view
		 */
		void fillExternalTable();
		/**
		 * Slot to redraw the command history
		 */
		void drawExternalTable();

	private:
		/**
		 * Pointer to main Gui handler
		 */
		KnowledgebaseCreator * gui;
	};

} /* namespace kbcr */

#endif /* INCLUDE_HANDLER_EXTERNALTABLEHANDLER_H_ */
