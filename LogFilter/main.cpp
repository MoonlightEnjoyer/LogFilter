#include "mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = new MainWindow();
    w->show();
    return a.exec();
}
