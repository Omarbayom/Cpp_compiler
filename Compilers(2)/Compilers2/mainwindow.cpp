#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSizePolicy>
#include <QFile>
#include "sample_table.h"
#include "parsetree.h"

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
    l.SymbolTable(l.Tokens);
    // Create the table widget
    table = new QTableWidget(this);
    table->setObjectName("tablewidget"); // Set the object name
    table->setRowCount(l.Symb.size());
    table->setColumnCount(3); // Changed to 3 columns
    table->setHorizontalHeaderLabels(QStringList() << "Token Value" << "Token ID" << "Number");

    for (size_t  row = 0; row < l.Symb.size(); ++row)
    {
        QTableWidgetItem *tokenIDItem = new QTableWidgetItem(QString::fromStdString(l.Symb[row].id));
        QTableWidgetItem *tokenValueItem = new QTableWidgetItem(QString::fromStdString(l.Symb[row].type));
        QTableWidgetItem *lineNumberItem = new QTableWidgetItem(QString::number(row + 1)); // Line number starts from 1
        // Set font size for each item
        QFont font = tokenIDItem->font();
        font.setPointSize(15); // Set the desired font size (12 in this example)
        tokenIDItem->setFont(font);
        tokenValueItem->setFont(font);
        lineNumberItem->setFont(font);
        table->setItem(row, 0, tokenIDItem);
        table->setItem(row, 1, tokenValueItem);
        table->setItem(row, 2, lineNumberItem);
    }
    table->setMaximumWidth(600); // Set maximum width for the table



    // Create a QTextEdit widget to display the file content

    fileWidget = new QTextEdit(this); // Assign to member variable
    fileWidget->setPlainText(fileContents);
    fileWidget->setMaximumWidth(600);
    QFont font = fileWidget->font(); // Get the current font
    font.setPointSize(19); // Set the desired font size (14 in this example)
    fileWidget->setFont(font); // Apply the new font to the QTextEdit

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

    QPushButton *SymbolTableButton = new QPushButton("Symbol Table",this);
    connect(SymbolTableButton, &QPushButton::clicked, this, &MainWindow::switchPage);


    // Add the switchPageButton to the layout
    layout->addWidget(SymbolTableButton);
    SymbolTableButton->setMaximumWidth(150);

    QPushButton *ParseTreeButton = new QPushButton("Parse Tree",this);
    connect(ParseTreeButton, &QPushButton::clicked, this, &MainWindow::switchToParse);

    // Add the switchPageButton to the layout
    layout->addWidget(ParseTreeButton);
    ParseTreeButton->setMaximumWidth(150);
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
    l.Symb.clear();

    l.lexicalAnalyze(fileContents.toStdString());
    table->clearContents();
    l.SymbolTable(l.Tokens);
    table->setRowCount(l.Symb.size());
    for (int row = 0; row < l.Symb.size(); ++row)
    {
        QTableWidgetItem *tokenIDItem = new QTableWidgetItem(QString::fromStdString(l.Symb[row].id));
        QTableWidgetItem *tokenValueItem = new QTableWidgetItem(QString::fromStdString(l.Symb[row].type));
        QTableWidgetItem *lineNumberItem = new QTableWidgetItem(QString::number(row + 1)); // Line number starts from 1
        table->setItem(row, 0, tokenIDItem);
        table->setItem(row, 1, tokenValueItem);
        table->setItem(row, 2, lineNumberItem);
    }
    saveFile();
}

void MainWindow::switchPage() {
    QString fileName = "D:\\Compilers(2)\\Compilers2\\Testfile.txt";
    QString fileContents = QString::fromStdString(l.readFileToString(fileName.toStdString()));
    l.Tokens.clear();

    l.lexicalAnalyze(fileContents.toStdString());
    table->clearContents();
    hide();
    sample_table* s = new sample_table(l, this); // Pass the instance to sample_table
    s->setModal(true);
    s->exec();
}

void MainWindow::switchToParse() {
    hide();
    ParseTree* p = new ParseTree(l,this); // Pass the instance to sample_table
    p->setModal(true);
    p->exec();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    this->showMaximized();
}





