#include <QApplication>
#include "../include/gui/ConceptNetGui.h"
#include <QtGui>
#include <qdesktopwidget.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cng::ConceptNetGui w;
	//Center GUI on screen
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width()- w.width()) / 2;
	int y = (screenGeometry.height()-w.height()) / 2;
	w.move(x, y);
	w.show();

	return a.exec();
}
