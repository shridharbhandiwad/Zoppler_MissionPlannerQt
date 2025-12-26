/********************************************************************************
** Form generated from reading UI file 'cvistarplanner.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CVISTARPLANNER_H
#define UI_CVISTARPLANNER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "MapDisplay/cmapcanvas.h"

QT_BEGIN_NAMESPACE

class Ui_CVistarPlanner
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    CMapCanvas *mapCanvas;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QComboBox *comboBox_ObjectSelection;
    QPushButton *pushButton_Mark;
    QWidget *widget_2;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_Initialize;
    QPushButton *pushButton_Start;
    QPushButton *pushButton_Stop;
    QPushButton *pushButton_ImportMaps;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CVistarPlanner)
    {
        if (CVistarPlanner->objectName().isEmpty())
            CVistarPlanner->setObjectName(QString::fromUtf8("CVistarPlanner"));
        CVistarPlanner->resize(1230, 705);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        CVistarPlanner->setPalette(palette);
        centralwidget = new QWidget(CVistarPlanner);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        mapCanvas = new CMapCanvas(centralwidget);
        mapCanvas->setObjectName(QString::fromUtf8("mapCanvas"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush);
        mapCanvas->setPalette(palette1);
        widget = new QWidget(mapCanvas);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(370, 0, 331, 61));
        widget->setAutoFillBackground(false);
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        comboBox_ObjectSelection = new QComboBox(widget);
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->addItem(QString());
        comboBox_ObjectSelection->setObjectName(QString::fromUtf8("comboBox_ObjectSelection"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox_ObjectSelection->sizePolicy().hasHeightForWidth());
        comboBox_ObjectSelection->setSizePolicy(sizePolicy);
        comboBox_ObjectSelection->setMinimumSize(QSize(0, 0));
        QPalette palette2;
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        comboBox_ObjectSelection->setPalette(palette2);
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        comboBox_ObjectSelection->setFont(font);
        comboBox_ObjectSelection->setAutoFillBackground(false);
        comboBox_ObjectSelection->setStyleSheet(QString::fromUtf8("QPushButton {background-color:rgb(0,0,255)};"));

        gridLayout_2->addWidget(comboBox_ObjectSelection, 0, 1, 1, 1);

        pushButton_Mark = new QPushButton(widget);
        pushButton_Mark->setObjectName(QString::fromUtf8("pushButton_Mark"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton_Mark->sizePolicy().hasHeightForWidth());
        pushButton_Mark->setSizePolicy(sizePolicy1);
        pushButton_Mark->setFont(font);
        pushButton_Mark->setAutoFillBackground(false);
        pushButton_Mark->setStyleSheet(QString::fromUtf8("QPushButton {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2196F3, stop:1 #1976D2); border: none;color: white; padding: 10px 20px;border-radius: 4px;}\n"
"    QPushButton:hover {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #42A5F5, stop:1 #1E88E5);}\n"
" QPushButton:pressed {background-color: #1565C0;}"));

        gridLayout_2->addWidget(pushButton_Mark, 0, 2, 1, 1);

        widget_2 = new QWidget(mapCanvas);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(780, 0, 411, 61));
        gridLayout_3 = new QGridLayout(widget_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        pushButton_Initialize = new QPushButton(widget_2);
        pushButton_Initialize->setObjectName(QString::fromUtf8("pushButton_Initialize"));
        sizePolicy1.setHeightForWidth(pushButton_Initialize->sizePolicy().hasHeightForWidth());
        pushButton_Initialize->setSizePolicy(sizePolicy1);
        pushButton_Initialize->setFont(font);
        pushButton_Initialize->setAutoFillBackground(false);
        pushButton_Initialize->setStyleSheet(QString::fromUtf8("QPushButton {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFD54F, stop:1 #FBC02D); border: none;color: white; padding: 10px 20px;border-radius: 4px;color:black;}\n"
"    QPushButton:hover {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFE082, stop:1 #FDD835);}\n"
" QPushButton:pressed {background-color: #F9A825;}"));

        gridLayout_3->addWidget(pushButton_Initialize, 0, 0, 1, 1);

        pushButton_Start = new QPushButton(widget_2);
        pushButton_Start->setObjectName(QString::fromUtf8("pushButton_Start"));
        sizePolicy1.setHeightForWidth(pushButton_Start->sizePolicy().hasHeightForWidth());
        pushButton_Start->setSizePolicy(sizePolicy1);
        pushButton_Start->setFont(font);
        pushButton_Start->setAutoFillBackground(false);
        pushButton_Start->setStyleSheet(QString::fromUtf8("QPushButton {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4CAF50, stop:1 #2E7D32); border: none;color: white; padding: 10px 20px;border-radius: 4px;}\n"
"    QPushButton:hover {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #66BB6A, stop:1 #388E3C);}\n"
" QPushButton:pressed {background-color: #1B5E20;}"));

        gridLayout_3->addWidget(pushButton_Start, 0, 1, 1, 1);

        pushButton_Stop = new QPushButton(widget_2);
        pushButton_Stop->setObjectName(QString::fromUtf8("pushButton_Stop"));
        sizePolicy1.setHeightForWidth(pushButton_Stop->sizePolicy().hasHeightForWidth());
        pushButton_Stop->setSizePolicy(sizePolicy1);
        pushButton_Stop->setFont(font);
        pushButton_Stop->setAutoFillBackground(false);
        pushButton_Stop->setStyleSheet(QString::fromUtf8("QPushButton {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E53935, stop:1 #C62828); border: none;color: white; padding: 10px 20px;border-radius: 4px;}\n"
"    QPushButton:hover {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #EF5350, stop:1 #D32F2F);}\n"
" QPushButton:pressed {background-color: #C62828;}"));

        gridLayout_3->addWidget(pushButton_Stop, 0, 2, 1, 1);

        pushButton_ImportMaps = new QPushButton(mapCanvas);
        pushButton_ImportMaps->setObjectName(QString::fromUtf8("pushButton_ImportMaps"));
        pushButton_ImportMaps->setGeometry(QRect(10, 10, 191, 51));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_ImportMaps->sizePolicy().hasHeightForWidth());
        pushButton_ImportMaps->setSizePolicy(sizePolicy2);
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        pushButton_ImportMaps->setFont(font1);
        pushButton_ImportMaps->setAutoFillBackground(false);
        pushButton_ImportMaps->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #9C27B0,\n"
"                                      stop:1 #7B1FA2);\n"
"    border: none;\n"
"    color: white;\n"
"    padding: 10px 20px;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #AB47BC,\n"
"                                      stop:1 #8E24AA);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #6A1B9A;\n"
"}"));

        gridLayout->addWidget(mapCanvas, 0, 0, 1, 1);

        CVistarPlanner->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(CVistarPlanner);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette3.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Base, brush);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush);
        QBrush brush2(QColor(255, 255, 255, 128));
        brush2.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::Active, QPalette::PlaceholderText, brush2);
#endif
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::ButtonText, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush2);
#endif
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::ButtonText, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush2);
#endif
        statusBar->setPalette(palette3);
        QFont font2;
        font2.setPointSize(15);
        font2.setBold(true);
        font2.setItalic(true);
        statusBar->setFont(font2);
        statusBar->setAutoFillBackground(true);
        statusBar->setStyleSheet(QString::fromUtf8("color:white"));
        CVistarPlanner->setStatusBar(statusBar);

        retranslateUi(CVistarPlanner);

        QMetaObject::connectSlotsByName(CVistarPlanner);
    } // setupUi

    void retranslateUi(QMainWindow *CVistarPlanner)
    {
        CVistarPlanner->setWindowTitle(QCoreApplication::translate("CVistarPlanner", "CVistarPlanner", nullptr));
        comboBox_ObjectSelection->setItemText(0, QString());
        comboBox_ObjectSelection->setItemText(1, QCoreApplication::translate("CVistarPlanner", "DRONE", nullptr));
        comboBox_ObjectSelection->setItemText(2, QCoreApplication::translate("CVistarPlanner", "DRONE_SWARM", nullptr));
        comboBox_ObjectSelection->setItemText(3, QCoreApplication::translate("CVistarPlanner", "FIGHTER", nullptr));
        comboBox_ObjectSelection->setItemText(4, QCoreApplication::translate("CVistarPlanner", "UAV", nullptr));
        comboBox_ObjectSelection->setItemText(5, QCoreApplication::translate("CVistarPlanner", "RADAR", nullptr));
        comboBox_ObjectSelection->setItemText(6, QCoreApplication::translate("CVistarPlanner", "LAUNCHER", nullptr));
        comboBox_ObjectSelection->setItemText(7, QCoreApplication::translate("CVistarPlanner", "MISSILE", nullptr));
        comboBox_ObjectSelection->setItemText(8, QCoreApplication::translate("CVistarPlanner", "ROUTE", nullptr));

        pushButton_Mark->setText(QCoreApplication::translate("CVistarPlanner", "Mark", nullptr));
        pushButton_Initialize->setText(QCoreApplication::translate("CVistarPlanner", "INITIALIZE", nullptr));
        pushButton_Start->setText(QCoreApplication::translate("CVistarPlanner", "START", nullptr));
        pushButton_Stop->setText(QCoreApplication::translate("CVistarPlanner", "STOP", nullptr));
        pushButton_ImportMaps->setText(QCoreApplication::translate("CVistarPlanner", "Import Map", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CVistarPlanner: public Ui_CVistarPlanner {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CVISTARPLANNER_H
