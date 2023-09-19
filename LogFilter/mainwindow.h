#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>
#include <vector>
#include <QString>
#include <string>
#include <regex>
#include "filecontext.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openFile();
    void addFilter();
private:
    Ui::MainWindow *ui;
    QPushButton* openFileButton;
    QTextEdit* mainTextEdit;
    QTabWidget* tabWidget;
    std::vector<FileContext*> filesToFilter;
};



#endif // MAINWINDOW_H
