#include <QtGui/QApplication>
#include <QThread>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Zouga Soft");
    QCoreApplication::setApplicationName("Relax! lite");
    QCoreApplication::setApplicationVersion("1.0.0");

    qDebug() << qApp->thread()->currentThreadId();

    MainWindow w;
    w.show();
    
    return a.exec();
}
