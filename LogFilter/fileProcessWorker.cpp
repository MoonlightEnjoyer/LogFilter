#include <QFile>
#include <fstream>
#include "fileProcessWorker.h"
#include "simpleregex.h"
#include <re2/re2.h>
#include <hs.h>

//*
// * This is the function that will be called for each match that occurs. @a ctx
// * is to allow you to have some application-specific state that you will get
// * access to for each match. In our simple example we're just going to use it
// * to pass in the pattern that was being searched for so we can print it out.


static int eventHandler(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *ctx)
{
    //std::cout << "Match for pattern \"" << (char *)ctx << "\" at (" << from << " : " << to << ")" << std::endl;

    std::int64_t* context = (std::int64_t*)ctx;
    context[0] += (context[1] - context[2] + 1);
    return 0;
}

std::int64_t FileProcessWorker::processFile(FileContext* currentContext, uchar* sourceMemory, uchar* resultMemory, std::int64_t bytesMapped, std::int64_t processedBytes, std::int64_t totalSize)
{
    RE2 filter_regex("(" + currentContext->filterLineEdit->text().toStdString() + ")");

    std::int64_t i_prev = 0;

    std::int64_t* context = new std::int64_t[3];

    std::int64_t* matchCounter = context;
    *matchCounter = 0;

    hs_database_t* database;
    hs_compile_error* compile_err;

    char* pattern = (char*)currentContext->filterLineEdit->text().toStdString().c_str();

    if (hs_compile(pattern, HS_FLAG_DOTALL, HS_MODE_BLOCK, NULL, &database,
                   &compile_err) != HS_SUCCESS) {
        std::cout << "ERROR: Unable to compile pattern \"" << pattern << "\" : " << compile_err->message << std::endl;
        hs_free_compile_error(compile_err);
        return -1;
    }

    char* input_data;
    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(database, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(database);
        return -1;
    }

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

                std::copy(sourceMemory + i, sourceMemory + j + 1, resultMemory + *matchCounter);

                input_data = (char*)(resultMemory + *matchCounter);

                context[1] = j;
                context[2] = i;

                if (hs_scan(database, input_data, j - i, 0, scratch, eventHandler, context) != HS_SUCCESS)
                {
                    fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
                    hs_free_scratch(scratch);
                    free(input_data);
                    hs_free_database(database);
                    return -1;
                }

                //matchCounter += (j - i + 1) * RE2::PartialMatch((char*)(resultMemory + matchCounter), filter_regex, (void*)NULL);

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


    std::int64_t matchCounterSave = *matchCounter;
    delete [] context;
    return matchCounterSave;
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
