#include <QThread>
#include <QFileDialog>
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
