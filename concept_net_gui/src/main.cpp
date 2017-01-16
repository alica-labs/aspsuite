#include <QApplication>
#include "../include/ConceptNetGui.h"
#include <QtGui>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ConceptNetGui w;
	w.show();

	return a.exec();
}
