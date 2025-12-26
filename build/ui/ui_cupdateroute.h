/********************************************************************************
** Form generated from reading UI file 'cupdateroute.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUPDATEROUTE_H
#define UI_CUPDATEROUTE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CUpdateRoute
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_update;
    QPushButton *pushButton_close;

    void setupUi(QWidget *CUpdateRoute)
    {
        if (CUpdateRoute->objectName().isEmpty())
            CUpdateRoute->setObjectName(QString::fromUtf8("CUpdateRoute"));
        CUpdateRoute->resize(588, 288);
        CUpdateRoute->setStyleSheet(QString::fromUtf8("/* ===============================\n"
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
"\n"
"/* ===============================\n"
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
"/* Close Button (Optiona"
                        "l Different Color) */\n"
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
"}\n"
"\n"
"/* ===============================\n"
"   Table Widget Background + Border\n"
"   =============================== */\n"
"QTableWidget {\n"
"    background: #ffffff;\n"
"    border: 1px solid #b6bec7;\n"
"    border-radius: 6px;\n"
"    gridline-color: #d0d4d9;\n"
"    font-size: 14px;\n"
"    selection-background-color: #cfe3ff;\n"
"    selection-color: #000000;\n"
"}\n"
"\n"
"/* ===============================\n"
"   Table Header\n"
"   =============================== */\n"
"QHeaderView::section {\n"
"    background-color: #eef1f5;\n"
"    color: #2c3e50;\n"
"    padding: 6px 8px;\n"
"    border: 1px solid #d0d4d9;\n"
"    font-weight: 600;\n"
"    border-radius: 0px;\n"
"}\n"
"\n"
"/* Remove the small triangle indicator background */\n"
"QHeader"
                        "View::down-arrow,\n"
"QHeaderView::up-arrow {\n"
"    width: 10px;\n"
"    height: 10px;\n"
"}\n"
"\n"
"/* ===============================\n"
"   Alternating Row Colors\n"
"   =============================== */\n"
"QTableWidget::item {\n"
"    padding: 0px;\n"
"}\n"
"\n"
"/* Hover effect */\n"
"QTableWidget::item:hover {\n"
"    background-color: #ecf3ff;\n"
"}\n"
"\n"
"/* Selected row */\n"
"QTableWidget::item:selected {\n"
"    background-color: #cfe3ff;\n"
"    color: #000000;\n"
"}\n"
"\n"
"/* ===============================\n"
"   Scrollbars\n"
"   =============================== */\n"
"QScrollBar:vertical {\n"
"    width: 10px;\n"
"    background: #eef1f5;\n"
"    margin: 0px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #b6bec7;\n"
"    min-height: 30px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #9aa3ad;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical,\n"
"QScrollBar::sub-line:vertical {\n"
"    height:"
                        " 0px;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    height: 10px;\n"
"    background: #eef1f5;\n"
"    margin: 0px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: #b6bec7;\n"
"    min-width: 30px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #9aa3ad;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal,\n"
"QScrollBar::sub-line:horizontal {\n"
"    width: 0px;\n"
"}"));
        gridLayout = new QGridLayout(CUpdateRoute);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableWidget = new QTableWidget(CUpdateRoute);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QFont font;
        font.setBold(true);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setFont(font);
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setFont(font);
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        __qtablewidgetitem2->setFont(font);
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        if (tableWidget->rowCount() < 10)
            tableWidget->setRowCount(10);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setItem(0, 0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setItem(1, 0, __qtablewidgetitem4);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setStyleSheet(QString::fromUtf8(""));
        tableWidget->setAutoScroll(false);
        tableWidget->setAutoScrollMargin(16);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidget->setRowCount(10);
        tableWidget->horizontalHeader()->setVisible(true);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(false);
        tableWidget->horizontalHeader()->setMinimumSectionSize(41);
        tableWidget->horizontalHeader()->setDefaultSectionSize(150);
        tableWidget->horizontalHeader()->setHighlightSections(false);
        tableWidget->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(true);
        tableWidget->verticalHeader()->setCascadingSectionResizes(false);
        tableWidget->verticalHeader()->setMinimumSectionSize(40);
        tableWidget->verticalHeader()->setDefaultSectionSize(40);
        tableWidget->verticalHeader()->setHighlightSections(false);
        tableWidget->verticalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidget->verticalHeader()->setStretchLastSection(false);

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);

        widget = new QWidget(CUpdateRoute);
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


        gridLayout->addWidget(widget, 1, 0, 1, 1);


        retranslateUi(CUpdateRoute);

        QMetaObject::connectSlotsByName(CUpdateRoute);
    } // setupUi

    void retranslateUi(QWidget *CUpdateRoute)
    {
        CUpdateRoute->setWindowTitle(QCoreApplication::translate("CUpdateRoute", "Form", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("CUpdateRoute", "Lat", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("CUpdateRoute", "Lon", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("CUpdateRoute", "Alt", nullptr));

        const bool __sortingEnabled = tableWidget->isSortingEnabled();
        tableWidget->setSortingEnabled(false);
        tableWidget->setSortingEnabled(__sortingEnabled);

        pushButton_update->setText(QCoreApplication::translate("CUpdateRoute", "Update", nullptr));
        pushButton_close->setText(QCoreApplication::translate("CUpdateRoute", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CUpdateRoute: public Ui_CUpdateRoute {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUPDATEROUTE_H
