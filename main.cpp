#include "mainwindow.h"
#include "maze.h"
#include <iostream>
#include <utility>
#include <QApplication>
#include <QThread>


int main(int argc, char *argv[])
{
    qDebug()<<QThread::currentThreadId();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
