/*
 * ExternalTableHandler.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: stefan
 */

#include <sstream>

#include <handler/ExternalTableHandler.h>

#include "gui/ConceptNetGui.h"

#include <ui_conceptnetgui.h>

#include <asp_solver/ASPSolver.h>

#include <clingo/clingocontrol.hh>
#include <clasp/solver.h>

namespace cng
{

	ExternalTableHandler::ExternalTableHandler(ConceptNetGui* gui)
	{
		this->gui = gui;
	}

	ExternalTableHandler::~ExternalTableHandler()
	{
	}

	void ExternalTableHandler::fillExternalTable()
	{
		drawExternalTable();
		for (auto iter = this->gui->getSolver()->clingo->begin(); iter != this->gui->getSolver()->clingo->end(); iter =
				this->gui->getSolver()->clingo->next(iter))
		{
			if (this->gui->getSolver()->clingo->valid(iter))
			{
				if (this->gui->getSolver()->clingo->external(iter))
				{
					int pos = this->gui->getUi()->externalTable->rowCount();
					this->gui->getUi()->externalTable->insertRow(pos);
					std::stringstream ss;
					ss << this->gui->getSolver()->clingo->atom(iter);
					auto leftItem = new QTableWidgetItem(QString(ss.str().c_str()));
					leftItem->setFlags(leftItem->flags() ^ Qt::ItemIsEditable);
					this->gui->getUi()->externalTable->setItem(pos, 0, leftItem);

					if (this->gui->getSolver()->clingo->atom(iter).sign() == 0)
					{
						auto rightItem = new QTableWidgetItem(QString("True"));
						rightItem->setFlags(rightItem->flags() ^ Qt::ItemIsEditable);
						this->gui->getUi()->externalTable->setItem(pos, 1, rightItem);
					}
					else
					{
						auto rightItem = new QTableWidgetItem(QString("False"));
						rightItem->setFlags(rightItem->flags() ^ Qt::ItemIsEditable);
						this->gui->getUi()->externalTable->setItem(pos, 1, rightItem);
					}
				}
				//TODO find way to get truth values from an external
				std::cout << iter << " " << this->gui->getSolver()->clingo->external(iter) << " "
						<< this->gui->getSolver()->clingo->atom(iter) << " " << std::endl;
				std::cout << Clasp::valSign(Clasp::trueValue(Clasp::decodeLit(this->gui->getSolver()->clingo->literal(iter)))) << std::endl;

			}
			else
			{
				break;
			}
		}
	}

	void ExternalTableHandler::drawExternalTable()
	{
		this->gui->getUi()->externalTable->clear();
		this->gui->getUi()->externalTable->setColumnCount(2);
		this->gui->getUi()->externalTable->setHorizontalHeaderLabels(QStringList {"External Statement", "Truth Value",
		NULL});
		this->gui->getUi()->externalTable->setColumnWidth(0, 350);
		this->gui->getUi()->externalTable->horizontalHeader()->stretchLastSection();
		this->gui->getUi()->externalTable->setRowCount(0);
	}

} /* namespace cng */
