#ifndef SAMPLE_TABLE_H
#define SAMPLE_TABLE_H

#include <QDialog>
#include "comp.h"
#include <QTableWidget>
#include <mainwindow.h>

namespace Ui {
class sample_table;
}

class sample_table : public QDialog
{
    Q_OBJECT

public:
    explicit sample_table(LexicalAnalyzer& lex,QWidget *parent = nullptr);
    ~sample_table();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void switchPage();

private:
    LexicalAnalyzer l;
    QTableWidget *table;
    Ui::sample_table *ui;
    void setupTable();
};

#endif // SAMPLE_TABLE_H
