#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include "ui_mainwindow.h"

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
    void updateProgressBar(double value, QLineEdit* lineEdit);
    void closeTab(int index);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
