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
#include <re2/re2.h>
#include <absl/strings/string_view.h>

void FileContext::search()
{
    QFuture<void> future = QtConcurrent::run(&FileContext::updateProgressBar, this);
}

void FileContext::processFile(int id, int threadsNumber, FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, long mapped_size)
{
    RE2 filter_regex("(" + currentContext->filterTextEdit->toPlainText().toStdString() + ")");

    long bytesToProcess;

    if (mapped_size % threadsNumber == 0)
    {
        bytesToProcess = mapped_size / threadsNumber;
    }
    else if (id == threadsNumber - 1)
    {
        bytesToProcess = mapped_size % threadsNumber;
    }
    else
    {
        bytesToProcess = (mapped_size - mapped_size % threadsNumber) / threadsNumber;
    }

    long matchCounter = 0;
    long memoryStart = id * bytesToProcess;
    long memoryEnd = (id + 1) * bytesToProcess;
    uchar* memoryWriteStart = memoryStart + mapped_result_file;

    for (long i = memoryStart; i < memoryEnd; i++)
    {
        for (long j = i; j < memoryEnd; j++)
        {
            if (mapped_file[j] == '\n')
            {
                std::copy(mapped_file + i, mapped_file + j, memoryWriteStart + matchCounter);

                matchCounter += (j - i) * RE2::PartialMatch((char*)(memoryWriteStart + matchCounter), filter_regex, (void*)NULL);

                i = j;
                break;
            }
        }
    }
}

void FileContext::startThreads(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, long mapped_size)
{
    QFuture<void> future0 = QtConcurrent::run(FileContext::processFile, 0, 1, currentContext, mapped_file, mapped_result_file, mapped_size);
    future0.waitForFinished();
}

void FileContext::updateProgressBar(FileContext* currentContext)
{
    using namespace std;

    double value = 0;

    QFile sourceFile(QString::fromStdString(currentContext->sourceFilePath));
    ofstream resultFileFiller("/home/dude/Desktop/filterTest/test.txt");
    resultFileFiller.seekp(sourceFile.size() - 1);
    resultFileFiller.write("", 1);
    resultFileFiller.flush();
    resultFileFiller.close();
    QFile resultFile("/home/dude/Desktop/filterTest/test.txt");
    if (sourceFile.open(QIODevice::ReadWrite))
    {
        if (resultFile.open(QIODevice::ReadWrite))
        {
            long mapped_size = 2l * 1024l * 1024l * 1024l;
            uchar* mapped_file;
            uchar* mapped_result_file;
            long main_processing_size = sourceFile.size() / mapped_size * mapped_size;
            long mapped_total;

            for (mapped_total = 0; mapped_total < main_processing_size; mapped_total += mapped_size)
            {
                mapped_file = sourceFile.map(mapped_total, mapped_size);
                mapped_result_file = resultFile.map(mapped_total, mapped_size);
                startThreads(currentContext, mapped_file, mapped_result_file, mapped_size);
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);
                value = ((double)(mapped_total + mapped_size) / (double)sourceFile.size()) * 100.0;

                currentContext->progressBar->setValue(value);
                currentContext->progressBar->setFormat("Progress: " + QString::number(mapped_total + mapped_size) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");
            }

            mapped_file = sourceFile.map(mapped_total, sourceFile.size() - mapped_total);
            mapped_result_file = resultFile.map(mapped_total, sourceFile.size() - mapped_total);
            startThreads(currentContext, mapped_file, mapped_result_file, sourceFile.size() - mapped_total);
            sourceFile.unmap(mapped_file);
            resultFile.unmap(mapped_result_file);
            value = 100.0;

            currentContext->progressBar->setValue(value);
            currentContext->progressBar->setFormat("Progress: " + QString::number(mapped_total) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");

            resultFile.close();
        }
        sourceFile.close();
    }
}
