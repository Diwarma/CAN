#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "Приложение запущено";

    MainWindow w;
    qDebug() << "MainWindow создан";

    w.show();
    return a.exec();
}
