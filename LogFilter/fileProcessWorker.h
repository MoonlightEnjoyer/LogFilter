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
public slots:
    void process();
signals:
    void progress(std::int64_t);
};

#endif // FILEPROCESSWORKER_H
