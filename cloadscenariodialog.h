#ifndef CLOADSCENARIODIALOG_H
#define CLOADSCENARIODIALOG_H

#include <QDialog>
#include <QString>
#include <QListWidget>
#include "cscenariomanager.h"

namespace Ui {
class CLoadScenarioDialog;
}

class CLoadScenarioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLoadScenarioDialog(CScenarioManager *scenarioManager, QWidget *parent = nullptr);
    ~CLoadScenarioDialog();

    QString getSelectedScenarioPath() const;
    bool shouldLoadSampleScenario() const;
    int getSelectedSampleScenarioIndex() const;

private slots:
    void on_pushButton_Load_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Browse_clicked();
    void on_listWidget_Scenarios_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_Samples_itemDoubleClicked(QListWidgetItem *item);
    void on_tabWidget_currentChanged(int index);

private:
    Ui::CLoadScenarioDialog *ui;
    CScenarioManager *_m_scenarioManager;
    QString _m_selectedScenarioPath;
    bool _m_loadSampleScenario;
    int _m_selectedSampleIndex;
    
    void loadAvailableScenarios();
    void loadSampleScenarios();
};

#endif // CLOADSCENARIODIALOG_H
