#include <QThread>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "filecontext.h"
#include "fileProcessWorker.h"

FileContext::FileContext(
    std::string sourceFilePath,
    QLineEdit* filterLineEdit,
    QTextEdit* tabTextEdit,
    QThread* workerThread,
    QSpinBox* pageSpinBox,
    FileProcessWorker* worker)
{
    this->sourceFilePath = sourceFilePath;
    this->filterLineEdit = filterLineEdit;
    this->tabTextEdit = tabTextEdit;
    this->workerThread = workerThread;
    this->pageSpinBox = pageSpinBox;
    this->worker = worker;

    this->getLineLength();
}

FileContext::~FileContext()
{
    this->workerThread->requestInterruption();
    this->workerThread->wait();
    delete this->filterLineEdit;
    delete this->tabTextEdit;
    delete this->workerThread;
    delete this->pageSpinBox;
    delete this->worker;
}

void FileContext::search()
{
    QString resultFileName = QFileDialog::getSaveFileName();

    if (resultFileName.length() > 0)
    {
        this->worker->fileContext = this;
        this->worker->resultFileName = resultFileName;
        this->worker->moveToThread(this->workerThread);
        this->workerThread->start();
    }
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
    std::ifstream sourceFile(this->sourceFilePath);

    unsigned int value = this->pageSpinBox->value() - 1;

    std::int64_t startPos = (std::int64_t)value * (std::int64_t)this->pageSize * (std::int64_t)this->lineLength;

    if (value < 0 || startPos >= std::filesystem::file_size(this->sourceFilePath))
    {
        this->pageSpinBox->setValue(this->page);
        sourceFile.close();
        return;
    }

    this->page = value + 1;

    sourceFile.seekg(startPos);

    for (std::int64_t f = startPos; f > 0 && sourceFile.peek() != '\n' && sourceFile.peek() != '\r'; f--)
    {
        sourceFile.seekg(f);
    }

    this->tabTextEdit->clear();
    this->pageStartPos = sourceFile.tellg();

    std::string line;

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));
    }

    this->pageEndPos = sourceFile.tellg();

    sourceFile.close();
}

void FileContext::getPrevPage()
{
    std::ifstream sourceFile(this->sourceFilePath);

    if (this->page <= 1)
    {
        sourceFile.close();
        return;
    }

    this->page--;
    this->pageSpinBox->setValue(this->page);
    this->tabTextEdit->clear();

    int counter = 0;
    std::string line;

    for (std::int64_t i = this->pageStartPos; i >= 0 && counter <= this->pageSize + 1; i--)
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
    std::ifstream sourceFile(this->sourceFilePath);

    QString previousText = this->tabTextEdit->toPlainText();

    this->tabTextEdit->clear();

    sourceFile.seekg(this->pageEndPos);

    std::string line;

    for (int i = 0; i < this->pageSize && getline(sourceFile, line); i++)
    {
        this->tabTextEdit->append(QString::fromStdString(line));

    }

    if (this->tabTextEdit->toPlainText().size() != 0)
    {
        this->page++;
        this->pageSpinBox->setValue(this->page);
        this->pageStartPos = this->pageEndPos;
        this->pageEndPos = sourceFile.tellg();
    }
    else
    {
        this->tabTextEdit->append(previousText);
    }

    sourceFile.close();
}

void FileContext::getLastPage()
{
    std::ifstream sourceFile(this->sourceFilePath);

    this->pageStartPos = std::filesystem::file_size(this->sourceFilePath);
    this->page = this->pageStartPos / (this->pageSize * this->lineLength);
    this->pageSpinBox->setValue(this->page);
    this->tabTextEdit->clear();

    int counter = 0;
    std::string line;

    for (std::int64_t i = this->pageStartPos; i >= 0 && counter <= this->pageSize + 1; i--)
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

    sourceFile.close();
}
