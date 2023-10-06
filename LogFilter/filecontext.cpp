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
#include <filesystem>

void FileContext::search()
{
    QFuture<void> future = QtConcurrent::run(&FileContext::updateProgressBar, this);
}

long FileContext::processFile(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, long mapped_size)
{
    RE2 filter_regex("(" + currentContext->filterTextEdit->toPlainText().toStdString() + ")");
    uchar* memoryWriteStart = mapped_result_file;
    long matchCounter = 0;

    for (long i = 0; i < mapped_size; i++)
    {
        for (long j = i; j < mapped_size; j++)
        {
            if (mapped_file[j] == '\r')
            {
                if (j + 1 < mapped_size && mapped_file[j + 1] == '\n')
                {
                    j++;
                }

                std::copy(mapped_file + i, mapped_file + j + 1, memoryWriteStart);

                if (RE2::PartialMatch((char*)(memoryWriteStart), filter_regex, (void*)NULL))
                {
                    matchCounter += (j - i + 1);
                    memoryWriteStart += (j - i + 1);
                }
                else
                {
                    std::fill(memoryWriteStart, memoryWriteStart + j - i + 1, 0);
                }

                i = j;
                break;
            }
        }
    }

    return matchCounter;
}

long FileContext::startThreads(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, long mapped_size)
{
    QFuture<long> future0 = QtConcurrent::run(FileContext::processFile, currentContext, mapped_file, mapped_result_file, mapped_size);
    future0.waitForFinished();
    return future0.result();
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
    long sizeCounter = 0;
    long s = 0;
    if (sourceFile.open(QIODevice::ReadWrite))
    {
        if (resultFile.open(QIODevice::ReadWrite))
        {
            long mapped_size = 2l * 1024l * 1024l * 1024l;
            uchar* mapped_file;
            uchar* mapped_result_file;
            long mapped_total;

            long matchCounter = 0;

            for (mapped_total = 0; mapped_total + mapped_size <= sourceFile.size(); mapped_total += mapped_size)
            {
                mapped_file = sourceFile.map(mapped_total, mapped_size);
                mapped_result_file = resultFile.map(sizeCounter, mapped_size);
                matchCounter = startThreads(currentContext, mapped_file, mapped_result_file, mapped_size);
                sizeCounter += matchCounter;
                for (long f = mapped_size - 1; f > 0 && mapped_file[f] != '\n' && mapped_file[f] != '\r'; f--, mapped_total--)
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

    std::filesystem::resize_file("/home/dude/Desktop/filterTest/test.txt", sizeCounter);
}
