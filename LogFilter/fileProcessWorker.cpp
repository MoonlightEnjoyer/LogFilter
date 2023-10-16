#include "fileProcessWorker.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

void FileProcessWorker::process()
{
    using namespace std;

    double value = 0;

    QFile sourceFile(QString::fromStdString(this->fileContext->sourceFilePath));
    ofstream resultFileFiller(resultFileName.toStdString());
    resultFileFiller.seekp(sourceFile.size() - 1);
    resultFileFiller.write("", 1);
    resultFileFiller.flush();
    resultFileFiller.close();
    QFile resultFile(resultFileName);
    std::int64_t sizeCounter = 0;
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
                matchCounter = FileContext::processFile(this->fileContext, mapped_file, mapped_result_file, mapped_size);
                sizeCounter += matchCounter;
                for (std::int64_t f = mapped_size - 1; f > 0 && mapped_file[f] != '\n' && mapped_file[f] != '\r'; f--, mapped_total--)
                {
                }
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);
                value = ((double)(mapped_total + mapped_size) / (double)sourceFile.size()) * 100.0;

                //this->fileContext->progressBar->setValue(value);
                //this->fileContext->progressBar->setFormat("Progress: " + QString::number(mapped_total + mapped_size) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");
            }

            mapped_file = sourceFile.map(mapped_total, sourceFile.size() - mapped_total);
            mapped_result_file = resultFile.map(sizeCounter, sourceFile.size() - mapped_total);
            matchCounter = FileContext::processFile(this->fileContext, mapped_file, mapped_result_file, sourceFile.size() - mapped_total);
            sizeCounter += matchCounter;
            sourceFile.unmap(mapped_file);
            resultFile.unmap(mapped_result_file);
            value = 100.0;

            //this->fileContext->progressBar->setValue(value);
            //this->fileContext->progressBar->setFormat("Progress: " + QString::number(mapped_total) + " / " + QString::number(sourceFile.size()) + " * 100 = " + QString::number(value) + "%");

            resultFile.close();
        }
        sourceFile.close();
    }

    std::filesystem::resize_file(resultFileName.toStdString(), sizeCounter);
}
