#include "gui/KnowledgebaseCreator.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtGui>

#include <signal.h>
#include <unistd.h>

void catchUnixSignals(const std::vector<int>& quitSignals)
{

    auto handler = [](int sig) -> void { QCoreApplication::quit(); };

    // each of these signals calls the handler (quits the QCoreApplication).
    for (int sig : quitSignals) {
        signal(sig, handler);
    }
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/drawing.svg"));
    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
    kbcr::KnowledgebaseCreator w;
    // Center GUI on screen
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - w.width()) / 2;
    int y = (screenGeometry.height() - w.height()) / 2;
    w.move(x, y);
    w.show();

    return a.exec();
}
