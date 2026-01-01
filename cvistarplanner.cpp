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
    , _m_homeLat(0.0)
    , _m_homeLon(0.0)
    , _m_homeAlt(100.0)
    , _m_currentLat(0.0)
    , _m_currentLon(0.0)
{
    ui->setupUi(this);
    ui->mapCanvas->Initialize();
    
    // Setup toolbar menus
    setupToolbarMenus();
    
    // Setup all connections
    setupConnections();

    _m_networkInterface = new CNetworkInterface(this);
    _m_networkInterface->startListening(8888);

    connect(_m_networkInterface,SIGNAL(signalUpdateObject(QJsonDocument)),ui->mapCanvas,SLOT(slotUpdateObject(QJsonDocument)));

    // Initialize path settings dialog
    _m_pathSettingsDialog = new CPathSettingsDialog(this);

    // Initialize waypoints table
    ui->tableWidget_Waypoints->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Waypoints->setColumnWidth(0, 30);
    ui->tableWidget_Waypoints->setColumnWidth(1, 100);
    ui->tableWidget_Waypoints->setColumnWidth(2, 30);
    ui->tableWidget_Waypoints->setColumnWidth(3, 30);
    ui->tableWidget_Waypoints->setColumnWidth(4, 30);
    ui->tableWidget_Waypoints->setColumnWidth(5, 100);
    ui->tableWidget_Waypoints->setColumnWidth(6, 100);
    ui->tableWidget_Waypoints->setColumnWidth(7, 60);
    ui->tableWidget_Waypoints->setColumnWidth(8, 50);
    ui->tableWidget_Waypoints->setColumnWidth(9, 40);
    ui->tableWidget_Waypoints->setColumnWidth(10, 50);
    ui->tableWidget_Waypoints->setColumnWidth(11, 60);
    ui->tableWidget_Waypoints->setColumnWidth(12, 60);
    ui->tableWidget_Waypoints->setColumnWidth(13, 50);
    
    // Set status message
    ui->statusBar->showMessage("Ready - Select objects to place on map", 0);
    ui->label_Status->setText("Status: Ready");
}

void CVistarPlanner::setupToolbarMenus()
{
    // ========== Scenario Manager Menu ==========
    _m_scenarioMenu = new QMenu(this);
    _m_scenarioMenu->setStyleSheet(
        "QMenu {"
        "    background-color: rgba(45, 45, 53, 0.95);"
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
        "    background-color: rgba(45, 45, 53, 0.95);"
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
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionSCurve, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_S_CURVE);
        ui->statusBar->showMessage("Click to select START point for S-Curve path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionLCurve, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_L_CURVE);
        ui->statusBar->showMessage("Click to select START point for L-Curve path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionFigure8, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_FIGURE_8);
        ui->statusBar->showMessage("Click to select START point for Figure-8 path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionSpiral, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_SPIRAL);
        ui->statusBar->showMessage("Click to select START point for Spiral path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionZigzag, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_ZIGZAG);
        ui->statusBar->showMessage("Click to select START point for Zigzag path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionBezier, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_BEZIER);
        ui->statusBar->showMessage("Click to select START point for Bezier Curve path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
    connect(actionRandom, &QAction::triggered, this, [this]() { 
        ui->mapCanvas->startPathGeneration(PATH_TYPE_RANDOM);
        ui->statusBar->showMessage("Click to select START point for Random path", 5000);
        ui->label_Status->setText("Status: Path Mode");
    });
}

void CVistarPlanner::setupConnections()
{
    // Map canvas signals
    connect(ui->mapCanvas, SIGNAL(signalMouseRead(QString)), this, SLOT(slotMouseRead(QString)));
    connect(ui->mapCanvas, SIGNAL(signalClearObjectSelection()), this, SLOT(slotClearObjectSelection()));
    
    // Path generation signals
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCompleted, this, [this](QString routeId) {
        ui->statusBar->showMessage("Path generated successfully: " + routeId, 3000);
        ui->label_Status->setText("Status: Ready");
        updateWaypointsTable();
    });
    connect(ui->mapCanvas, &CMapCanvas::signalPathGenerationCancelled, this, [this]() {
        ui->statusBar->showMessage("Path generation cancelled", 3000);
        ui->label_Status->setText("Status: Ready");
    });
    
    // Toolbar actions
    connect(ui->action_ScenarioManager, &QAction::triggered, this, &CVistarPlanner::onScenarioManagerTriggered);
    connect(ui->action_PathGenerator, &QAction::triggered, this, &CVistarPlanner::onPathGeneratorTriggered);
    connect(ui->action_Initialize, &QAction::triggered, this, &CVistarPlanner::onInitializeTriggered);
    connect(ui->action_Start, &QAction::triggered, this, &CVistarPlanner::onStartTriggered);
    connect(ui->action_Stop, &QAction::triggered, this, &CVistarPlanner::onStopTriggered);
    connect(ui->action_ImportMap, &QAction::triggered, this, &CVistarPlanner::onImportMapTriggered);
    
    // Right sidebar - waypoint file operations
    connect(ui->pushButton_LoadWP, &QPushButton::clicked, this, &CVistarPlanner::onLoadWPFile);
    connect(ui->pushButton_SaveWP, &QPushButton::clicked, this, &CVistarPlanner::onSaveWPFile);
    connect(ui->pushButton_ReadWPs, &QPushButton::clicked, this, &CVistarPlanner::onReadWPs);
    connect(ui->pushButton_WriteWPs, &QPushButton::clicked, this, &CVistarPlanner::onWriteWPs);
    
    // Home location changes
    connect(ui->lineEdit_HomeLat, &QLineEdit::editingFinished, this, &CVistarPlanner::onHomeLocationChanged);
    connect(ui->lineEdit_HomeLon, &QLineEdit::editingFinished, this, &CVistarPlanner::onHomeLocationChanged);
    connect(ui->lineEdit_HomeAlt, &QLineEdit::editingFinished, this, &CVistarPlanner::onHomeLocationChanged);
    
    // Map operations
    connect(ui->comboBox_MapType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CVistarPlanner::onMapTypeChanged);
    connect(ui->slider_Zoom, &QSlider::valueChanged, this, &CVistarPlanner::onZoomSliderChanged);
    connect(ui->checkBox_Grid, &QCheckBox::toggled, this, &CVistarPlanner::onGridCheckboxToggled);
    connect(ui->pushButton_ViewKML, &QPushButton::clicked, this, &CVistarPlanner::onViewKMLClicked);
    
    // Waypoint table
    connect(ui->pushButton_AddBelow, &QPushButton::clicked, this, &CVistarPlanner::onAddBelowClicked);
    connect(ui->tableWidget_Waypoints, &QTableWidget::cellChanged, this, &CVistarPlanner::onWaypointCellChanged);
    connect(ui->tableWidget_Waypoints, &QTableWidget::itemSelectionChanged, this, &CVistarPlanner::onWaypointSelectionChanged);
    
    // Menu actions
    connect(ui->action_LoadMission, &QAction::triggered, this, &CVistarPlanner::on_pushButton_LoadScenario_clicked);
    connect(ui->action_SaveMission, &QAction::triggered, this, &CVistarPlanner::on_pushButton_SaveScenario_clicked);
    connect(ui->action_ClearMission, &QAction::triggered, this, &CVistarPlanner::on_pushButton_ResetScenario_clicked);
    connect(ui->action_NewMission, &QAction::triggered, this, &CVistarPlanner::on_pushButton_ResetScenario_clicked);
    
    connect(ui->action_StartSim, &QAction::triggered, this, &CVistarPlanner::on_pushButton_Start_clicked);
    connect(ui->action_StopSim, &QAction::triggered, this, &CVistarPlanner::on_pushButton_Stop_clicked);
}

CVistarPlanner::~CVistarPlanner()
{
    delete ui;
}

void CVistarPlanner::slotMouseRead( QString mouseRead ) {
    ui->statusBar->showMessage(mouseRead);
    
    // Parse coordinates from the mouse read string
    // Format: "Lon : X°    Lat : Y°"
    QRegularExpression re("Lon\\s*:\\s*([\\d.-]+)°\\s*Lat\\s*:\\s*([\\d.-]+)°");
    QRegularExpressionMatch match = re.match(mouseRead);
    if (match.hasMatch()) {
        _m_currentLon = match.captured(1).toDouble();
        _m_currentLat = match.captured(2).toDouble();
        updateCoordinateDisplay(_m_currentLat, _m_currentLon);
    }
}

void CVistarPlanner::updateCoordinateDisplay(double lat, double lon)
{
    ui->lineEdit_Lat->setText(QString::number(lat, 'f', 6));
    ui->lineEdit_Lon->setText(QString::number(lon, 'f', 6));
    
    // Calculate distance from home if home is set
    if (_m_homeLat != 0.0 || _m_homeLon != 0.0) {
        // Haversine formula for distance
        double R = 6371000; // Earth's radius in meters
        double lat1 = _m_homeLat * M_PI / 180.0;
        double lat2 = lat * M_PI / 180.0;
        double dLat = (lat - _m_homeLat) * M_PI / 180.0;
        double dLon = (lon - _m_homeLon) * M_PI / 180.0;
        
        double a = sin(dLat/2) * sin(dLat/2) +
                   cos(lat1) * cos(lat2) *
                   sin(dLon/2) * sin(dLon/2);
        double c = 2 * atan2(sqrt(a), sqrt(1-a));
        double distance = R * c;
        
        ui->label_Home->setText(QString("Home: %1 m").arg(distance, 0, 'f', 2));
    }
}

void CVistarPlanner::on_pushButton_Mark_clicked()
{
    selectForMarking(ui->comboBox_ObjectSelection->currentIndex());
}

void CVistarPlanner::selectForMarking( int nClass ) {
    ui->mapCanvas->SetObjectToLoadOnClick(nClass);
    
    if (nClass > 0) {
        QString typeName = ui->comboBox_ObjectSelection->currentText();
        ui->statusBar->showMessage("Click on map to place " + typeName, 5000);
        ui->label_Status->setText("Status: Placing " + typeName);
    } else {
        ui->label_Status->setText("Status: Ready");
    }
}


void CVistarPlanner::slotClearObjectSelection() {

    ui->comboBox_ObjectSelection->setCurrentIndex(0);
    ui->label_Status->setText("Status: Ready");
}

void CVistarPlanner::on_pushButton_Initialize_clicked()
{
    ui->mapCanvas->InitializeAllObjects();
    ui->statusBar->showMessage("All objects initialized", 3000);
    ui->label_Status->setText("Status: Initialized");
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
    ui->label_Status->setText("Status: Running");
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
    ui->label_Status->setText("Status: Stopped");
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
        ui->label_Status->setText("Status: Saved");
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
        ui->label_Status->setText("Status: Loaded");
        updateWaypointsTable();
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
        ui->label_Status->setText("Status: Reset");
        clearWaypointsTable();
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

// ========== Waypoint File Operations ==========

void CVistarPlanner::onLoadWPFile()
{
    on_pushButton_LoadScenario_clicked();
}

void CVistarPlanner::onSaveWPFile()
{
    on_pushButton_SaveScenario_clicked();
}

void CVistarPlanner::onReadWPs()
{
    // Read waypoints from connected device (placeholder)
    ui->statusBar->showMessage("Reading waypoints from device...", 3000);
    updateWaypointsTable();
}

void CVistarPlanner::onWriteWPs()
{
    // Write waypoints to connected device (placeholder)
    ui->statusBar->showMessage("Writing waypoints to device...", 3000);
}

// ========== Waypoint Table Operations ==========

void CVistarPlanner::updateWaypointsTable()
{
    // This will be connected to the map canvas to update the table
    // when routes change
    ui->tableWidget_Waypoints->setRowCount(0);
    
    // Get all routes from map canvas and populate table
    // This is a placeholder - actual implementation would get data from mapCanvas
}

void CVistarPlanner::addWaypointRow(int row, const QString &command, double lat, double lon, double alt, double dist, double az)
{
    ui->tableWidget_Waypoints->insertRow(row);
    
    // Row number
    QTableWidgetItem *numItem = new QTableWidgetItem(QString::number(row + 1));
    numItem->setTextAlignment(Qt::AlignCenter);
    numItem->setFlags(numItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget_Waypoints->setItem(row, 0, numItem);
    
    // Command (combo box would be better but using text for simplicity)
    QComboBox *cmdCombo = new QComboBox();
    cmdCombo->addItems({"WAYPOINT", "LOITER_UNLIM", "LOITER_TURNS", "LOITER_TIME", "RETURN_TO_LAUNCH", "LAND", "TAKEOFF"});
    cmdCombo->setCurrentText(command);
    cmdCombo->setStyleSheet("QComboBox { background-color: #2d2d35; color: white; border: none; }");
    ui->tableWidget_Waypoints->setCellWidget(row, 1, cmdCombo);
    
    // Empty columns (param1, param2, param3)
    for (int i = 2; i <= 4; i++) {
        QTableWidgetItem *item = new QTableWidgetItem("0");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Waypoints->setItem(row, i, item);
    }
    
    // Lat
    QTableWidgetItem *latItem = new QTableWidgetItem(QString::number(lat, 'f', 7));
    latItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget_Waypoints->setItem(row, 5, latItem);
    
    // Lon
    QTableWidgetItem *lonItem = new QTableWidgetItem(QString::number(lon, 'f', 7));
    lonItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget_Waypoints->setItem(row, 6, lonItem);
    
    // Alt
    QTableWidgetItem *altItem = new QTableWidgetItem(QString::number(alt, 'f', 0));
    altItem->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_Waypoints->setItem(row, 7, altItem);
    
    // Delete button
    QPushButton *deleteBtn = new QPushButton("X");
    deleteBtn->setStyleSheet("QPushButton { background-color: #c62828; color: white; font-weight: bold; border-radius: 3px; padding: 2px; }");
    deleteBtn->setFixedSize(30, 25);
    connect(deleteBtn, &QPushButton::clicked, this, [this, row]() {
        ui->tableWidget_Waypoints->removeRow(row);
        // Renumber rows
        for (int i = 0; i < ui->tableWidget_Waypoints->rowCount(); i++) {
            QTableWidgetItem *numItem = ui->tableWidget_Waypoints->item(i, 0);
            if (numItem) numItem->setText(QString::number(i + 1));
        }
    });
    ui->tableWidget_Waypoints->setCellWidget(row, 8, deleteBtn);
    
    // Up button
    QPushButton *upBtn = new QPushButton("▲");
    upBtn->setStyleSheet("QPushButton { background-color: #455a64; color: white; border-radius: 3px; padding: 2px; }");
    upBtn->setFixedSize(30, 25);
    ui->tableWidget_Waypoints->setCellWidget(row, 9, upBtn);
    
    // Down button
    QPushButton *downBtn = new QPushButton("▼");
    downBtn->setStyleSheet("QPushButton { background-color: #455a64; color: white; border-radius: 3px; padding: 2px; }");
    downBtn->setFixedSize(30, 25);
    ui->tableWidget_Waypoints->setCellWidget(row, 10, downBtn);
    
    // Grad %
    QTableWidgetItem *gradItem = new QTableWidgetItem("0.0");
    gradItem->setTextAlignment(Qt::AlignCenter);
    gradItem->setFlags(gradItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget_Waypoints->setItem(row, 11, gradItem);
    
    // Dist
    QTableWidgetItem *distItem = new QTableWidgetItem(QString::number(dist, 'f', 1));
    distItem->setTextAlignment(Qt::AlignCenter);
    distItem->setFlags(distItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget_Waypoints->setItem(row, 12, distItem);
    
    // AZ
    QTableWidgetItem *azItem = new QTableWidgetItem(QString::number(az, 'f', 0));
    azItem->setTextAlignment(Qt::AlignCenter);
    azItem->setFlags(azItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget_Waypoints->setItem(row, 13, azItem);
}

void CVistarPlanner::clearWaypointsTable()
{
    ui->tableWidget_Waypoints->setRowCount(0);
}

void CVistarPlanner::onAddBelowClicked()
{
    int currentRow = ui->tableWidget_Waypoints->currentRow();
    if (currentRow < 0) {
        currentRow = ui->tableWidget_Waypoints->rowCount() - 1;
    }
    
    // Use home location or current cursor position for new waypoint
    double lat = (_m_currentLat != 0.0) ? _m_currentLat : _m_homeLat;
    double lon = (_m_currentLon != 0.0) ? _m_currentLon : _m_homeLon;
    double alt = ui->spinBox_DefaultAlt->value();
    
    addWaypointRow(currentRow + 1, "WAYPOINT", lat, lon, alt, 0.0, 0.0);
    
    ui->statusBar->showMessage("Waypoint added", 2000);
}

void CVistarPlanner::onWaypointCellChanged(int row, int column)
{
    // Handle waypoint edits - update map canvas
    Q_UNUSED(row);
    Q_UNUSED(column);
}

void CVistarPlanner::onWaypointSelectionChanged()
{
    // Highlight selected waypoint on map
    int row = ui->tableWidget_Waypoints->currentRow();
    if (row >= 0) {
        QTableWidgetItem *latItem = ui->tableWidget_Waypoints->item(row, 5);
        QTableWidgetItem *lonItem = ui->tableWidget_Waypoints->item(row, 6);
        if (latItem && lonItem) {
            // Could pan map to this waypoint
        }
    }
}

// ========== Home Location ==========

void CVistarPlanner::onHomeLocationChanged()
{
    _m_homeLat = ui->lineEdit_HomeLat->text().toDouble();
    _m_homeLon = ui->lineEdit_HomeLon->text().toDouble();
    _m_homeAlt = ui->lineEdit_HomeAlt->text().toDouble();
    
    ui->statusBar->showMessage(QString("Home set: %1, %2, Alt %3m")
        .arg(_m_homeLat, 0, 'f', 6)
        .arg(_m_homeLon, 0, 'f', 6)
        .arg(_m_homeAlt, 0, 'f', 0), 3000);
}

// ========== Map Operations ==========

void CVistarPlanner::onMapTypeChanged(int index)
{
    QString mapType = ui->comboBox_MapType->itemText(index);
    ui->statusBar->showMessage("Map type changed to: " + mapType, 2000);
    // Actual map type change would be implemented in mapCanvas
}

void CVistarPlanner::onZoomSliderChanged(int value)
{
    // Zoom level changed - could be connected to mapCanvas zoom
    Q_UNUSED(value);
}

void CVistarPlanner::onGridCheckboxToggled(bool checked)
{
    ui->statusBar->showMessage(checked ? "Grid enabled" : "Grid disabled", 2000);
}

void CVistarPlanner::onViewKMLClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open KML File",
        "",
        "KML Files (*.kml *.kmz)"
    );
    
    if (!filePath.isEmpty()) {
        ui->statusBar->showMessage("Loading KML: " + filePath, 3000);
        // KML loading would be implemented here
    }
}

void CVistarPlanner::onRouteUpdated(const QString &routeId)
{
    Q_UNUSED(routeId);
    updateWaypointsTable();
}

void CVistarPlanner::onWaypointsChanged()
{
    updateWaypointsTable();
}
