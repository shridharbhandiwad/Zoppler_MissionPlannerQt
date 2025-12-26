#ifndef CSAVESCENARIODIALOG_H
#define CSAVESCENARIODIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class CSaveScenarioDialog;
}

class CSaveScenarioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSaveScenarioDialog(QWidget *parent = nullptr);
    ~CSaveScenarioDialog();

    QString getScenarioName() const;
    QString getScenarioDescription() const;
    QString getFilePath() const;

private slots:
    void on_pushButton_Save_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_BrowseLocation_clicked();
    void on_lineEdit_Name_textChanged(const QString &text);

private:
    Ui::CSaveScenarioDialog *ui;
    QString _m_selectedDirectory;
    
    bool validateInput();
};

#endif // CSAVESCENARIODIALOG_H
