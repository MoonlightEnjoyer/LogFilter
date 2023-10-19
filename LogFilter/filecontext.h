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
    FileContext(
        std::string sourceFilePath,
        QLineEdit* filterTextEdit,
        QTextEdit* tabTextEdit,
        QThread* workerThread,
        QSpinBox* pageSpinBox,
        FileProcessWorker* worker);
    std::string sourceFilePath;
    QLineEdit* filterTextEdit;
    QTextEdit* tabTextEdit;
    QThread* workerThread;
    QSpinBox* pageSpinBox;
    FileProcessWorker* worker;
    unsigned int lineLength = 0;
    unsigned int pageSize = 10000;
    unsigned int page = 1;
    std::int64_t pageStartPos = 0;
    std::int64_t pageEndPos = 0;
    void getLineLength();

    ~FileContext();

public slots:
    void search();
    void getPage();
    void getNextPage();
    void getPrevPage();
};

#endif // FILECONTEXT_H
