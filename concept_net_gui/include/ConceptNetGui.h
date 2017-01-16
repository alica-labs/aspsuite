#ifndef CONCEPTNETGUI_H
#define CONCEPTNETGUI_H

#include <QMainWindow>
#include <QtGui>

namespace Ui
{
	class ConceptNetGui;
}

class ConceptNetGui : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConceptNetGui(QWidget *parent = 0);
	~ConceptNetGui();

private:
	Ui::ConceptNetGui *ui;
};

#endif // CONCEPTNETGUI_H
