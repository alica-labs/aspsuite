#include "../include/ConceptNetGui.h"
#include <ui_conceptnetgui.h>

ConceptNetGui::ConceptNetGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConceptNetGui)
{
    ui->setupUi(this);
}

ConceptNetGui::~ConceptNetGui()
{
    delete ui;
}
