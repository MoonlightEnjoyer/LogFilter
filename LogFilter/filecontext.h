#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>
#include <vector>
#include <QString>
#include <string>
#include <QProgressBar>
#include <iostream>
#include <fstream>
#include <QString>
#include <string>
#include "fileProcessWorker.h"
#include <QThread>
#include <iostream>
#include <QString>
#include <string>
#include <QWidget>
#include <QThread>
#include <filesystem>
#include "simpleregex.h"
#include "mainwindow.h"

class FileProcessWorker;

class FileContext : public QObject
{
    Q_OBJECT
public:
    std::string sourceFilePath;
    QTextEdit* filterTextEdit;
    QTextEdit* tabTextEdit;
    QProgressBar* progressBar;
    QThread* workerThread;
    FileProcessWorker* worker;

public slots:
    void search();
};

#endif // FILECONTEXT_H
