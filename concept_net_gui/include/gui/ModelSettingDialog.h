/*
 * ModelSettingDialog.h
 *
 *  Created on: Mar 12, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_GUI_MODELSETTINGDIALOG_H_
#define INCLUDE_GUI_MODELSETTINGDIALOG_H_

#include <QDialog>

namespace Ui
{
	class ModelSettingDialog;
}
namespace cng
{

	class ConceptNetGui;
	class ModelSettingDialog : public QDialog
	{
	Q_OBJECT
	public:
		ModelSettingDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~ModelSettingDialog();
		double getMinCn5Weight();
		int getNumberOfModels();
		bool isShowModelsInQuery();

	private:
		/**
		 * Pointer to Dialog
		 */
		Ui::ModelSettingDialog* ui;
		/**
		 * Pointer to main gui
		 */
		ConceptNetGui* mainGui;
		/**
		 * Connect every gui element
		 */
		void connectElements();

		int numberOfModels;
		bool showModelsInQuery;
		double minCN5Weight;

	private slots:
		void applySettings();
	};

} /* namespace cng */

#endif /* INCLUDE_GUI_MODELSETTINGDIALOG_H_ */
