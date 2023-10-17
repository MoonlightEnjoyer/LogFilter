#include "fileProcessWorker.h"

std::int64_t FileProcessWorker::processFile(FileContext* currentContext, uchar* mapped_file, uchar* mapped_result_file, std::int64_t mapped_size)
{
    std::string filter_regex_s(currentContext->filterTextEdit->text().toStdString());
    char* filter_regex = (char*)filter_regex_s.c_str();    uchar* memoryWriteStart = mapped_result_file;
    std::int64_t matchCounter = 0;

    for (std::int64_t i = 0; i < mapped_size; i++)
    {
        for (std::int64_t j = i; j < mapped_size; j++)
        {
            if (mapped_file[j] == '\r' || mapped_file[j] == '\n')
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
            std::int64_t mappedReal;

            for (mapped_total = 0; mapped_total <= sourceFile.size(); mapped_total += mapped_size)
            {
                mappedReal = ((mapped_total + mapped_size) <= sourceFile.size()) ? mapped_size : (sourceFile.size() - mapped_total);
                mapped_file = sourceFile.map(mapped_total, mappedReal);
                mapped_result_file = resultFile.map(sizeCounter, mappedReal);
                matchCounter = processFile(this->fileContext, mapped_file, mapped_result_file, mappedReal);
                value = ((double)(mapped_total + mappedReal) / (double)sourceFile.size()) * 100.0;
                sizeCounter += matchCounter;
                for (std::int64_t f = mappedReal - 1; f > 0 && mapped_file[f] != '\n' && mapped_file[f] != '\r'; f--, mapped_total--)
                {
                }
                sourceFile.unmap(mapped_file);
                resultFile.unmap(mapped_result_file);

                emit progress(int(value), this->fileContext->filterTextEdit);
            }
            resultFile.close();
        }
        sourceFile.close();
    }
    std::filesystem::resize_file(resultFileName.toStdString(), sizeCounter);
}
