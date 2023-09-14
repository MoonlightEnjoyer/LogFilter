#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <iostream>
#include <fstream>
#include <QString>
#include <QTabWidget>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainTextEdit = this->centralWidget()->findChild<QTextEdit*>("mainTextEdit");
    openFileButton = this->centralWidget()->findChild<QPushButton*>("openFileButton");
    tabWidget = this->centralWidget()->findChild<QTabWidget*>("tabWidget");
    connect(openFileButton, &QPushButton::released, this, &MainWindow::openFile);
}

void MainWindow::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();

    using namespace std;
    int index = 0;
    for (const auto& fileName : fileNames)
    {
        QTextEdit* newTextEdit = new QTextEdit();
        newTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        QStringList splittedName = fileName.split('/');
        tabWidget->addTab(newTextEdit, splittedName.constLast());
        tabWidget->setCurrentIndex(index++);
        tabWidget->setCurrentWidget(newTextEdit);

        ifstream file;
        file.open(fileName.toStdString());
        string line;

        if (file.is_open())
        {
            for (int i = 0; i < 5; i++)
            {
                getline(file, line);
                newTextEdit->append(QString::fromStdString(line));
            }
        }


    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

