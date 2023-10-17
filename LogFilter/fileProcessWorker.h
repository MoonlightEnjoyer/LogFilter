#ifndef FILEPROCESSWORKER_H
#define FILEPROCESSWORKER_H

#include <QString>
#include <iostream>
#include "filecontext.h"
#include "simpleregex.h"

class FileContext;

class FileProcessWorker : public QObject
{
    Q_OBJECT
public:
    FileContext* fileContext;
    QString resultFileName;
    FileProcessWorker() {}
    virtual ~FileProcessWorker() {}
    static std::int64_t processFile(FileContext*, uchar*, uchar*, std::int64_t);
public slots:
    void process();
signals:
    void progress(int, QProgressBar*);
};

#endif // FILEPROCESSWORKER_H
