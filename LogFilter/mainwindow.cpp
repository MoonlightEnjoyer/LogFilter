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
#include <regex>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainTextEdit = this->centralWidget()->findChild<QTextEdit*>("mainTextEdit");
    openFileButton = this->centralWidget()->findChild<QPushButton*>("openFileButton");
    startFilterButton = this->centralWidget()->findChild<QPushButton*>("startFilterButton");
    tabWidget = this->centralWidget()->findChild<QTabWidget*>("tabWidget");
    connect(openFileButton, &QPushButton::released, this, &MainWindow::openFile);
    connect(startFilterButton, &QPushButton::released, this, &MainWindow::filterFile);
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
        filesToFilter.push_back(fileName.toStdString());
        string line;

        if (file.is_open())
        {
            for (int i = 0; i < 5 && getline(file, line); i++)
            {
                newTextEdit->append(QString::fromStdString(line));
            }

            file.close();
        }
    }
}

void MainWindow::filterFile()
{
    using namespace std;
    for (int i = 0; i < tabWidget->count(); i++)
    {
        ifstream sourceFile;
        ofstream resultFile;
        string filepath = filesToFilter[i];
        sourceFile.open(filepath);
        resultFile.open("filtered_test_file.txt");
        if (sourceFile.is_open())
        {
            if (resultFile.is_open())
            {
                string line;
                while (getline(sourceFile, line))
                {
                    resultFile << line << endl;
                }

                resultFile.close();
            }
            sourceFile.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
