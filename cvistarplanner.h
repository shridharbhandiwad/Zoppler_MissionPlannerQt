#ifndef CVISTARPLANNER_H
#define CVISTARPLANNER_H

#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include "cnetworkinterface.h"
#include "cscenariomanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CVistarPlanner;
}
QT_END_NAMESPACE

class CVistarPlanner : public QMainWindow
{
    Q_OBJECT

public:
    CVistarPlanner(QWidget *parent = nullptr);
    ~CVistarPlanner();

private:
    Ui::CVistarPlanner *ui;
    void selectForMarking( int nClass );

    CNetworkInterface *_m_networkInterface;
    CScenarioManager *_m_scenarioManager;
    
    // Scenario menu button
    QPushButton *_m_scenarioButton;
    QMenu *_m_scenarioMenu;
    
    // Helper methods for scenario management
    void setupScenarioMenu();
    void loadScenarioToCanvas(const Scenario &scenario);
    Scenario getCurrentScenarioFromCanvas();

private slots :
    void slotMouseRead( QString mouseRead );
    void on_pushButton_Mark_clicked();
    void slotClearObjectSelection();
    void on_pushButton_Initialize_clicked();
    void on_pushButton_Start_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_ImportMaps_clicked();
    
    // Scenario management slots
    void onLoadScenario();
    void onSaveScenario();
    void onScenarioLoaded(const Scenario &scenario);
    void onScenarioSaved(const QString &filePath);
    void onScenarioError(const QString &message);
};
#endif // CVISTARPLANNER_H
