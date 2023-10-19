#include <QThread>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "filecontext.h"
#include "fileProcessWorker.h"

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

    std::int64_t value = this->pageSpinBox->value() - 1;

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

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));
    }

}
