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
#include <regex>
#include <QProgressBar>
#include <iostream>
#include <fstream>

class FileContext : public QObject
{
    Q_OBJECT
public:
    std::string sourceFilePath;
    QTextEdit* filterTextEdit;
    QTextEdit* tabTextEdit;
    QProgressBar* progressBar;
public slots:
    void search();
    static void updateProgressBar(FileContext*);
};

#endif // FILECONTEXT_H
