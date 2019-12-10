#include "handler/ExternalTableHandler.h"

#include "gui/KnowledgebaseCreator.h"

#include <ui_knowledgebasecreator.h>

#include <reasoner/asp/Solver.h>

#include <clingo.hh>

#include <QtWidgets/QTableWidgetItem>
#include <sstream>

namespace kbcr
{

ExternalTableHandler::ExternalTableHandler(KnowledgebaseCreator* gui)
{
    this->gui = gui;
    connect(this->gui->getUi()->externalTable, SIGNAL(itemPressed(QTableWidgetItem *)),
            this, SLOT(updateExternalLabel(QTableWidgetItem *)));
}

ExternalTableHandler::~ExternalTableHandler() {}

void ExternalTableHandler::fillExternalTable()
{
    drawExternalTable();
    for (Clingo::SymbolicAtom atom : this->gui->getSolver()->getSymbolicAtoms()) {
        if (atom.is_external()) {
            int pos = this->gui->getUi()->externalTable->rowCount();
            this->gui->getUi()->externalTable->insertRow(pos);
            std::stringstream ss;
            ss << atom.symbol();
            auto leftItem = new QTableWidgetItem(QString(ss.str().c_str()));
            leftItem->setFlags(leftItem->flags() ^ Qt::ItemIsEditable);
            this->gui->getUi()->externalTable->setItem(pos, 0, leftItem);
            bool found = false;
            for (auto vec : this->gui->getSolver()->getCurrentModels()) {
                if (find(vec.begin(), vec.end(), atom.symbol()) != vec.end()) {
                    found = true;
                    break;
                }
            }

            if (found) {
                auto rightItem = new QTableWidgetItem(QString("True"));
                rightItem->setFlags(rightItem->flags() ^ Qt::ItemIsEditable);
                this->gui->getUi()->externalTable->setItem(pos, 1, rightItem);
            } else {
                auto rightItem = new QTableWidgetItem(QString("False"));
                rightItem->setFlags(rightItem->flags() ^ Qt::ItemIsEditable);
                this->gui->getUi()->externalTable->setItem(pos, 1, rightItem);
            }
        }
    }
}

void ExternalTableHandler::drawExternalTable()
{
    this->gui->getUi()->externalTable->clear();
    this->gui->getUi()->externalTable->setColumnCount(2);
    this->gui->getUi()->externalTable->setHorizontalHeaderLabels(QStringList{"External Statement", "Truth Value", NULL});
    this->gui->getUi()->externalTable->setColumnWidth(0, 350);
    this->gui->getUi()->externalTable->horizontalHeader()->stretchLastSection();
    this->gui->getUi()->externalTable->setRowCount(0);
}

void ExternalTableHandler::updateExternalLabel(QTableWidgetItem* item) {
	if(item->column() == 0)
	{
		this->gui->getUi()->externalTextEdit->setText(item->text());
	}
}

} /* namespace kbcr */
