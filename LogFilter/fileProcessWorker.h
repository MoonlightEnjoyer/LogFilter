#ifndef FILEPROCESSWORKER_H
#define FILEPROCESSWORKER_H

#include <QObject>
#include <QString>
#include <iostream>
#include "filecontext.h"

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
    void progress(std::int64_t);
};

#endif // FILEPROCESSWORKER_H
