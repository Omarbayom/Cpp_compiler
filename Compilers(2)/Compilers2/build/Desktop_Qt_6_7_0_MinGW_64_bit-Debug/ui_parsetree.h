/********************************************************************************
** Form generated from reading UI file 'parsetree.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARSETREE_H
#define UI_PARSETREE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ParseTree
{
public:
    QLabel *label_pic;

    void setupUi(QDialog *ParseTree)
    {
        if (ParseTree->objectName().isEmpty())
            ParseTree->setObjectName("ParseTree");
        ParseTree->resize(1920, 812);
        label_pic = new QLabel(ParseTree);
        label_pic->setObjectName("label_pic");
        label_pic->setGeometry(QRect(-2, 5, 1921, 801));

        retranslateUi(ParseTree);

        QMetaObject::connectSlotsByName(ParseTree);
    } // setupUi

    void retranslateUi(QDialog *ParseTree)
    {
        ParseTree->setWindowTitle(QCoreApplication::translate("ParseTree", "Dialog", nullptr));
        label_pic->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ParseTree: public Ui_ParseTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARSETREE_H
