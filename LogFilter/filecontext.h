#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include <QTextEdit>
#include <QString>
#include <QProgressBar>
#include <QThread>
#include <string>
#include <iostream>
#include <fstream>
#include "simpleregex.h"
#include "mainwindow.h"
#include "fileProcessWorker.h"
#include <QLineEdit>

class FileProcessWorker;

class FileContext : public QObject
{
    Q_OBJECT
public:
    std::string sourceFilePath;
    QLineEdit* filterTextEdit;
    QTextEdit* tabTextEdit;
    QProgressBar* progressBar;
    QThread* workerThread;
    FileProcessWorker* worker;

public slots:
    void search();
};

#endif // FILECONTEXT_H
