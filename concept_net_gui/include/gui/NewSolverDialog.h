/*
 * NewSolverDialog.h
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#ifndef SRC_GUI_NEWSOLVERDIALOG_H_
#define SRC_GUI_NEWSOLVERDIALOG_H_

#include <QDialog>
#include <QtGui>

namespace Ui
{
	class NewSolverDialog;
}

namespace cng
{
	class ConceptNetGui;
	class NewSolverDialog : public QDialog
	{
		Q_OBJECT

	public:
		NewSolverDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~NewSolverDialog();

	private slots:
			void okBtnClicked();

	private:
		Ui::NewSolverDialog* ui;
		ConceptNetGui* mainGui;
	};

} /* namespace cng */

#endif /* SRC_GUI_NEWSOLVERDIALOG_H_ */
