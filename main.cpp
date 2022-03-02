#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QFile>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile file(":/stylesheets/Combinear.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(file.readAll()) };
    a.setStyleSheet(styleSheet);

    w.setWindowTitle("SynchronizedPlayer");
    w.show();

    QObject::connect(&w, &MainWindow::themeChanged, &w, [&w](QString theme)->void
    {
        QFile file(":/stylesheets/" + theme + ".qss");
        file.open(QFile::ReadOnly);
        QString styleSheet { QLatin1String(file.readAll()) };
        w.setStyleSheet(styleSheet);
    });

    return a.exec();
}
