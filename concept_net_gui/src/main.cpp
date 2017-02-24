#include <QApplication>
#include <QtGui>
#include <QDesktopWidget>
#include "gui/ConceptNetGui.h"

#include <signal.h>
#include <unistd.h>


void catchUnixSignals(const std::vector<int>& quitSignals) {

    auto handler = [](int sig) ->void {
        QCoreApplication::quit();
    };

    // each of these signals calls the handler (quits the QCoreApplication).
    for ( int sig : quitSignals )
    {
        signal(sig, handler);
    }
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
	cng::ConceptNetGui w;
	//Center GUI on screen
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width()- w.width()) / 2;
	int y = (screenGeometry.height()-w.height()) / 2;
	w.move(x, y);
	w.show();

	return a.exec();
}
