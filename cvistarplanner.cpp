#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include "MapDisplay/cpathgenerator.h"
#include <QFileDialog>
#include <QMenu>
#include <QWidgetAction>
#include <QToolButton>
#include <QMessageBox>
#include <cmath>

CVistarPlanner::CVistarPlanner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CVistarPlanner)
    , _m_currentTheme(AppTheme::Dark)
{
    ui->setupUi(this);
    ui->mapCanvas->Initialize();
    
    // Setup toolbar menus
    setupToolbarMenus();
    
    // Setup object action group for exclusive selection
    setupObjectActions();
    
    // Setup all connections
    setupConnections();

    _m_networkInterface = new CNetworkInterface(this);
    _m_networkInterface->startListening(8888);

    connect(_m_networkInterface,SIGNAL(signalUpdateObject(QJsonDocument)),ui->mapCanvas,SLOT(slotUpdateObject(QJsonDocument)));

    // Initialize path settings dialog
    _m_pathSettingsDialog = new CPathSettingsDialog(this);
    
    // Load theme from settings and apply
    loadThemeFromSettings();
    
    // Set status message
    ui->statusBar->showMessage("Ready - Select objects to place on map", 0);
}

void CVistarPlanner::setupObjectActions()
{
    // Create action group for exclusive object selection
    _m_objectActionGroup = new QActionGroup(this);
    _m_objectActionGroup->setExclusive(false); // Allow deselection
    
    // Add all object actions to the group
    _m_objectActionGroup->addAction(ui->action_AddDrone);
    _m_objectActionGroup->addAction(ui->action_AddDroneSwarm);
    _m_objectActionGroup->addAction(ui->action_AddFighter);
    _m_objectActionGroup->addAction(ui->action_AddUAV);
    _m_objectActionGroup->addAction(ui->action_AddRadar);
    _m_objectActionGroup->addAction(ui->action_AddLauncher);
    _m_objectActionGroup->addAction(ui->action_AddMissile);
    _m_objectActionGroup->addAction(ui->action_AddRoute);
    
    // Connect action group triggered signal
    connect(_m_objectActionGroup, &QActionGroup::triggered, this, &CVistarPlanner::onAddObjectTriggered);
}

void CVistarPlanner::setupToolbarMenus()
{
    // ========== Scenario Manager Menu ==========
    _m_scenarioMenu = new QMenu(this);
    _m_scenarioMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(72, 72, 90, 0.95);"
        "    border: 2px solid rgba(74, 144, 217, 0.6);"
        "    border-radius: 8px;"
        "    padding: 8px 5px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    color: white;"
        "    padding: 12px 30px;"
        "    margin: 3px 6px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(74, 144, 217, 0.8);"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background: rgba(255, 255, 255, 0.2);"
        "    margin: 6px 15px;"
        "}"
    );
    
    QAction *actionLoadScenario = new QAction("📂  Load Scenario", this);
    QAction *actionSaveScenario = new QAction("💾  Save Scenario", this);
    QAction *actionResetScenario = new QAction("🔄  Reset Scenario", this);
    
    connect(actionLoadScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_LoadScenario_clicked);
    connect(actionSaveScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_SaveScenario_clicked);
    connect(actionResetScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_ResetScenario_clicked);
    
    _m_scenarioMenu->addAction(actionLoadScenario);
    _m_scenarioMenu->addAction(actionSaveScenario);
    _m_scenarioMenu->addSeparator();
    _m_scenarioMenu->addAction(actionResetScenario);

    // ========== Path Generator Menu ==========
    _m_pathMenu = new QMenu(this);
    _m_pathMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(72, 72, 90, 0.95);"
        "    border: 2px solid rgba(255, 152, 0, 0.6);"
        "    border-radius: 8px;"
        "    padding: 10px 5px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    color: white;"
        "    padding: 12px 35px;"
        "    margin: 4px 8px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(255, 167, 38, 0.75);"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background: rgba(255, 255, 255, 0.2);"
        "    margin: 8px 15px;"
        "}"
    );
    
    QAction *actionSettings = new QAction("⚙  Settings", this);
    QAction *actionStraight = new QAction("➤  Straight Line", this);
    QAction *actionSCurve = new QAction("〰  S-Curve", this);
    QAction *actionLCurve = new QAction("⌐  L-Curve", this);
    QAction *actionFigure8 = new QAction("∞  Figure-8", this);
    QAction *actionSpiral = new QAction("🌀  Spiral", this);
    QAction *actionZigzag = new QAction("⚡  Zigzag", this);
    QAction *actionBezier = new QAction("⌒  Bezier Curve", this);
    QAction *actionRandom = new QAction("🎲  Random", this);
    
    actionStraight->setData(PATH_TYPE_STRAIGHT);
    actionSCurve->setData(PATH_TYPE_S_CURVE);
    actionLCurve->setData(PATH_TYPE_L_CURVE);
    actionFigure8->setData(PATH_TYPE_FIGURE_8);
    actionSpiral->setData(PATH_TYPE_SPIRAL);
    actionZigzag->setData(PATH_TYPE_ZIGZAG);
    actionBezier->setData(PATH_TYPE_BEZIER);
    actionRandom->setData(PATH_TYPE_RANDOM);
    
    _m_pathMenu->addAction(actionSettings);
    _m_pathMenu->addSeparator();
    _m_pathMenu->addAction(actionStraight);
    _m_pathMenu->addSeparator();
    _m_pathMenu->addAction(actionSCurve);
    _m_pathMenu->addAction(actionLCurve);
    _m_pathMenu->addAction(actionBezier);
    _m_pathMenu->addSeparator();
    _m_pathMenu->addAction(actionFigure8);
    _m_pathMenu->addAction(actionSpiral);
    _m_pathMenu->addSeparator();
    _m_pathMenu->addAction(actionZigzag);
    _m_pathMenu->addAction(actionRandom);
    
    // Connect path menu actions
    connect(actionSettings, &QAction::triggered, this, &CVistarPlanner::openPathSettings);
    connect(actionStraight, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_STRAIGHT);
        ui->statusBar->showMessage("Click to select START point for Straight path", 5000);
    });
    connect(actionSCurve, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_S_CURVE);
        ui->statusBar->showMessage("Click to select START point for S-Curve path", 5000);
    });
    connect(actionLCurve, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_L_CURVE);
        ui->statusBar->showMessage("Click to select START point for L-Curve path", 5000);
    });
    connect(actionFigure8, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_FIGURE_8);
        ui->statusBar->showMessage("Click to select START point for Figure-8 path", 5000);
    });
    connect(actionSpiral, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_SPIRAL);
        ui->statusBar->showMessage("Click to select START point for Spiral path", 5000);
    });
    connect(actionZigzag, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_ZIGZAG);
        ui->statusBar->showMessage("Click to select START point for Zigzag path", 5000);
    });
    connect(actionBezier, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_BEZIER);
        ui->statusBar->showMessage("Click to select START point for Bezier Curve path", 5000);
    });
    connect(actionRandom, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_RANDOM);
        ui->statusBar->showMessage("Click to select START point for Random path", 5000);
    });

    // ========== Settings Menu ==========
    _m_settingsMenu = new QMenu(this);
    _m_settingsMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(72, 72, 90, 0.95);"
        "    border: 2px solid rgba(144, 164, 174, 0.6);"
        "    border-radius: 8px;"
        "    padding: 8px 5px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    color: white;"
        "    padding: 12px 30px;"
        "    margin: 3px 6px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(144, 164, 174, 0.6);"
        "}"
        "QMenu::item:checked {"
        "    background-color: rgba(74, 144, 217, 0.5);"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background: rgba(255, 255, 255, 0.2);"
        "    margin: 6px 15px;"
        "}"
    );
    
    // Create theme submenu
    QMenu *themeSubMenu = _m_settingsMenu->addMenu("🎨  Theme");
    themeSubMenu->setStyleSheet(_m_settingsMenu->styleSheet());
    
    // Create theme action group for exclusive selection
    _m_themeActionGroup = new QActionGroup(this);
    _m_themeActionGroup->setExclusive(true);
    
    _m_actionDarkTheme = new QAction("🌙  Dark Theme", this);
    _m_actionDarkTheme->setCheckable(true);
    _m_actionDarkTheme->setChecked(true);
    _m_themeActionGroup->addAction(_m_actionDarkTheme);
    
    _m_actionLightTheme = new QAction("☀️  Light Theme", this);
    _m_actionLightTheme->setCheckable(true);
    _m_themeActionGroup->addAction(_m_actionLightTheme);
    
    themeSubMenu->addAction(_m_actionDarkTheme);
    themeSubMenu->addAction(_m_actionLightTheme);
    
    // Connect theme actions
    connect(_m_actionDarkTheme, &QAction::triggered, this, &CVistarPlanner::onDarkThemeTriggered);
    connect(_m_actionLightTheme, &QAction::triggered, this, &CVistarPlanner::onLightThemeTriggered);
}

void CVistarPlanner::setupConnections()
{
    // Map canvas signals
    connect(ui->mapCanvas, SIGNAL(signalMouseRead(QString)), this, SLOT(slotMouseRead(QString)));
    connect(ui->mapCanvas, SIGNAL(signalClearObjectSelection()), this, SLOT(slotClearObjectSelection()));
    
    // Path generation signals
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCompleted, this, [this](QString routeId) {
        ui->statusBar->showMessage("Path generated successfully: " + routeId, 3000);
    });
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCancelled, this, [this]() {
        ui->statusBar->showMessage("Path generation cancelled", 3000);
    });
    
    // Toolbar actions
    connect(ui->action_ScenarioManager, &QAction::triggered, this, &CVistarPlanner::onScenarioManagerTriggered);
    connect(ui->action_PathGenerator, &QAction::triggered, this, &CVistarPlanner::onPathGeneratorTriggered);
    connect(ui->action_Initialize, &QAction::triggered, this, &CVistarPlanner::onInitializeTriggered);
    connect(ui->action_Start, &QAction::triggered, this, &CVistarPlanner::onStartTriggered);
    connect(ui->action_Stop, &QAction::triggered, this, &CVistarPlanner::onStopTriggered);
    connect(ui->action_ImportMap, &QAction::triggered, this, &CVistarPlanner::onImportMapTriggered);
    connect(ui->action_Settings, &QAction::triggered, this, &CVistarPlanner::onSettingsTriggered);
    
}

CVistarPlanner::~CVistarPlanner()
{
    delete ui;
}

void CVistarPlanner::slotMouseRead( QString mouseRead ) {
    ui->statusBar->showMessage(mouseRead);
}

void CVistarPlanner::slotClearObjectSelection() {
    // Uncheck all object actions
    for (QAction *action : _m_objectActionGroup->actions()) {
        action->setChecked(false);
    }
    ui->statusBar->showMessage("Ready - Select objects to place on map", 0);
}

void CVistarPlanner::selectForMarking( int nClass ) {
    ui->mapCanvas->SetObjectToLoadOnClick(nClass);
}

void CVistarPlanner::onAddObjectTriggered(QAction *action)
{
    // First uncheck all other actions
    for (QAction *a : _m_objectActionGroup->actions()) {
        if (a != action) {
            a->setChecked(false);
        }
    }
    
    int objectType = 0;
    QString typeName;
    
    if (action == ui->action_AddDrone) {
        objectType = 1; // DRONE index
        typeName = "Drone";
    } else if (action == ui->action_AddDroneSwarm) {
        objectType = 2; // DRONE_SWARM index
        typeName = "Drone Swarm";
    } else if (action == ui->action_AddFighter) {
        objectType = 3; // FIGHTER index
        typeName = "Fighter";
    } else if (action == ui->action_AddUAV) {
        objectType = 4; // UAV index
        typeName = "UAV";
    } else if (action == ui->action_AddRadar) {
        objectType = 5; // RADAR index
        typeName = "Radar";
    } else if (action == ui->action_AddLauncher) {
        objectType = 6; // LAUNCHER index
        typeName = "Launcher";
    } else if (action == ui->action_AddMissile) {
        objectType = 7; // MISSILE index
        typeName = "Missile";
    } else if (action == ui->action_AddRoute) {
        objectType = 8; // ROUTE index
        typeName = "Route";
    }
    
    if (action->isChecked()) {
        selectForMarking(objectType);
        ui->statusBar->showMessage("Click on map to place " + typeName, 5000);
    } else {
        selectForMarking(0);
        ui->statusBar->showMessage("Ready - Select objects to place on map", 0);
    }
}

void CVistarPlanner::on_pushButton_Initialize_clicked()
{
    ui->mapCanvas->InitializeAllObjects();
    ui->statusBar->showMessage("All objects initialized", 3000);
}


void CVistarPlanner::on_pushButton_Start_clicked()
{
    QJsonObject jsonRoot;

    // Root key-value pairs
    jsonRoot["STREAM"] = "Event";
    jsonRoot["TYPE"] = "Start";

    // Convert to string
    QJsonDocument doc(jsonRoot);

    CNetworkInterface::PublishMessage(doc);
    
    ui->statusBar->showMessage("Simulation started", 3000);
}


void CVistarPlanner::on_pushButton_Stop_clicked()
{
    QJsonObject jsonRoot;

    // Root key-value pairs
    jsonRoot["STREAM"] = "Event";
    jsonRoot["TYPE"] = "Stop";

    // Convert to string
    QJsonDocument doc(jsonRoot);

    CNetworkInterface::PublishMessage(doc);
    
    ui->statusBar->showMessage("Simulation stopped", 3000);
}


void CVistarPlanner::on_pushButton_ImportMaps_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Raster (TIFF)",
        "",
        "TIFF Files (*.tif *.tiff)"
        );

    if(filePath.isEmpty())
        return;

    ui->statusBar->showMessage("Importing map: " + filePath, 3000);
    //ui->mapCanvas->importRasterMap(filePath);
}

void CVistarPlanner::on_pushButton_SaveScenario_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Scenario",
        "",
        "JSON Files (*.json)"
        );

    if(filePath.isEmpty())
        return;

    bool success = ui->mapCanvas->saveCurrentScenario(filePath);
    if (success) {
        ui->statusBar->showMessage("Scenario saved successfully!", 3000);
    } else {
        ui->statusBar->showMessage("Failed to save scenario!", 3000);
    }
}

void CVistarPlanner::on_pushButton_LoadScenario_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Load Scenario",
        "",
        "Scenario Files (*.json *.gpx *.scenario);;JSON Files (*.json);;GPX Files (*.gpx);;All Files (*.*)"
        );

    if(filePath.isEmpty())
        return;

    bool success = ui->mapCanvas->loadScenario(filePath);
    if (success) {
        ui->statusBar->showMessage("Scenario loaded successfully!", 3000);
    } else {
        ui->statusBar->showMessage("Failed to load scenario!", 3000);
    }
}

void CVistarPlanner::on_pushButton_ResetScenario_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Reset Scenario", 
        "Are you sure you want to clear all objects and routes?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        ui->mapCanvas->resetScenario();
        ui->statusBar->showMessage("Scenario reset - all objects cleared!", 3000);
    }
}

void CVistarPlanner::openPathSettings()
{
    // Load current settings from map canvas
    _m_pathSettingsDialog->setPathParameters(ui->mapCanvas->getPathParameters());
    
    // Show dialog and apply if accepted
    if (_m_pathSettingsDialog->exec() == QDialog::Accepted) {
        CPathGenerator::PathParameters params = _m_pathSettingsDialog->getPathParameters();
        ui->mapCanvas->setPathParameters(params);
        ui->statusBar->showMessage("Path generation settings updated!", 3000);
    }
}

// ========== New Toolbar Slots ==========

void CVistarPlanner::onScenarioManagerTriggered()
{
    // Get the toolbar button's position
    QToolButton *button = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->action_ScenarioManager));
    if (button) {
        QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
        _m_scenarioMenu->exec(pos);
    } else {
        _m_scenarioMenu->exec(QCursor::pos());
    }
}

void CVistarPlanner::onPathGeneratorTriggered()
{
    QToolButton *button = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->action_PathGenerator));
    if (button) {
        QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
        _m_pathMenu->exec(pos);
    } else {
        _m_pathMenu->exec(QCursor::pos());
    }
}

void CVistarPlanner::onInitializeTriggered()
{
    on_pushButton_Initialize_clicked();
}

void CVistarPlanner::onStartTriggered()
{
    on_pushButton_Start_clicked();
}

void CVistarPlanner::onStopTriggered()
{
    on_pushButton_Stop_clicked();
}

void CVistarPlanner::onImportMapTriggered()
{
    on_pushButton_ImportMaps_clicked();
}

void CVistarPlanner::onSettingsTriggered()
{
    QToolButton *button = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->action_Settings));
    if (button) {
        QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
        _m_settingsMenu->exec(pos);
    } else {
        _m_settingsMenu->exec(QCursor::pos());
    }
}

// ========== Theme Management ==========

void CVistarPlanner::onDarkThemeTriggered()
{
    applyTheme(AppTheme::Dark);
    ui->statusBar->showMessage("Dark theme applied", 3000);
}

void CVistarPlanner::onLightThemeTriggered()
{
    applyTheme(AppTheme::Light);
    ui->statusBar->showMessage("Light theme applied", 3000);
}

void CVistarPlanner::applyTheme(AppTheme theme)
{
    _m_currentTheme = theme;
    saveThemeToSettings(theme);
    
    if (theme == AppTheme::Dark) {
        setStyleSheet(getDarkThemeStyleSheet());
        _m_actionDarkTheme->setChecked(true);
    } else {
        setStyleSheet(getLightThemeStyleSheet());
        _m_actionLightTheme->setChecked(true);
    }
    
    // Update settings menu stylesheet based on theme
    QString menuStyle;
    if (theme == AppTheme::Dark) {
        menuStyle = R"(
            QMenu {
                background-color: rgba(72, 72, 90, 0.95);
                border: 2px solid rgba(144, 164, 174, 0.6);
                border-radius: 8px;
                padding: 8px 5px;
            }
            QMenu::item {
                background-color: transparent;
                color: white;
                padding: 12px 30px;
                margin: 3px 6px;
                border-radius: 6px;
                font-weight: bold;
            }
            QMenu::item:selected {
                background-color: rgba(144, 164, 174, 0.6);
            }
            QMenu::item:checked {
                background-color: rgba(74, 144, 217, 0.5);
            }
            QMenu::separator {
                height: 1px;
                background: rgba(255, 255, 255, 0.2);
                margin: 6px 15px;
            }
        )";
    } else {
        menuStyle = R"(
            QMenu {
                background-color: rgba(255, 255, 255, 0.98);
                border: 2px solid rgba(144, 164, 174, 0.8);
                border-radius: 8px;
                padding: 8px 5px;
            }
            QMenu::item {
                background-color: transparent;
                color: #333333;
                padding: 12px 30px;
                margin: 3px 6px;
                border-radius: 6px;
                font-weight: bold;
            }
            QMenu::item:selected {
                background-color: rgba(144, 164, 174, 0.4);
            }
            QMenu::item:checked {
                background-color: rgba(74, 144, 217, 0.3);
            }
            QMenu::separator {
                height: 1px;
                background: rgba(0, 0, 0, 0.15);
                margin: 6px 15px;
            }
        )";
    }
    _m_settingsMenu->setStyleSheet(menuStyle);
}

void CVistarPlanner::loadThemeFromSettings()
{
    QSettings settings("VistarPlanner", "VistarPlanner");
    int themeValue = settings.value("theme", static_cast<int>(AppTheme::Dark)).toInt();
    AppTheme theme = static_cast<AppTheme>(themeValue);
    applyTheme(theme);
}

void CVistarPlanner::saveThemeToSettings(AppTheme theme)
{
    QSettings settings("VistarPlanner", "VistarPlanner");
    settings.setValue("theme", static_cast<int>(theme));
}

QString CVistarPlanner::getDarkThemeStyleSheet()
{
    return R"(
QMainWindow {
    background-color: #3a3a44;
}
QToolBar {
    background-color: #48485a;
    border: none;
    padding: 5px;
    spacing: 5px;
}
QToolButton {
    background-color: #5a5a6a;
    border: 1px solid #6a6a7a;
    border-radius: 6px;
    padding: 8px;
    color: white;
    min-width: 40px;
    min-height: 40px;
}
QToolButton:hover {
    background-color: #4a90d9;
    border-color: #5aa0e9;
}
QToolButton:pressed {
    background-color: #3a7bc8;
}
QToolButton:checked {
    background-color: #4a90d9;
    border: 2px solid #7ab8f5;
}
QLabel {
    color: #f0f0f0;
}
QSlider::groove:horizontal {
    background: #4a4a58;
    height: 8px;
    border-radius: 4px;
}
QSlider::handle:horizontal {
    background: #4a90d9;
    width: 20px;
    margin: -6px 0;
    border-radius: 10px;
}
QStatusBar {
    background-color: #48485a;
    color: white;
    border-top: 1px solid #5a5a6a;
}
QMenu {
    background-color: rgba(72, 72, 90, 0.95);
    border: 2px solid rgba(74, 144, 217, 0.6);
    border-radius: 8px;
    padding: 8px 5px;
}
QMenu::item {
    background-color: transparent;
    color: white;
    padding: 12px 30px;
    margin: 3px 6px;
    border-radius: 6px;
    font-weight: bold;
}
QMenu::item:selected {
    background-color: rgba(74, 144, 217, 0.8);
}
QMenu::separator {
    height: 1px;
    background: rgba(255, 255, 255, 0.2);
    margin: 6px 15px;
}
)";
}

QString CVistarPlanner::getLightThemeStyleSheet()
{
    return R"(
QMainWindow {
    background-color: #f5f5f5;
}
QToolBar {
    background-color: #e8e8e8;
    border: none;
    padding: 5px;
    spacing: 5px;
}
QToolButton {
    background-color: #ffffff;
    border: 1px solid #c0c0c0;
    border-radius: 6px;
    padding: 8px;
    color: #333333;
    min-width: 40px;
    min-height: 40px;
}
QToolButton:hover {
    background-color: #d0e8ff;
    border-color: #4a90d9;
}
QToolButton:pressed {
    background-color: #b0d4ff;
}
QToolButton:checked {
    background-color: #4a90d9;
    border: 2px solid #2a70b9;
    color: white;
}
QLabel {
    color: #333333;
}
QSlider::groove:horizontal {
    background: #d0d0d0;
    height: 8px;
    border-radius: 4px;
}
QSlider::handle:horizontal {
    background: #4a90d9;
    width: 20px;
    margin: -6px 0;
    border-radius: 10px;
}
QStatusBar {
    background-color: #e8e8e8;
    color: #333333;
    border-top: 1px solid #c0c0c0;
}
QMenu {
    background-color: rgba(255, 255, 255, 0.98);
    border: 2px solid rgba(74, 144, 217, 0.6);
    border-radius: 8px;
    padding: 8px 5px;
}
QMenu::item {
    background-color: transparent;
    color: #333333;
    padding: 12px 30px;
    margin: 3px 6px;
    border-radius: 6px;
    font-weight: bold;
}
QMenu::item:selected {
    background-color: rgba(74, 144, 217, 0.3);
}
QMenu::separator {
    height: 1px;
    background: rgba(0, 0, 0, 0.15);
    margin: 6px 15px;
}
)";
}

