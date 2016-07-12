#include "controller.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller* controller = new Controller();

    MainWindow w(controller);
    w.show();

    int ret = a.exec();

    delete controller;

    return ret;
}
