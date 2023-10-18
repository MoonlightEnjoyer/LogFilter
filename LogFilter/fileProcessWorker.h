#ifndef FILEPROCESSWORKER_H
#define FILEPROCESSWORKER_H

#include <QString>
#include <QLineEdit>
#include <iostream>
#include "filecontext.h"

class FileContext;

class FileProcessWorker : public QObject
{
    Q_OBJECT
public:
    FileContext* fileContext;
    QString resultFileName;
    FileProcessWorker() {}
    virtual ~FileProcessWorker() {}
    std::int64_t processFile(FileContext*, uchar*, uchar*, std::int64_t,  std::int64_t, std::int64_t);
public slots:
    void process();
signals:
    void progress(double, QLineEdit*);
};

#endif // FILEPROCESSWORKER_H
