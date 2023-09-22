#include "mainwindow.h"
#include "filecontext.h"
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
#include <vector>
#include <QWidget>
#include <QProgressBar>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFile>


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
        QTextEdit* tabFileTextEdit = new QTextEdit();
        tabFileTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        QTextEdit* tabFilterTextEdit = new QTextEdit();
        tabFilterTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        QStringList splittedName = fileName.split('/');
        QPushButton* searchButton = new QPushButton();
        searchButton->setText("Search");
        QGridLayout* tabLayout = new QGridLayout();
        QProgressBar* progressBar = new QProgressBar();

        QWidget* tabPage = new QWidget();
        tabPage->setLayout(tabLayout);
        tabLayout->addWidget(tabFilterTextEdit, 0, 0);
        tabLayout->addWidget(searchButton, 0, 1);
        tabLayout->addWidget(progressBar, 1, 0, 1, 2);
        tabLayout->addWidget(tabFileTextEdit, 2, 0, 1, 2);

        tabLayout->setRowStretch(0, 15);
        tabLayout->setRowStretch(1, 10);
        tabLayout->setRowStretch(2, 100);

        tabWidget->addTab(tabPage, splittedName.constLast());

        tabWidget->setCurrentIndex(index++);
        tabWidget->setCurrentWidget(tabFileTextEdit);

        ifstream file;
        file.open(fileName.toStdString());
        FileContext* newFileContext = new FileContext();
        newFileContext->tabTextEdit = tabFileTextEdit;
        newFileContext->filterTextEdit = tabFilterTextEdit;
        newFileContext->sourceFilePath = fileName.toStdString();
        newFileContext->progressBar = progressBar;
        filesToFilter.push_back(newFileContext);
        connect(searchButton, &QPushButton::released, newFileContext, &FileContext::search);
        string line;

        if (file.is_open())
        {
            for (int i = 0; i < 5 && getline(file, line); i++)
            {
                tabFileTextEdit->append(QString::fromStdString(line));
            }

            file.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
