#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comp.h"
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QStackedWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void saveFile();
    void compileButtonClicked();
    void switchPage();
    void switchToParse();

private:
    Ui::MainWindow *ui;
    LexicalAnalyzer l; // Instance of LexicalAnalyzer
    QTextEdit *fileWidget;
    QTableWidget *table;

    void setupTable();
};
#endif // MAINWINDOW_H
