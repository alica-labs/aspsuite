#include <QApplication>
#include "../include/gui/ConceptNetGui.h"
#include <QtGui>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cng::ConceptNetGui w;
	w.show();

	return a.exec();
}
