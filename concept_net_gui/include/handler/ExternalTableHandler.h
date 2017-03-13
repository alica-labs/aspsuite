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

namespace cng
{

	class ConceptNetGui;
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
		ExternalTableHandler(ConceptNetGui * gui);
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
		ConceptNetGui * gui;
	};

} /* namespace cng */

#endif /* INCLUDE_HANDLER_EXTERNALTABLEHANDLER_H_ */
