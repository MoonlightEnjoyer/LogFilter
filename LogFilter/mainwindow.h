#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QTextEdit>
#include <QTabWidget>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include <QTabWidget>
#include <QWidget>
#include <QProgressBar>
#include <QThread>
#include <QFile>
#include <QLineEdit>
#include <iostream>
#include <fstream>
#include <string>
#include "ui_mainwindow.h"
#include "fileProcessWorker.h"
#include "filecontext.h"

class FileContext;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void openFile();
    void updateProgressBar(int value, QProgressBar* progressBar);
private:
    Ui::MainWindow *ui;
    QPushButton* openFileButton;
    QTextEdit* mainTextEdit;
    QTabWidget* tabWidget;
};

#endif // MAINWINDOW_H
