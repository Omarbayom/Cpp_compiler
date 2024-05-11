#include "parsetree.h"
#include "ui_parsetree.h"
#include <QSvgWidget>
#include <QVBoxLayout>
#include <QPixmap>

ParseTree::ParseTree(LexicalAnalyzer& lex, QWidget *parent)
    : QDialog(parent)
    , l(lex)
    , ui(new Ui::ParseTree)
{
    ui->setupUi(this);
    QPixmap pix ("C:\\Users\\User\\Desktop\\New folder\\parse_tree.svg");
    ui->label_pic->setPixmap(pix.scaled(1920,1080,Qt::KeepAspectRatio));
}

ParseTree::~ParseTree()
{
    delete ui;
}

void ParseTree::setupTable() {
    QString fileName = "D:\\Compilers(2)\\Compilers2\\Testfile.txt";
    QString fileContents = QString::fromStdString(l.readFileToString(fileName.toStdString()));
    l.lexicalAnalyze(fileContents.toStdString());
    l.Tokens.clear();
    l.lexicalAnalyze(fileContents.toStdString());
    l.level(l.Tokens);
    l.processTokens(l.Tokens);
    l.SymbolTable(l.Tokens);
    l.Pareser();
}



void ParseTree::showEvent(QShowEvent *event)
{
    // Call the base class implementation
    QDialog::showEvent(event);

    // Maximize the main window when it's shown
    this->showMaximized(); // or just showMaximized(); since 'this' is implied
}
