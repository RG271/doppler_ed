#include <QApplication>
#include <QScreen>
#include "mainwindow.h"


// Main
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    // Center main window w
    QRect g = QApplication::primaryScreen()->geometry();
    w.move((g.width() - w.width()) >> 1, (g.height() - w.height()) >> 1);

    return a.exec();
}
