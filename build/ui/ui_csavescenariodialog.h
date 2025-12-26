/********************************************************************************
** Form generated from reading UI file 'csavescenariodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSAVESCENARIODIALOG_H
#define UI_CSAVESCENARIODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CSaveScenarioDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_Title;
    QGroupBox *groupBox_ScenarioInfo;
    QFormLayout *formLayout;
    QLabel *label_Name;
    QLineEdit *lineEdit_Name;
    QLabel *label_Description;
    QTextEdit *textEdit_Description;
    QGroupBox *groupBox_SaveLocation;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_SaveLocation;
    QPushButton *pushButton_BrowseLocation;
    QLabel *label_Status;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Save;
    QPushButton *pushButton_Cancel;

    void setupUi(QDialog *CSaveScenarioDialog)
    {
        if (CSaveScenarioDialog->objectName().isEmpty())
            CSaveScenarioDialog->setObjectName(QString::fromUtf8("CSaveScenarioDialog"));
        CSaveScenarioDialog->resize(500, 400);
        verticalLayout = new QVBoxLayout(CSaveScenarioDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_Title = new QLabel(CSaveScenarioDialog);
        label_Title->setObjectName(QString::fromUtf8("label_Title"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        label_Title->setFont(font);
        label_Title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_Title);

        groupBox_ScenarioInfo = new QGroupBox(CSaveScenarioDialog);
        groupBox_ScenarioInfo->setObjectName(QString::fromUtf8("groupBox_ScenarioInfo"));
        formLayout = new QFormLayout(groupBox_ScenarioInfo);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_Name = new QLabel(groupBox_ScenarioInfo);
        label_Name->setObjectName(QString::fromUtf8("label_Name"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_Name);

        lineEdit_Name = new QLineEdit(groupBox_ScenarioInfo);
        lineEdit_Name->setObjectName(QString::fromUtf8("lineEdit_Name"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_Name);

        label_Description = new QLabel(groupBox_ScenarioInfo);
        label_Description->setObjectName(QString::fromUtf8("label_Description"));
        label_Description->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_Description);

        textEdit_Description = new QTextEdit(groupBox_ScenarioInfo);
        textEdit_Description->setObjectName(QString::fromUtf8("textEdit_Description"));
        textEdit_Description->setMaximumSize(QSize(16777215, 100));

        formLayout->setWidget(1, QFormLayout::FieldRole, textEdit_Description);


        verticalLayout->addWidget(groupBox_ScenarioInfo);

        groupBox_SaveLocation = new QGroupBox(CSaveScenarioDialog);
        groupBox_SaveLocation->setObjectName(QString::fromUtf8("groupBox_SaveLocation"));
        verticalLayout_3 = new QVBoxLayout(groupBox_SaveLocation);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineEdit_SaveLocation = new QLineEdit(groupBox_SaveLocation);
        lineEdit_SaveLocation->setObjectName(QString::fromUtf8("lineEdit_SaveLocation"));
        lineEdit_SaveLocation->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEdit_SaveLocation);

        pushButton_BrowseLocation = new QPushButton(groupBox_SaveLocation);
        pushButton_BrowseLocation->setObjectName(QString::fromUtf8("pushButton_BrowseLocation"));

        horizontalLayout_2->addWidget(pushButton_BrowseLocation);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox_SaveLocation);

        label_Status = new QLabel(CSaveScenarioDialog);
        label_Status->setObjectName(QString::fromUtf8("label_Status"));
        label_Status->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_Status);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_Save = new QPushButton(CSaveScenarioDialog);
        pushButton_Save->setObjectName(QString::fromUtf8("pushButton_Save"));
        pushButton_Save->setMinimumSize(QSize(100, 30));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        pushButton_Save->setFont(font1);
        pushButton_Save->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #2196F3,\n"
"                                      stop:1 #1976D2);\n"
"    border: none;\n"
"    color: white;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                      stop:0 #42A5F5,\n"
"                                      stop:1 #1E88E5);\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #1565C0;\n"
"}\n"
"QPushButton:disabled {\n"
"    background-color: #BDBDBD;\n"
"}"));

        horizontalLayout->addWidget(pushButton_Save);

        pushButton_Cancel = new QPushButton(CSaveScenarioDialog);
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


        retranslateUi(CSaveScenarioDialog);

        QMetaObject::connectSlotsByName(CSaveScenarioDialog);
    } // setupUi

    void retranslateUi(QDialog *CSaveScenarioDialog)
    {
        CSaveScenarioDialog->setWindowTitle(QCoreApplication::translate("CSaveScenarioDialog", "Save Scenario", nullptr));
        label_Title->setText(QCoreApplication::translate("CSaveScenarioDialog", "Save Current Scenario", nullptr));
        groupBox_ScenarioInfo->setTitle(QCoreApplication::translate("CSaveScenarioDialog", "Scenario Information", nullptr));
        label_Name->setText(QCoreApplication::translate("CSaveScenarioDialog", "Scenario Name:*", nullptr));
        lineEdit_Name->setPlaceholderText(QCoreApplication::translate("CSaveScenarioDialog", "Enter scenario name", nullptr));
        label_Description->setText(QCoreApplication::translate("CSaveScenarioDialog", "Description:", nullptr));
        textEdit_Description->setPlaceholderText(QCoreApplication::translate("CSaveScenarioDialog", "Enter optional description", nullptr));
        groupBox_SaveLocation->setTitle(QCoreApplication::translate("CSaveScenarioDialog", "Save Location", nullptr));
        pushButton_BrowseLocation->setText(QCoreApplication::translate("CSaveScenarioDialog", "Browse...", nullptr));
        label_Status->setText(QCoreApplication::translate("CSaveScenarioDialog", "Please enter a scenario name", nullptr));
        pushButton_Save->setText(QCoreApplication::translate("CSaveScenarioDialog", "Save", nullptr));
        pushButton_Cancel->setText(QCoreApplication::translate("CSaveScenarioDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CSaveScenarioDialog: public Ui_CSaveScenarioDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSAVESCENARIODIALOG_H
