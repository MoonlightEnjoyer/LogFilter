#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>
#include <vector>
#include <QString>
#include <string>
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QTextEdit>
#include <iostream>
#include <fstream>
#include <QString>
#include <QTabWidget>
#include <string>
#include <QWidget>
#include <QProgressBar>
#include <QThread>
#include <QFile>
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
