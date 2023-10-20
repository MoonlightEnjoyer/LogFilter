#include <QFile>
#include <fstream>
#include "fileProcessWorker.h"
#include "simpleregex.h"

std::int64_t FileProcessWorker::processFile(FileContext* currentContext, uchar* sourceMemory, uchar* resultMemory, std::int64_t bytesMapped, std::int64_t processedBytes, std::int64_t totalSize)
{
    std::string filter_regex_string(currentContext->filterLineEdit->text().toStdString());
    const int regexSize = filter_regex_string.size();
    char* filter_regex_char = (char*)filter_regex_string.c_str();
    std::int64_t matchCounter = 0;
    std::int64_t i_prev = 0;

    for (std::int64_t i = 0; i < bytesMapped; i++)
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            return -1;
        }

        for (std::int64_t j = i; j < bytesMapped; j++)
        {
            if (sourceMemory[j] == '\r' || sourceMemory[j] == '\n')
            {
                if (j + 1 < bytesMapped && sourceMemory[j + 1] == '\n')
                {
                    j++;
                }

                if (SimpleRegex::Match((char*)(sourceMemory + i), 0, j - i, filter_regex_char, regexSize))
                {
                    std::copy(sourceMemory + i, sourceMemory + j + 1, resultMemory + matchCounter);
                    matchCounter += (j - i + 1);
                }

                i = j;
                break;
            }
        }

        if (i - i_prev >= totalSize * 0.01 || i == bytesMapped - 1)
        {
            i_prev = i;
            emit progress((double)(processedBytes + i + 1) / (double)totalSize, currentContext->filterLineEdit);
        }

    }
    return matchCounter;
}

void FileProcessWorker::process()
{
    QFile sourceFile(QString::fromStdString(this->fileContext->sourceFilePath));
    std::ofstream resultFileFiller(resultFileName.toStdString());
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
            const std::int64_t defaultMapSize = (std::int64_t)2 * (std::int64_t)1024 * (std::int64_t)1024 * (std::int64_t)1024;
            uchar* mapped_file;
            uchar* mapped_result_file;
            std::int64_t mapped_total;
            std::int64_t matchCounter = 0;
            std::int64_t bytesMapped;

            for (mapped_total = 0; mapped_total <= sourceFile.size(); mapped_total += defaultMapSize)
            {
                bytesMapped = ((mapped_total + defaultMapSize) <= sourceFile.size()) ? defaultMapSize : (sourceFile.size() - mapped_total);
                mapped_file = sourceFile.map(mapped_total, bytesMapped);
                mapped_result_file = resultFile.map(sizeCounter, bytesMapped);
                matchCounter = processFile(this->fileContext, mapped_file, mapped_result_file, bytesMapped, mapped_total, sourceFile.size());
                if (matchCounter == -1)
                {
                    resultFile.close();
                    sourceFile.close();
                    std::filesystem::resize_file(resultFileName.toStdString(), sizeCounter);
                    this->thread()->quit();
                    return;
                }
                sizeCounter += matchCounter;
                for (std::int64_t f = bytesMapped - 1; f > 0 && mapped_file[f] != '\n' && mapped_file[f] != '\r'; f--, mapped_total--)
                {
                }
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);
            }
            resultFile.close();
        }
        sourceFile.close();
    }
    std::filesystem::resize_file(resultFileName.toStdString(), sizeCounter);

    this->thread()->quit();
}
