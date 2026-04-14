#ifndef CVISTARPLANNER_H
#define CVISTARPLANNER_H

#include <QMainWindow>
#include <QMenu>
#include <QActionGroup>
#include <QSettings>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include "cnetworkinterface.h"
#include "cpathsettingsdialog.h"
#include "RadarView/radarmanager.h"
#include "RadarView/radarlistpanel.h"
#include "RadarView/radardockwidget.h"
#include "RadarView/radarattributesdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CVistarPlanner;
}
QT_END_NAMESPACE

// Theme enumeration
enum class AppTheme {
    Dark,
    Light
};

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
    void setupObjectActions();
    
    // Theme management
    void applyTheme(AppTheme theme);
    void loadThemeFromSettings();
    void saveThemeToSettings(AppTheme theme);
    QString getDarkThemeStyleSheet();
    QString getLightThemeStyleSheet();

    CNetworkInterface *_m_networkInterface;
    CPathSettingsDialog *_m_pathSettingsDialog;

    // ── Radar View ─────────────────────────────────────────────────────
    RadarManager     *_m_radarManager;
    RadarListPanel   *_m_radarListPanel;
    QDockWidget      *_m_radarListDock;
    QMap<int, RadarDockWidget*> _m_radarDocks;       // radarId → dock
    QMap<QString, int>         _m_radarObjectIdToIntId;   // objectId → radarId
    QMap<QString, QJsonObject> _m_radarAttrsCache;        // objectId → attrs JSON
    int               _m_nextRadarIntId = 1;

    void setupRadarView();
    void syncRadarAttrsCache();

    // Menus for toolbar buttons
    QMenu *_m_scenarioMenu;
    QMenu *_m_pathMenu;
    QMenu *_m_settingsMenu;
    
    // Theme actions
    QAction *_m_actionDarkTheme;
    QAction *_m_actionLightTheme;
    QActionGroup *_m_themeActionGroup;
    AppTheme _m_currentTheme;
    
    // Action group for exclusive object selection
    QActionGroup *_m_objectActionGroup;

private slots :
    void slotMouseRead( QString mouseRead );
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
    void onSettingsTriggered();
    
    // Theme slots
    void onDarkThemeTriggered();
    void onLightThemeTriggered();

    // Object placement actions
    void onAddObjectTriggered(QAction *action);

    // Radar View slots
    void onRadarViewTriggered();
    void onRadarSelected(int radarId);
    void onRadarDockClosed(int radarId);
    // Called when the map canvas places/loads a radar object.
    void onRadarObjectPlaced(QString radarObjectId, double maxRangeKm);
    // Called when the scenario is cleared/reset.
    void onScenarioObjectsCleared();
    // Called when the user selects Attributes from a RADAR's context menu.
    void onOpenRadarAttributes(QString radarObjectId);
    // Called when a scenario with radarAttributes block is loaded from disk.
    void onRadarObjectAttrsLoaded(QString radarObjectId, QJsonObject attrsJson);
    // Called when a RadarDetection UDP message arrives.
    void onRadarDetectionsUpdated(QString radarObjectId, QJsonArray detectionsJson);
};
#endif // CVISTARPLANNER_H
