#include <QThread>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "filecontext.h"
#include "fileProcessWorker.h"

FileContext::FileContext(
    std::string sourceFilePath,
    QLineEdit* filterTextEdit,
    QTextEdit* tabTextEdit,
    QThread* workerThread,
    QSpinBox* pageSpinBox,
    FileProcessWorker* worker)
{
    this->sourceFilePath = sourceFilePath;
    this->filterTextEdit = filterTextEdit;
    this->tabTextEdit = tabTextEdit;
    this->workerThread = workerThread;
    this->pageSpinBox = pageSpinBox;
    this->worker = worker;

    this->getLineLength();
}

void FileContext::search()
{
    QString resultFileName = QFileDialog::getSaveFileName();

    this->worker->fileContext = this;
    this->worker->resultFileName = resultFileName;
    this->worker->moveToThread(this->workerThread);
    this->workerThread->start();
}

void FileContext::getLineLength()
{
    std::ifstream sourceFile(this->sourceFilePath);
    std::string line;
    unsigned int sizeSum = 0;
    int i = 0;
    for (i; i < 10 && getline(sourceFile, line); i++)
    {
        line.size() ? sizeSum += line.size() : i--;
    }

    this->lineLength = sizeSum / i;

    sourceFile.close();
}

void FileContext::getPage()
{
    std::cout << this->pageSpinBox->value() << std::endl;
    std::cout << this->lineLength << std::endl;

    std::ifstream sourceFile(this->sourceFilePath);

    unsigned int value = this->pageSpinBox->value() - 1;

    this->page = value + 1;

    if (value < 0 || value * this->pageSize >= std::filesystem::file_size(this->sourceFilePath))
    {
        std::cout << "Page with given number doesn't exist." << std::endl;
        this->tabTextEdit->clear();
        sourceFile.close();
        return;
    }

    sourceFile.seekg(value * this->pageSize);

    for (std::int64_t f = value * this->pageSize; f > 0 && sourceFile.peek() != '\n' && sourceFile.peek() != '\r'; f--)
    {
        sourceFile.seekg(f);
    }

    std::string line;

    this->tabTextEdit->clear();

    this->pageStartPos = sourceFile.tellg();

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));
    }

    this->pageEndPos = sourceFile.tellg();

    sourceFile.close();
}

void FileContext::getPrevPage()
{
    std::cout << "Page(prev): " << this->page << std::endl;

    std::ifstream sourceFile(this->sourceFilePath);

    if (this->page <= 1)
    {
        sourceFile.close();
        return;
    }

    this->page--;

    this->pageSpinBox->setValue(this->page);

    sourceFile.seekg(this->pageEndPos);

    std::string line;

    this->tabTextEdit->clear();

    int counter = 0;

    for (int i = this->pageStartPos; i >= 0 && counter <= this->pageSize + 1; i--)
    {
        sourceFile.seekg(i);
        if (sourceFile.peek() == '\n')
        {
            counter++;
        }
    }

    this->pageEndPos = this->pageStartPos;

    this->pageStartPos = sourceFile.tellg();

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));
    }

    this->pageEndPos = sourceFile.tellg();

    sourceFile.close();
}

void FileContext::getNextPage()
{
    std::cout << "Page(next): " << this->page << std::endl;
    std::ifstream sourceFile(this->sourceFilePath);

    this->page++;

    this->pageSpinBox->setValue(this->page);

    sourceFile.seekg(this->pageEndPos);

    std::string line;

    this->tabTextEdit->clear();

    this->pageStartPos = this->pageEndPos;

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));
    }

    this->pageEndPos = sourceFile.tellg();

    sourceFile.close();
}
