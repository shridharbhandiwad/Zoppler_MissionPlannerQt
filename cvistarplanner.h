#ifndef CVISTARPLANNER_H
#define CVISTARPLANNER_H

#include <QMainWindow>
#include <QMenu>
#include <QTableWidgetItem>
#include "cnetworkinterface.h"
#include "cpathsettingsdialog.h"

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
    void setupToolbarMenus();
    void setupConnections();
    void updateCoordinateDisplay(double lat, double lon);
    void updateWaypointsTable();
    void addWaypointRow(int row, const QString &command, double lat, double lon, double alt, double dist, double az);
    void clearWaypointsTable();

    CNetworkInterface *_m_networkInterface;
    CPathSettingsDialog *_m_pathSettingsDialog;
    
    // Menus for toolbar buttons
    QMenu *_m_scenarioMenu;
    QMenu *_m_pathMenu;
    
    // Home location
    double _m_homeLat;
    double _m_homeLon;
    double _m_homeAlt;
    
    // Current cursor position
    double _m_currentLat;
    double _m_currentLon;

private slots :
    void slotMouseRead( QString mouseRead );
    void on_pushButton_Mark_clicked();
    void slotClearObjectSelection();
    void on_pushButton_Initialize_clicked();
    void on_pushButton_Start_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_ImportMaps_clicked();
    void on_pushButton_SaveScenario_clicked();
    void on_pushButton_LoadScenario_clicked();
    void on_pushButton_ResetScenario_clicked();
    void openPathSettings();
    
    // New slots for enhanced UI
    void onScenarioManagerTriggered();
    void onPathGeneratorTriggered();
    void onInitializeTriggered();
    void onStartTriggered();
    void onStopTriggered();
    void onImportMapTriggered();
    
    // Waypoint file operations
    void onLoadWPFile();
    void onSaveWPFile();
    void onReadWPs();
    void onWriteWPs();
    
    // Waypoint table operations
    void onAddBelowClicked();
    void onWaypointCellChanged(int row, int column);
    void onWaypointSelectionChanged();
    
    // Home location
    void onHomeLocationChanged();
    
    // Map operations
    void onMapTypeChanged(int index);
    void onZoomSliderChanged(int value);
    void onGridCheckboxToggled(bool checked);
    void onViewKMLClicked();
    
    // Route/waypoint signals from map
    void onRouteUpdated(const QString &routeId);
    void onWaypointsChanged();
};
#endif // CVISTARPLANNER_H
