#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainTextEdit = this->centralWidget()->findChild<QTextEdit*>("mainTextEdit");
    openFileButton = this->centralWidget()->findChild<QPushButton*>("openFileButton");
    tabWidget = this->centralWidget()->findChild<QTabWidget*>("tabWidget");
    connect(openFileButton, &QPushButton::released, this, &MainWindow::openFile);
}

void MainWindow::updateProgressBar(int value, QLineEdit* lineEdit)
{
    QString color = value == 100.0 ? "#41f970" : "#f99e41";
    QPalette palette = lineEdit->palette();
    QRectF qrect = QRectF(lineEdit->rect());
    QLinearGradient gradient = QLinearGradient(qrect.topLeft(), qrect.topRight());
    gradient.setColorAt((double)value / 100.0 - 0.001, QColor(color));
    gradient.setColorAt((double)value / 100.0, QColor("#ffffff"));
    gradient.setColorAt((double)value / 100.0 + 0.001, QColor("#ffffff"));
    palette.setBrush(QPalette::Base, QBrush(gradient));
    lineEdit->setPalette(palette);
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

        QWidget* tabPage = new QWidget();
        tabPage->setLayout(tabLayout);

        tabLayout->addWidget(tabFilterTextEdit, 0, 0);
        tabLayout->addWidget(searchButton, 0, 1);
        tabLayout->addWidget(tabFileTextEdit, 1, 0, 1, 2);
        tabLayout->setRowStretch(0, 15);
        tabLayout->setRowStretch(1, 100);

        tabWidget->addTab(tabPage, splittedName.constLast());

        FileContext* newFileContext = new FileContext();
        newFileContext->tabTextEdit = tabFileTextEdit;
        newFileContext->filterTextEdit = tabFilterTextEdit;
        newFileContext->sourceFilePath = fileName.toStdString();

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
