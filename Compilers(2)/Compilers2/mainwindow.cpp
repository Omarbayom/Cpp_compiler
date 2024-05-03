#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSizePolicy>
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTable(); // Call the setupTable function
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    QString fileName = "D:\\Compilers(2)\\Compilers2\\Testfile.txt";
    QString fileContents = QString::fromStdString(l.readFileToString(fileName.toStdString()));
    l.lexicalAnalyze(fileContents.toStdString());

    // Create the table widget
    table = new QTableWidget(this);
    table->setObjectName("tablewidget"); // Set the object name
    table->setRowCount(l.Tokens.size());
    table->setColumnCount(3); // Changed to 3 columns
    table->setHorizontalHeaderLabels(QStringList() << "Token ID" << "Token Value" << "Line Number");

    for (size_t  row = 0; row < l.Tokens.size(); ++row)
    {
        QTableWidgetItem *tokenIDItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].id));
        QTableWidgetItem *tokenValueItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].type));
        QTableWidgetItem *lineNumberItem = new QTableWidgetItem(QString::number(row + 1)); // Line number starts from 1
        table->setItem(row, 0, tokenIDItem);
        table->setItem(row, 1, tokenValueItem);
        table->setItem(row, 2, lineNumberItem);
    }
    table->setMaximumWidth(400); // Set maximum width for the table



    // Create a QTextEdit widget to display the file content
    fileWidget = new QTextEdit(this); // Assign to member variable
    fileWidget->setPlainText(fileContents);
    fileWidget->setMaximumWidth(400);

    // Create a QVBoxLayout for the central widget
    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);

    // Create a QHBoxLayout for the file widget and table
    QHBoxLayout *innerLayout = new QHBoxLayout;
    innerLayout->addWidget(fileWidget); // Add the file widget
    innerLayout->addWidget(table); // Add the table


    layout->addLayout(innerLayout); // Add the inner layout to the central widget's layout

    // Add some spacing between the label and the table
    layout->addSpacing(20);

    connect(fileWidget, &QTextEdit::textChanged, this, &MainWindow::saveFile);

    QPushButton *compileButton = new QPushButton("Compile", this);
    connect(compileButton, &QPushButton::clicked, this, &MainWindow::compileButtonClicked);
    layout->addWidget(compileButton);
    compileButton->setMaximumWidth(150);
}


void MainWindow::saveFile()
{
    QString fileName = "D:\\Compilers(2)\\Compilers2\\Testfile.txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << fileWidget->toPlainText(); // Access fileWidget through member variable
        file.close();
    }
}

void MainWindow::compileButtonClicked()
{
    QString fileName = "D:\\Compilers(2)\\Compilers2\\Testfile.txt";
    QString fileContents = QString::fromStdString(l.readFileToString(fileName.toStdString()));
    l.Tokens.clear();

    l.lexicalAnalyze(fileContents.toStdString());
    table->clearContents();
    table->setRowCount(l.Tokens.size());
    for (int row = 0; row < l.Tokens.size(); ++row)
    {
        QTableWidgetItem *tokenIDItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].id));
        QTableWidgetItem *tokenValueItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].type));
        QTableWidgetItem *lineNumberItem = new QTableWidgetItem(QString::number(row + 1)); // Line number starts from 1
        table->setItem(row, 0, tokenIDItem);
        table->setItem(row, 1, tokenValueItem);
        table->setItem(row, 2, lineNumberItem);
    }
    saveFile();
}






