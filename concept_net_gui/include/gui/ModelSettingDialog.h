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
	/**
	 * Dialog used to change settings for models and cn5
	 */
	class ModelSettingDialog : public QDialog
	{
	Q_OBJECT
	public:
		ModelSettingDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~ModelSettingDialog();
		double getMinCn5Weight();
		int getNumberOfModels();
		bool isShowModelsInQuery();
		bool isSaveSortedChecked();

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
		/**
		 * Number of models to show
		 */
		int numberOfModels;
		/**
		 * Are models supposed to be shown in quey result?
		 */
		bool showModelsInQuery;
		/**
		 * Are models supposed to be saved sorted?
		 */
		bool saveModelsSorted;
		/**
		 * Minimum weight concept net edges need to have to be considered
		 */
		double minCN5Weight;

	private slots:
		/**
		 * Slot to apply settings
		 */
		void applySettings();
	};

} /* namespace cng */

#endif /* INCLUDE_GUI_MODELSETTINGDIALOG_H_ */
