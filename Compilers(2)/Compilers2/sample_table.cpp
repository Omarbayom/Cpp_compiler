#include "sample_table.h"
#include "ui_sample_table.h"
#include <QVBoxLayout>

sample_table::sample_table(LexicalAnalyzer& lex, QWidget *parent)
    : QDialog(parent)
    , l(lex),
    ui(new Ui::sample_table)// Initialize l with the passed LexicalAnalyzer pointer
{
    ui->setupUi(this);
    setupTable();
}


sample_table::~sample_table()
{
    delete ui;
}

void sample_table::setupTable(){
    // Create the table widget
    table = new QTableWidget(this);
    table->setObjectName("tablewidget"); // Set the object name
    table->setRowCount(l.Tokens.size());
    table->setColumnCount(3); // Changed to 3 columns
    table->setHorizontalHeaderLabels(QStringList() << "Token ID" << "Token Value" << "Level");

    // Loop through the tokens and populate the table
    for (size_t row = 0; row < l.Tokens.size(); ++row)
    {
        QTableWidgetItem *tokenIDItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].id));
        QTableWidgetItem *tokenValueItem = new QTableWidgetItem(QString::fromStdString(l.Tokens[row].type));
        QTableWidgetItem *lineNumberItem = new QTableWidgetItem(QString::number(l.Tokens[row].level)); // Convert int to QString
        table->setItem(row, 0, tokenIDItem);
        table->setItem(row, 1, tokenValueItem);
        table->setItem(row, 2, lineNumberItem);
    }

    table->setMaximumWidth(1000); // Set maximum width for the table
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);

    QPushButton *SymbolTableButton = new QPushButton("Back",this);
    connect(SymbolTableButton, &QPushButton::clicked, this, &sample_table::switchPage);

    layout->addWidget(SymbolTableButton);
    SymbolTableButton->setMaximumWidth(150);
}

void sample_table::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event); // Call the base class implementation
    showMaximized(); // Maximize the dialog when it's shown
}

void sample_table::switchPage() {
    hide(); // Hide the current window
    MainWindow *m = new MainWindow(); // Create a new instance of MainWindow
    m->show(); // Show the new MainWindow
}

