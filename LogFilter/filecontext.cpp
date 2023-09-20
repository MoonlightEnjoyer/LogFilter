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
#include <regex>
#include <vector>
#include <QWidget>
#include <QProgressBar>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFile>

void FileContext::search()
{
    QFuture<void> future = QtConcurrent::run(&FileContext::updateProgressBar, this);
}

void processFile(int id, uchar* mapped_file, uchar* mapped_result_file, long mapped_size)
{
    std::copy(mapped_file + id * (mapped_size / 4), mapped_file + id * (mapped_size / 4) + mapped_size / 4, mapped_result_file + id * (mapped_size / 4));
}

void FileContext::updateProgressBar(FileContext* currentContext)
{
    using namespace std;
    std::regex filter_regex(currentContext->filterTextEdit->toPlainText().toStdString());
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
                QFuture<void> future0 = QtConcurrent::run(processFile, 0, mapped_file, mapped_result_file, mapped_size);
                QFuture<void> future1 = QtConcurrent::run(processFile, 1, mapped_file, mapped_result_file, mapped_size);
                QFuture<void> future2 = QtConcurrent::run(processFile, 2, mapped_file, mapped_result_file, mapped_size);
                QFuture<void> future3 = QtConcurrent::run(processFile, 3, mapped_file, mapped_result_file, mapped_size);
                future0.waitForFinished();
                future1.waitForFinished();
                future2.waitForFinished();
                future3.waitForFinished();
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);
                value = ((double)(mapped_total + mapped_size) / (double)sourceFile.size()) * 100.0;

                currentContext->progressBar->setValue(value);
                currentContext->progressBar->setFormat("Progress: " + QString::number(mapped_total + mapped_size) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");
            }

            mapped_file = sourceFile.map(mapped_total, sourceFile.size() - mapped_total);
            mapped_result_file = resultFile.map(mapped_total, sourceFile.size() - mapped_total);
            std::copy(mapped_file, mapped_file + sourceFile.size() - mapped_total, mapped_result_file);
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
