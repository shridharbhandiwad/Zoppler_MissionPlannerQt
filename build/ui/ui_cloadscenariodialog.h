/********************************************************************************
** Form generated from reading UI file 'cloadscenariodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOADSCENARIODIALOG_H
#define UI_CLOADSCENARIODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CLoadScenarioDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_Title;
    QTabWidget *tabWidget;
    QWidget *tab_Saved;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_SavedInfo;
    QListWidget *listWidget_Scenarios;
    QPushButton *pushButton_Browse;
    QWidget *tab_Samples;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_SamplesInfo;
    QListWidget *listWidget_Samples;
    QLabel *label_SampleDescription;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Load;
    QPushButton *pushButton_Cancel;

    void setupUi(QDialog *CLoadScenarioDialog)
    {
        if (CLoadScenarioDialog->objectName().isEmpty())
            CLoadScenarioDialog->setObjectName(QString::fromUtf8("CLoadScenarioDialog"));
        CLoadScenarioDialog->resize(600, 450);
        verticalLayout = new QVBoxLayout(CLoadScenarioDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_Title = new QLabel(CLoadScenarioDialog);
        label_Title->setObjectName(QString::fromUtf8("label_Title"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        label_Title->setFont(font);
        label_Title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_Title);

        tabWidget = new QTabWidget(CLoadScenarioDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_Saved = new QWidget();
        tab_Saved->setObjectName(QString::fromUtf8("tab_Saved"));
        verticalLayout_2 = new QVBoxLayout(tab_Saved);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_SavedInfo = new QLabel(tab_Saved);
        label_SavedInfo->setObjectName(QString::fromUtf8("label_SavedInfo"));

        verticalLayout_2->addWidget(label_SavedInfo);

        listWidget_Scenarios = new QListWidget(tab_Saved);
        listWidget_Scenarios->setObjectName(QString::fromUtf8("listWidget_Scenarios"));
        listWidget_Scenarios->setAlternatingRowColors(true);

        verticalLayout_2->addWidget(listWidget_Scenarios);

        pushButton_Browse = new QPushButton(tab_Saved);
        pushButton_Browse->setObjectName(QString::fromUtf8("pushButton_Browse"));

        verticalLayout_2->addWidget(pushButton_Browse);

        tabWidget->addTab(tab_Saved, QString());
        tab_Samples = new QWidget();
        tab_Samples->setObjectName(QString::fromUtf8("tab_Samples"));
        verticalLayout_3 = new QVBoxLayout(tab_Samples);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_SamplesInfo = new QLabel(tab_Samples);
        label_SamplesInfo->setObjectName(QString::fromUtf8("label_SamplesInfo"));

        verticalLayout_3->addWidget(label_SamplesInfo);

        listWidget_Samples = new QListWidget(tab_Samples);
        listWidget_Samples->setObjectName(QString::fromUtf8("listWidget_Samples"));
        listWidget_Samples->setAlternatingRowColors(true);

        verticalLayout_3->addWidget(listWidget_Samples);

        label_SampleDescription = new QLabel(tab_Samples);
        label_SampleDescription->setObjectName(QString::fromUtf8("label_SampleDescription"));
        label_SampleDescription->setWordWrap(true);

        verticalLayout_3->addWidget(label_SampleDescription);

        tabWidget->addTab(tab_Samples, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_Load = new QPushButton(CLoadScenarioDialog);
        pushButton_Load->setObjectName(QString::fromUtf8("pushButton_Load"));
        pushButton_Load->setMinimumSize(QSize(100, 30));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        pushButton_Load->setFont(font1);
        pushButton_Load->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                                      stop:0 #4CAF50, \n"
"                                      stop:1 #2E7D32);\n"
"    border: none;\n"
"    color: white;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #66BB6A,\n"
"                                      stop:1 #388E3C);\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #1B5E20;\n"
"}"));

        horizontalLayout->addWidget(pushButton_Load);

        pushButton_Cancel = new QPushButton(CLoadScenarioDialog);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setMinimumSize(QSize(100, 30));
        pushButton_Cancel->setFont(font1);
        pushButton_Cancel->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #9E9E9E,\n"
"                                      stop:1 #757575);\n"
"    border: none;\n"
"    color: white;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #BDBDBD,\n"
"                                      stop:1 #9E9E9E);\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #616161;\n"
"}"));

        horizontalLayout->addWidget(pushButton_Cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(CLoadScenarioDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CLoadScenarioDialog);
    } // setupUi

    void retranslateUi(QDialog *CLoadScenarioDialog)
    {
        CLoadScenarioDialog->setWindowTitle(QCoreApplication::translate("CLoadScenarioDialog", "Load Scenario", nullptr));
        label_Title->setText(QCoreApplication::translate("CLoadScenarioDialog", "Load Scenario", nullptr));
        label_SavedInfo->setText(QCoreApplication::translate("CLoadScenarioDialog", "Select a previously saved scenario:", nullptr));
        pushButton_Browse->setText(QCoreApplication::translate("CLoadScenarioDialog", "Browse for File...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Saved), QCoreApplication::translate("CLoadScenarioDialog", "Saved Scenarios", nullptr));
        label_SamplesInfo->setText(QCoreApplication::translate("CLoadScenarioDialog", "Select a pre-configured sample scenario:", nullptr));
        label_SampleDescription->setText(QCoreApplication::translate("CLoadScenarioDialog", "Hover over a sample to see its description", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Samples), QCoreApplication::translate("CLoadScenarioDialog", "Sample Scenarios", nullptr));
        pushButton_Load->setText(QCoreApplication::translate("CLoadScenarioDialog", "Load", nullptr));
        pushButton_Cancel->setText(QCoreApplication::translate("CLoadScenarioDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CLoadScenarioDialog: public Ui_CLoadScenarioDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOADSCENARIODIALOG_H
