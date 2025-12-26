/********************************************************************************
** Form generated from reading UI file 'cupdateposition.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUPDATEPOSITION_H
#define UI_CUPDATEPOSITION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CUpdatePosition
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_latitude;
    QLabel *label_2;
    QLineEdit *lineEdit_longitude;
    QLabel *label_3;
    QLineEdit *lineEdit_altitude;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_update;
    QPushButton *pushButton_close;

    void setupUi(QWidget *CUpdatePosition)
    {
        if (CUpdatePosition->objectName().isEmpty())
            CUpdatePosition->setObjectName(QString::fromUtf8("CUpdatePosition"));
        CUpdatePosition->resize(287, 193);
        CUpdatePosition->setStyleSheet(QString::fromUtf8("/* ===============================\n"
"   Main Widget Background\n"
"   =============================== */\n"
"QWidget {\n"
"    background: qlineargradient(\n"
"        x1:0, y1:0, x2:0, y2:1,\n"
"        stop:0 #f2f4f7,\n"
"        stop:1 #d9dee4\n"
"    );\n"
"    font-family: Segoe UI, Roboto, sans-serif;\n"
"    font-size: 14px;\n"
"    color: #1E1E1E;\n"
"}\n"
"\n"
"/* ===============================\n"
"   Labels\n"
"   =============================== */\n"
"QLabel {\n"
"    color: #2c3e50;\n"
"    font-size: 14px;\n"
"    font-weight: 800;\n"
"}\n"
"\n"
"/* ===============================\n"
"   Line Edits\n"
"   =============================== */\n"
"QLineEdit {\n"
"    background: #ffffff;\n"
"    border: 1px solid #b6bec7;\n"
"    padding: 6px 8px;\n"
"    border-radius: 6px;\n"
"    selection-background-color: #0078d7;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 1px solid #0078d7;\n"
"    background: #fefefe;\n"
"}\n"
"\n"
"/* ===============================\n"
""
                        "   Buttons\n"
"   =============================== */\n"
"QPushButton {\n"
"    background-color: #3478f6;\n"
"    color: white;\n"
"    padding: 8px 14px;\n"
"    border-radius: 6px;\n"
"    border: none;\n"
"    font-weight: 800;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #2d6bdd;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #255bc0;\n"
"}\n"
"\n"
"/* Close Button (Optional Different Color) */\n"
"QPushButton#closeButton {\n"
"    background-color: #e74c3c;\n"
"}\n"
"\n"
"QPushButton#closeButton:hover {\n"
"    background-color: #cf4436;\n"
"}\n"
"\n"
"QPushButton#closeButton:pressed {\n"
"    background-color: #b83c30;\n"
"}"));
        gridLayout = new QGridLayout(CUpdatePosition);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(-1, 5, -1, 5);
        label = new QLabel(CUpdatePosition);
        label->setObjectName(QString::fromUtf8("label"));
        label->setStyleSheet(QString::fromUtf8("background:none;"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_latitude = new QLineEdit(CUpdatePosition);
        lineEdit_latitude->setObjectName(QString::fromUtf8("lineEdit_latitude"));

        gridLayout->addWidget(lineEdit_latitude, 0, 1, 1, 1);

        label_2 = new QLabel(CUpdatePosition);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setStyleSheet(QString::fromUtf8("background:none;"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_longitude = new QLineEdit(CUpdatePosition);
        lineEdit_longitude->setObjectName(QString::fromUtf8("lineEdit_longitude"));

        gridLayout->addWidget(lineEdit_longitude, 1, 1, 1, 1);

        label_3 = new QLabel(CUpdatePosition);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAutoFillBackground(false);
        label_3->setStyleSheet(QString::fromUtf8("background:none;"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        lineEdit_altitude = new QLineEdit(CUpdatePosition);
        lineEdit_altitude->setObjectName(QString::fromUtf8("lineEdit_altitude"));

        gridLayout->addWidget(lineEdit_altitude, 2, 1, 1, 1);

        widget = new QWidget(CUpdatePosition);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setStyleSheet(QString::fromUtf8("QWidget#widget{background:none;}"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_update = new QPushButton(widget);
        pushButton_update->setObjectName(QString::fromUtf8("pushButton_update"));

        horizontalLayout->addWidget(pushButton_update);

        pushButton_close = new QPushButton(widget);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));

        horizontalLayout->addWidget(pushButton_close);


        gridLayout->addWidget(widget, 3, 0, 1, 2);


        retranslateUi(CUpdatePosition);

        QMetaObject::connectSlotsByName(CUpdatePosition);
    } // setupUi

    void retranslateUi(QWidget *CUpdatePosition)
    {
        CUpdatePosition->setWindowTitle(QCoreApplication::translate("CUpdatePosition", "Form", nullptr));
        label->setText(QCoreApplication::translate("CUpdatePosition", "LATITUDE :", nullptr));
        label_2->setText(QCoreApplication::translate("CUpdatePosition", "LONGITUDE :", nullptr));
        label_3->setText(QCoreApplication::translate("CUpdatePosition", "ALTITUDE :", nullptr));
        pushButton_update->setText(QCoreApplication::translate("CUpdatePosition", "Update", nullptr));
        pushButton_close->setText(QCoreApplication::translate("CUpdatePosition", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CUpdatePosition: public Ui_CUpdatePosition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUPDATEPOSITION_H
