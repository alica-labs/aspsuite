/*
 * ModelSettingDialog.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: stefan
 */

#include <gui/ModelSettingDialog.h>
#include <gui/ConceptNetGui.h>

#include <ui_modelsettings.h>
namespace cng
{

	ModelSettingDialog::ModelSettingDialog(QWidget *parent, ConceptNetGui* gui) :
			QDialog(parent), ui(new Ui::ModelSettingDialog)
	{
		this->mainGui = gui;
		this->ui->setupUi(this);
		applySettings();
		connectElements();
	}

	ModelSettingDialog::~ModelSettingDialog()
	{
	}

	void ModelSettingDialog::connectElements()
	{
		this->connect(this->ui->cancelBtn, SIGNAL(released()), this, SLOT(close()));
		this->connect(this->ui->applyBtn, SIGNAL(released()), this, SLOT(applySettings()));
	}

	double ModelSettingDialog::getMinCn5Weight()
	{
		return this->minCN5Weight;
	}

	int ModelSettingDialog::getNumberOfModels()
	{
		return this->numberOfModels;
	}

	bool ModelSettingDialog::isShowModelsInQuery()
	{
		return this->showModelsInQuery;
	}

	void ModelSettingDialog::applySettings()
	{
		this->numberOfModels = this->ui->numberOfModelsSpinBox->value();
		this->minCN5Weight = this->ui->minWeightSpinBox->value();
		if(this->ui->showModelsComboBox->currentText().contains("Yes"))
		{
			this->showModelsInQuery = true;
		}
		else
		{
			this->showModelsInQuery = false;
		}
		this->close();
	}

} /* namespace cng */
