#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QHBoxLayout>
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include "mainwindow.h"
#include "fileProcessWorker.h"
#include "filecontext.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainTextEdit = this->centralWidget()->findChild<QTextEdit*>("mainTextEdit");
    openFileButton = this->centralWidget()->findChild<QPushButton*>("openFileButton");
    tabWidget = this->centralWidget()->findChild<QTabWidget*>("tabWidget");
    connect(openFileButton, &QPushButton::released, this, &MainWindow::openFile);
}

void MainWindow::updateProgressBar(double value, QLineEdit* lineEdit)
{
    QPalette palette = lineEdit->palette();
    QRectF qrect = QRectF(lineEdit->rect());
    QLinearGradient gradient = QLinearGradient(qrect.topLeft(), qrect.topRight());
    gradient.setColorAt(value - 0.001, value == 1.0 ? QColor(65, 249, 112, 100) : QColor(249, 158, 65, 100));
    gradient.setColorAt(value, QColor(255, 255, 255));
    gradient.setColorAt(value + 0.001, QColor(255, 255, 255));
    palette.setBrush(QPalette::Base, QBrush(gradient));
    lineEdit->setPalette(palette);
}

void MainWindow::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();

    for (const auto& fileName : fileNames)
    {
        QTextEdit* tabFileTextEdit = new QTextEdit();
        tabFileTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        QLineEdit* tabFilterTextEdit = new QLineEdit();
        QStringList splittedName = fileName.split('/');
        QPushButton* searchButton = new QPushButton();
        searchButton->setText("Search");
        QGridLayout* tabLayout = new QGridLayout();

        QWidget* tabPage = new QWidget();
        tabPage->setLayout(tabLayout);

        QPushButton* openPageButton = new QPushButton();
        openPageButton->setText("Open page");
        QPushButton* prevPageButton = new QPushButton();
        prevPageButton->setText("Prev page");
        QPushButton* nextPageButton = new QPushButton();
        nextPageButton->setText("Next page");

        QSpinBox* pageSpinBox = new QSpinBox();
        pageSpinBox->setValue(1);
        pageSpinBox->setMaximum(1);
        pageSpinBox->setMaximum(INT_MAX);

        QHBoxLayout* horLayout = new QHBoxLayout();

        horLayout->addWidget(prevPageButton);
        horLayout->addWidget(nextPageButton);
        horLayout->addWidget(pageSpinBox);
        horLayout->addWidget(openPageButton);

        tabLayout->addWidget(tabFilterTextEdit, 0, 0);
        tabLayout->addWidget(searchButton, 0, 1);
        tabLayout->addWidget(tabFileTextEdit, 1, 0, 1, 2);
        tabLayout->addLayout(horLayout, 2, 0);
        tabLayout->setRowStretch(0, 15);
        tabLayout->setRowStretch(1, 100);

        tabWidget->addTab(tabPage, splittedName.constLast());

        QThread* thread = new QThread();
        FileProcessWorker* worker = new FileProcessWorker();

        FileContext* newFileContext = new FileContext(fileName.toStdString(), tabFilterTextEdit, tabFileTextEdit, thread, pageSpinBox, worker);

        connect(thread, &QThread::started, worker, &FileProcessWorker::process);
        connect(worker, &FileProcessWorker::progress, this, &MainWindow::updateProgressBar);
        connect(searchButton, &QPushButton::released, newFileContext, &FileContext::search);
        connect(openPageButton, &QPushButton::released, newFileContext, &FileContext::getPage);
        connect(nextPageButton, &QPushButton::released, newFileContext, &FileContext::getNextPage);
        connect(prevPageButton, &QPushButton::released, newFileContext, &FileContext::getPrevPage);


        newFileContext->getPage();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
