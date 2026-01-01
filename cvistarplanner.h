#ifndef CVISTARPLANNER_H
#define CVISTARPLANNER_H

#include <QMainWindow>
#include <QMenu>
#include <QActionGroup>
#include <QSettings>
#include "cnetworkinterface.h"
#include "cpathsettingsdialog.h"

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
    
    // Map operations
    void onZoomSliderChanged(int value);
};
#endif // CVISTARPLANNER_H
