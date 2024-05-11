#ifndef PARSETREE_H
#define PARSETREE_H
#include <QDialog>
#include "comp.h"
namespace Ui {
class ParseTree;
}

class ParseTree : public QDialog
{
    Q_OBJECT

public:
    explicit ParseTree(LexicalAnalyzer& lex,QWidget *parent = nullptr);
    ~ParseTree();

protected:
    void showEvent(QShowEvent *event) override;


private:
    LexicalAnalyzer l;
    Ui::ParseTree *ui;
    void setupTable();
};

#endif // PARSETREE_H
