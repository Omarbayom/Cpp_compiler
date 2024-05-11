/********************************************************************************
** Form generated from reading UI file 'sample_table.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAMPLE_TABLE_H
#define UI_SAMPLE_TABLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_sample_table
{
public:

    void setupUi(QDialog *sample_table)
    {
        if (sample_table->objectName().isEmpty())
            sample_table->setObjectName("sample_table");
        sample_table->resize(400, 300);

        retranslateUi(sample_table);

        QMetaObject::connectSlotsByName(sample_table);
    } // setupUi

    void retranslateUi(QDialog *sample_table)
    {
        sample_table->setWindowTitle(QCoreApplication::translate("sample_table", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class sample_table: public Ui_sample_table {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAMPLE_TABLE_H
