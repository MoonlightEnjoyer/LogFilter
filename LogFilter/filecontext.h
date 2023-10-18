#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include <QTextEdit>
#include <QThread>
#include <string>
#include <iostream>
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
    QThread* workerThread;
    FileProcessWorker* worker;

public slots:
    void search();
};

#endif // FILECONTEXT_H
