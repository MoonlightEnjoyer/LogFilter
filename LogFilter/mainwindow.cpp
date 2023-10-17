#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainTextEdit = this->centralWidget()->findChild<QTextEdit*>("mainTextEdit");
    openFileButton = this->centralWidget()->findChild<QPushButton*>("openFileButton");
    tabWidget = this->centralWidget()->findChild<QTabWidget*>("tabWidget");
    connect(openFileButton, &QPushButton::released, this, &MainWindow::openFile);
}

void MainWindow::updateProgressBar(int value, QProgressBar* progressBar)
{
    progressBar->setValue(value);
    progressBar->setFormat(QString::number(value) + " %");
}

void MainWindow::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();

    using namespace std;
    for (const auto& fileName : fileNames)
    {
        QTextEdit* tabFileTextEdit = new QTextEdit();
        tabFileTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        QLineEdit* tabFilterTextEdit = new QLineEdit();
        QStringList splittedName = fileName.split('/');
        QPushButton* searchButton = new QPushButton();
        searchButton->setText("Search");
        QGridLayout* tabLayout = new QGridLayout();
        QProgressBar* progressBar = new QProgressBar();

        QWidget* tabPage = new QWidget();
        tabPage->setLayout(tabLayout);

        tabLayout->addWidget(tabFilterTextEdit, 0, 0);
        tabLayout->addWidget(searchButton, 0, 1);
        tabLayout->addWidget(progressBar, 1, 0, 1, 2);
        tabLayout->addWidget(tabFileTextEdit, 2, 0, 1, 2);
        tabLayout->setRowStretch(0, 15);
        tabLayout->setRowStretch(1, 15);
        tabLayout->setRowStretch(2, 100);

        tabWidget->addTab(tabPage, splittedName.constLast());

        FileContext* newFileContext = new FileContext();
        newFileContext->tabTextEdit = tabFileTextEdit;
        newFileContext->filterTextEdit = tabFilterTextEdit;
        newFileContext->sourceFilePath = fileName.toStdString();
        newFileContext->progressBar = progressBar;

        QThread* thread = new QThread();
        FileProcessWorker* worker = new FileProcessWorker();

        newFileContext->workerThread = thread;
        newFileContext->worker = worker;

        connect(thread, &QThread::started, worker, &FileProcessWorker::process);
        connect(worker, &FileProcessWorker::progress, this, &MainWindow::updateProgressBar);
        connect(searchButton, &QPushButton::released, newFileContext, &FileContext::search);

        ifstream file;
        file.open(fileName.toStdString());
        string line;

        if (file.is_open())
        {
            for (int i = 0; i < 5 && getline(file, line); i++)
            {
                tabFileTextEdit->append(QString::fromStdString(line));
            }

            file.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
