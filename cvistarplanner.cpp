#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include "MapDisplay/cpathgenerator.h"
#include <QFileDialog>
#include <QMenu>
#include <QWidgetAction>

CVistarPlanner::CVistarPlanner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CVistarPlanner)
{
    ui->setupUi(this);
    ui->mapCanvas->Initialize();
    connect(ui->mapCanvas, SIGNAL(signalMouseRead(QString)), this, SLOT(slotMouseRead(QString)));
    connect(ui->mapCanvas, SIGNAL(signalClearObjectSelection()), this, SLOT(slotClearObjectSelection()));

    _m_networkInterface = new CNetworkInterface(this);
    _m_networkInterface->startListening(8888);

    connect(_m_networkInterface,SIGNAL(signalUpdateObject(QJsonDocument)),ui->mapCanvas,SLOT(slotUpdateObject(QJsonDocument)));

    // Initialize path settings dialog
    _m_pathSettingsDialog = new CPathSettingsDialog(this);

    // Setup Scenario Manager dropdown menu
    QMenu *scenarioMenu = new QMenu(this);
    
    // Style the menu to match the application theme - semi-transparent with rounded edges
    scenarioMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(25, 118, 210, 0.85);"
        "    border: 2px solid rgba(33, 150, 243, 0.6);"
        "    border-radius: 12px;"
        "    padding: 8px 5px;"
        "    margin: 2px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    color: white;"
        "    padding: 12px 35px;"
        "    margin: 3px 6px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 10pt;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(66, 165, 245, 0.7);"
        "    border: 1px solid rgba(100, 181, 246, 0.5);"
        "}"
        "QMenu::item:pressed {"
        "    background-color: rgba(21, 101, 192, 0.8);"
        "}"
        "QMenu::separator {"
        "    height: 2px;"
        "    background: rgba(255, 255, 255, 0.3);"
        "    margin: 6px 15px;"
        "}"
    );
    
    // Create menu actions
    QAction *actionLoadScenario = new QAction("Load Scenario", this);
    QAction *actionSaveScenario = new QAction("Save Scenario", this);
    QAction *actionResetScenario = new QAction("Reset Scenario", this);
    
    // Connect actions to existing slots
    connect(actionLoadScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_LoadScenario_clicked);
    connect(actionSaveScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_SaveScenario_clicked);
    connect(actionResetScenario, &QAction::triggered, this, &CVistarPlanner::on_pushButton_ResetScenario_clicked);
    
    // Add actions to menu
    scenarioMenu->addAction(actionLoadScenario);
    scenarioMenu->addAction(actionSaveScenario);
    scenarioMenu->addSeparator();
    scenarioMenu->addAction(actionResetScenario);
    
    // Set the menu to the button
    ui->pushButton_ScenarioManagerMenu->setMenu(scenarioMenu);

    // Setup Path Generator dropdown menu
    QMenu *pathMenu = new QMenu(this);
    
    // Style the menu with a vibrant orange/amber theme matching the button
    pathMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(230, 81, 0, 0.92);"
        "    border: 2px solid rgba(255, 152, 0, 0.6);"
        "    border-radius: 12px;"
        "    padding: 10px 5px;"
        "    margin: 2px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    color: white;"
        "    padding: 14px 40px;"
        "    margin: 4px 8px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 10pt;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(255, 167, 38, 0.75);"
        "    border: 1px solid rgba(255, 193, 7, 0.5);"
        "}"
        "QMenu::item:pressed {"
        "    background-color: rgba(191, 54, 12, 0.85);"
        "}"
        "QMenu::separator {"
        "    height: 2px;"
        "    background: rgba(255, 255, 255, 0.35);"
        "    margin: 8px 15px;"
        "}"
        "QMenu::icon {"
        "    padding-left: 15px;"
        "}"
    );
    
    // Create path type actions with icons and descriptions
    QAction *actionStraight = new QAction("➤  Straight Line", this);
    actionStraight->setToolTip("Direct path between two points");
    actionStraight->setData(PATH_TYPE_STRAIGHT);
    
    QAction *actionSCurve = new QAction("〰  S-Curve", this);
    actionSCurve->setToolTip("Smooth S-shaped trajectory using sigmoid function");
    actionSCurve->setData(PATH_TYPE_S_CURVE);
    
    QAction *actionLCurve = new QAction("⌐  L-Curve", this);
    actionLCurve->setToolTip("L-shaped path with rounded corner");
    actionLCurve->setData(PATH_TYPE_L_CURVE);
    
    QAction *actionFigure8 = new QAction("∞  Figure-8", this);
    actionFigure8->setToolTip("Figure-8 looping trajectory for surveillance");
    actionFigure8->setData(PATH_TYPE_FIGURE_8);
    
    QAction *actionSpiral = new QAction("🌀  Spiral", this);
    actionSpiral->setToolTip("Spiral path expanding outward for area coverage");
    actionSpiral->setData(PATH_TYPE_SPIRAL);
    
    QAction *actionZigzag = new QAction("⚡  Zigzag", this);
    actionZigzag->setToolTip("Zigzag evasive maneuver pattern");
    actionZigzag->setData(PATH_TYPE_ZIGZAG);
    
    QAction *actionBezier = new QAction("⌒  Bezier Curve", this);
    actionBezier->setToolTip("Smooth Bezier curve trajectory");
    actionBezier->setData(PATH_TYPE_BEZIER);
    
    QAction *actionRandom = new QAction("🎲  Random", this);
    actionRandom->setToolTip("Random waypoints for unpredictable path");
    actionRandom->setData(PATH_TYPE_RANDOM);
    
    QAction *actionSettings = new QAction("⚙  Settings", this);
    actionSettings->setToolTip("Configure path generation parameters");
    
    // Add actions to menu with separators for grouping
    pathMenu->addAction(actionSettings);
    pathMenu->addSeparator();
    pathMenu->addAction(actionStraight);
    pathMenu->addSeparator();
    pathMenu->addAction(actionSCurve);
    pathMenu->addAction(actionLCurve);
    pathMenu->addAction(actionBezier);
    pathMenu->addSeparator();
    pathMenu->addAction(actionFigure8);
    pathMenu->addAction(actionSpiral);
    pathMenu->addSeparator();
    pathMenu->addAction(actionZigzag);
    pathMenu->addAction(actionRandom);
    
    // Connect all path actions to a single slot using lambda
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
    
    // Connect settings action
    connect(actionSettings, &QAction::triggered, this, &CVistarPlanner::openPathSettings);
    
    // Connect path generation signals for status updates
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCompleted, this, [this](QString routeId) {
        ui->statusBar->showMessage("Path generated successfully: " + routeId, 3000);
    });
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCancelled, this, [this]() {
        ui->statusBar->showMessage("Path generation cancelled", 3000);
    });
    
    // Set the menu to the button
    ui->pushButton_PathGenerator->setMenu(pathMenu);
}

CVistarPlanner::~CVistarPlanner()
{
    delete ui;
}

void CVistarPlanner::slotMouseRead( QString mouseRead ) {
    ui->statusBar->showMessage(mouseRead);
}

void CVistarPlanner::on_pushButton_Mark_clicked()
{
    selectForMarking(ui->comboBox_ObjectSelection->currentIndex());
}

void CVistarPlanner::selectForMarking( int nClass ) {
    ui->mapCanvas->SetObjectToLoadOnClick(nClass);
}


void CVistarPlanner::slotClearObjectSelection() {

    ui->comboBox_ObjectSelection->setCurrentIndex(0);
}

void CVistarPlanner::on_pushButton_Initialize_clicked()
{
    ui->mapCanvas->InitializeAllObjects();
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
        "JSON Files (*.json)"
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
    ui->mapCanvas->resetScenario();
    ui->statusBar->showMessage("Scenario reset - all objects cleared!", 3000);
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

