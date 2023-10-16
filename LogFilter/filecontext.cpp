#include "filecontext.h"
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
#include <filesystem>
#include "simpleregex.h"
#include "fileProcessWorker.h"
#include "mainwindow.h"

void FileContext::search()
{
    QString resultFileName = QFileDialog::getSaveFileName();

    QThread* thread = new QThread();
    FileProcessWorker* worker = new FileProcessWorker();
    worker->fileContext = this;
    worker->resultFileName = resultFileName;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileProcessWorker::process);
    connect(worker, &FileProcessWorker::progress, (MainWindow*)this->mainWindow, &MainWindow::updateProgressBar);


    thread->start();
}
