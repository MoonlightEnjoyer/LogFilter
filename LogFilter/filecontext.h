#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include <QTextEdit>
#include <QThread>
#include <QSpinBox>
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
    QSpinBox* pageSpinBox;
    FileProcessWorker* worker;
    unsigned int lineLength = 0;
    unsigned int pageSize = 10000;
    unsigned int page = 1;
    void getLineLength();

public slots:
    void search();
    void getPage();
};

#endif // FILECONTEXT_H
