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

void FileContext::search()
{
    QString resultFileName = QFileDialog::getSaveFileName();

    QThread* thread = new QThread();
    FileProcessWorker* worker = new FileProcessWorker();
    worker->fileContext = this;
    worker->resultFileName = resultFileName;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileProcessWorker::process);


    thread->start();
}

std::int64_t FileContext::processFile(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, std::int64_t mapped_size)
{
    std::string filter_regex_s(currentContext->filterTextEdit->toPlainText().toStdString());
    char* filter_regex = (char*)filter_regex_s.c_str();    uchar* memoryWriteStart = mapped_result_file;
    std::int64_t matchCounter = 0;

    for (std::int64_t i = 0; i < mapped_size; i++)
    {
        for (std::int64_t j = i; j < mapped_size; j++)
        {
            if (mapped_file[j] == '\r')
            {
                if (j + 1 < mapped_size && mapped_file[j + 1] == '\n')
                {
                    j++;
                }

                if (SimpleRegex::Match((char*)(mapped_file + i), 0, j - i, filter_regex, filter_regex_s.size()))
                {
                    std::copy(mapped_file + i, mapped_file + j + 1, memoryWriteStart + matchCounter);
                    matchCounter += (j - i + 1);
                }

                i = j;
                break;
            }
        }
    }

    return matchCounter;
}

std::int64_t FileContext::startThreads(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, std::int64_t mapped_size)
{
    QFuture<std::int64_t> future0 = QtConcurrent::run(FileContext::processFile, currentContext, mapped_file, mapped_result_file, mapped_size);
    future0.waitForFinished();
    return future0.result();
}

void FileContext::updateProgressBar(FileContext* currentContext, QString resultFileName)
{
    using namespace std;

    double value = 0;

    QFile sourceFile(QString::fromStdString(currentContext->sourceFilePath));
    ofstream resultFileFiller(resultFileName.toStdString());
    resultFileFiller.seekp(sourceFile.size() - 1);
    resultFileFiller.write("", 1);
    resultFileFiller.flush();
    resultFileFiller.close();
    QFile resultFile(resultFileName);
    std::int64_t sizeCounter = 0;
    std::int64_t s = 0;
    if (sourceFile.open(QIODevice::ReadWrite))
    {
        if (resultFile.open(QIODevice::ReadWrite))
        {
            std::int64_t mapped_size = (std::int64_t)2 * (std::int64_t)1024 * (std::int64_t)1024 * (std::int64_t)1024;
            uchar* mapped_file;
            uchar* mapped_result_file;
            std::int64_t mapped_total;

            std::int64_t matchCounter = 0;

            for (mapped_total = 0; mapped_total + mapped_size <= sourceFile.size(); mapped_total += mapped_size)
            {
                mapped_file = sourceFile.map(mapped_total, mapped_size);
                mapped_result_file = resultFile.map(sizeCounter, mapped_size);
                matchCounter = startThreads(currentContext, mapped_file, mapped_result_file, mapped_size);
                sizeCounter += matchCounter;
                for (std::int64_t f = mapped_size - 1; f > 0 && mapped_file[f] != '\n' && mapped_file[f] != '\r'; f--, mapped_total--)
                {
                }
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);
                value = ((double)(mapped_total + mapped_size) / (double)sourceFile.size()) * 100.0;

                currentContext->progressBar->setValue(value);
                currentContext->progressBar->setFormat("Progress: " + QString::number(mapped_total + mapped_size) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");
            }

            mapped_file = sourceFile.map(mapped_total, sourceFile.size() - mapped_total);
            mapped_result_file = resultFile.map(sizeCounter, sourceFile.size() - mapped_total);
            matchCounter = startThreads(currentContext, mapped_file, mapped_result_file, sourceFile.size() - mapped_total);
            sizeCounter += matchCounter;
            sourceFile.unmap(mapped_file);
            resultFile.unmap(mapped_result_file);
            value = 100.0;

            currentContext->progressBar->setValue(value);
            currentContext->progressBar->setFormat("Progress: " + QString::number(mapped_total) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");

            resultFile.close();
        }
        sourceFile.close();
    }

    std::filesystem::resize_file(resultFileName.toStdString(), sizeCounter);
}
