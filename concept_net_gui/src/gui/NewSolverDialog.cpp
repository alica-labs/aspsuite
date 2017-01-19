/*
 * NewSolverDialog.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#include "../include/gui/NewSolverDialog.h"
#include "../include/gui/ConceptNetGui.h"

#include "../include/commands/NewSolverCommand.h"


#include <ui_newsolver.h>

namespace cng
{

	NewSolverDialog::NewSolverDialog(QWidget *parent, ConceptNetGui* gui) :
			QDialog(parent), ui(new Ui::NewSolverDialog)
	{
		this->mainGui = gui;
		this->ui->setupUi(this);
		this->connect(this->ui->cancelBtn, SIGNAL(released()), this, SLOT(close()));
		this->connect(this->ui->okBtn, SIGNAL(released()), this, SLOT(okBtnClicked()));
	}

	NewSolverDialog::~NewSolverDialog()
	{
	}

	void NewSolverDialog::okBtnClicked()
	{
		shared_ptr<NewSolverCommand> cmd = make_shared<NewSolverCommand>(this->mainGui->getArguments(), this->mainGui, this->mainGui->getArgumentString());
		this->mainGui->addToCommandHistory(cmd);
		cmd->execute();
		this->close();
	}

} /* namespace cng */
