#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include "MapDisplay/cpathgenerator.h"
#include "MapDisplay/caipathgenerator.h"
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
    
    // Create settings action
    QAction *actionSettings = new QAction("⚙  Settings", this);
    actionSettings->setToolTip("Configure path generation parameters (AI/Conventional)");
    
    // === AI Path Generation Submenu ===
    QMenu *aiSubMenu = new QMenu("🤖 AI Path Generation", this);
    aiSubMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(0, 150, 136, 0.92);"
        "    border: 2px solid rgba(0, 188, 212, 0.6);"
        "    border-radius: 12px;"
        "    padding: 10px 5px;"
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
        "    background-color: rgba(0, 188, 212, 0.7);"
        "    border: 1px solid rgba(77, 208, 225, 0.5);"
        "}"
        "QMenu::item:pressed {"
        "    background-color: rgba(0, 121, 107, 0.85);"
        "}"
    );
    
    // AI Mission type actions
    QAction *actionAIPatrol = new QAction("🔍  Patrol Mission", this);
    actionAIPatrol->setToolTip("AI-generated patrol pattern for area surveillance");
    actionAIPatrol->setData(AI_MISSION_PATROL);
    
    QAction *actionAIStrike = new QAction("💥  Strike Mission", this);
    actionAIStrike->setToolTip("AI-optimized strike approach with ingress/egress planning");
    actionAIStrike->setData(AI_MISSION_STRIKE);
    
    QAction *actionAIRecon = new QAction("📡  Recon Mission", this);
    actionAIRecon->setToolTip("AI reconnaissance path with optimal sensor coverage");
    actionAIRecon->setData(AI_MISSION_RECON);
    
    QAction *actionAIEvasion = new QAction("🛡️  Evasion Mission", this);
    actionAIEvasion->setToolTip("AI unpredictable path to avoid detection and interception");
    actionAIEvasion->setData(AI_MISSION_EVASION);
    
    QAction *actionAIIntercept = new QAction("🎯  Intercept Mission", this);
    actionAIIntercept->setToolTip("AI optimal intercept trajectory for fast target approach");
    actionAIIntercept->setData(AI_MISSION_INTERCEPT);
    
    QAction *actionAIEscort = new QAction("✈️  Escort Mission", this);
    actionAIEscort->setToolTip("AI protective escort pattern for asset defense");
    actionAIEscort->setData(AI_MISSION_ESCORT);
    
    // Add AI mission actions to submenu
    aiSubMenu->addAction(actionAIPatrol);
    aiSubMenu->addAction(actionAIRecon);
    aiSubMenu->addSeparator();
    aiSubMenu->addAction(actionAIStrike);
    aiSubMenu->addAction(actionAIIntercept);
    aiSubMenu->addSeparator();
    aiSubMenu->addAction(actionAIEvasion);
    aiSubMenu->addAction(actionAIEscort);
    
    // Connect AI mission actions
    connect(actionAIPatrol, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_PATROL);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Patrol mission", 5000);
    });
    connect(actionAIStrike, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_STRIKE);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Strike mission", 5000);
    });
    connect(actionAIRecon, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_RECON);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Reconnaissance mission", 5000);
    });
    connect(actionAIEvasion, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_EVASION);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Evasion mission", 5000);
    });
    connect(actionAIIntercept, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_INTERCEPT);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Intercept mission", 5000);
    });
    connect(actionAIEscort, &QAction::triggered, this, [this]() {
        ui->mapCanvas->startAIPathGeneration(AI_MISSION_ESCORT);
        ui->statusBar->showMessage("🤖 AI Mode: Click to select START point for Escort mission", 5000);
    });
    
    // === Conventional Path Generation Submenu ===
    QMenu *conventionalSubMenu = new QMenu("⚙️ Conventional Paths", this);
    conventionalSubMenu->setStyleSheet(pathMenu->styleSheet());  // Same style as parent
    
    // Create conventional path type actions
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
    
    // Add conventional actions to submenu
    conventionalSubMenu->addAction(actionStraight);
    conventionalSubMenu->addSeparator();
    conventionalSubMenu->addAction(actionSCurve);
    conventionalSubMenu->addAction(actionLCurve);
    conventionalSubMenu->addAction(actionBezier);
    conventionalSubMenu->addSeparator();
    conventionalSubMenu->addAction(actionFigure8);
    conventionalSubMenu->addAction(actionSpiral);
    conventionalSubMenu->addSeparator();
    conventionalSubMenu->addAction(actionZigzag);
    conventionalSubMenu->addAction(actionRandom);
    
    // Add all to main path menu
    pathMenu->addAction(actionSettings);
    pathMenu->addSeparator();
    pathMenu->addMenu(aiSubMenu);
    pathMenu->addMenu(conventionalSubMenu);
    
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
        
        // Also update AI path parameters
        CAIPathGenerator::AIPathParameters aiParams = ui->mapCanvas->getAIPathParameters();
        aiParams.missionType = params.aiMissionType;
        aiParams.avoidDetection = params.aiAvoidDetection;
        aiParams.terrainFollowing = params.aiTerrainFollowing;
        aiParams.minAltitude = params.aiMinAltitude;
        aiParams.maxAltitude = params.aiMaxAltitude;
        aiParams.threatRadius = params.aiThreatRadius;
        aiParams.optimizeForFuel = params.aiOptimizeForFuel;
        aiParams.numWaypoints = params.numWaypoints;
        aiParams.preferredAltitude = params.defaultAltitude;
        ui->mapCanvas->setAIPathParameters(aiParams);
        
        QString modeStr = params.generationMethod == PATH_METHOD_AI ? "AI" : "Conventional";
        ui->statusBar->showMessage("Path generation settings updated (" + modeStr + " mode)!", 3000);
    }
}

