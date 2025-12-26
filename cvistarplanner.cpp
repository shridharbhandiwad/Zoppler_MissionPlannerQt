#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include "cloadscenariodialog.h"
#include "csavescenariodialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

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
    
    // Initialize scenario manager
    _m_scenarioManager = new CScenarioManager(this);
    connect(_m_scenarioManager, &CScenarioManager::scenarioLoaded, this, &CVistarPlanner::onScenarioLoaded);
    connect(_m_scenarioManager, &CScenarioManager::scenarioSaved, this, &CVistarPlanner::onScenarioSaved);
    connect(_m_scenarioManager, &CScenarioManager::error, this, &CVistarPlanner::onScenarioError);
    
    // Setup scenario menu
    setupScenarioMenu();
}

CVistarPlanner::~CVistarPlanner()
{
    delete ui;
    if (_m_scenarioMenu) {
        delete _m_scenarioMenu;
    }
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

void CVistarPlanner::setupScenarioMenu()
{
    // Create the Scenario button with dropdown menu
    _m_scenarioButton = new QPushButton("Scenario", this);
    _m_scenarioButton->setMinimumSize(150, 51);
    _m_scenarioButton->setFont(QFont("Arial", 14, QFont::Bold));
    _m_scenarioButton->setStyleSheet(
        "QPushButton {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #FF9800,"
        "                                      stop:1 #F57C00);"
        "    border: none;"
        "    color: white;"
        "    padding: 10px 20px;"
        "    border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #FFB74D,"
        "                                      stop:1 #FB8C00);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #E65100;"
        "}"
        "QPushButton::menu-indicator {"
        "    image: none;"
        "}"
    );
    
    // Create the dropdown menu
    _m_scenarioMenu = new QMenu(this);
    
    QAction *loadAction = new QAction("Load Scenario", this);
    loadAction->setToolTip("Load an existing scenario including objects, routes, etc.");
    connect(loadAction, &QAction::triggered, this, &CVistarPlanner::onLoadScenario);
    
    QAction *saveAction = new QAction("Save Scenario", this);
    saveAction->setToolTip("Save the current scenario to a file");
    connect(saveAction, &QAction::triggered, this, &CVistarPlanner::onSaveScenario);
    
    _m_scenarioMenu->addAction(loadAction);
    _m_scenarioMenu->addAction(saveAction);
    
    _m_scenarioButton->setMenu(_m_scenarioMenu);
    
    // Position the button in the UI (next to Import Map button)
    _m_scenarioButton->setParent(ui->mapCanvas);
    _m_scenarioButton->move(210, 10);
    _m_scenarioButton->show();
}

void CVistarPlanner::onLoadScenario()
{
    CLoadScenarioDialog dialog(_m_scenarioManager, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        Scenario scenario;
        
        if (dialog.shouldLoadSampleScenario()) {
            // Load sample scenario
            int index = dialog.getSelectedSampleScenarioIndex();
            switch (index) {
                case 0:
                    scenario = CScenarioManager::createSampleScenario1();
                    break;
                case 1:
                    scenario = CScenarioManager::createSampleScenario2();
                    break;
                case 2:
                    scenario = CScenarioManager::createSampleScenario3();
                    break;
                default:
                    QMessageBox::warning(this, "Error", "Invalid sample scenario selected");
                    return;
            }
            
            loadScenarioToCanvas(scenario);
            ui->statusBar->showMessage("Sample scenario loaded: " + scenario.name, 5000);
            
        } else {
            // Load from file
            QString filePath = dialog.getSelectedScenarioPath();
            if (!filePath.isEmpty()) {
                if (_m_scenarioManager->loadScenario(filePath, scenario)) {
                    loadScenarioToCanvas(scenario);
                } else {
                    QMessageBox::warning(this, "Load Error", "Failed to load scenario from file");
                }
            }
        }
    }
}

void CVistarPlanner::onSaveScenario()
{
    CSaveScenarioDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        Scenario scenario = getCurrentScenarioFromCanvas();
        scenario.name = dialog.getScenarioName();
        scenario.description = dialog.getScenarioDescription();
        scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
        
        QString filePath = dialog.getFilePath();
        if (!filePath.isEmpty()) {
            if (_m_scenarioManager->saveScenario(scenario, filePath)) {
                ui->statusBar->showMessage("Scenario saved successfully: " + scenario.name, 5000);
            } else {
                QMessageBox::warning(this, "Save Error", "Failed to save scenario to file");
            }
        }
    }
}

void CVistarPlanner::loadScenarioToCanvas(const Scenario &scenario)
{
    // Clear existing objects first
    ui->mapCanvas->InitializeAllObjects();
    
    // Load objects
    for (const ScenarioObject &obj : scenario.objects) {
        QJsonObject jsonObj;
        jsonObj["ID"] = obj.id;
        jsonObj["TYPE"] = obj.type;
        jsonObj["LAT"] = obj.latitude;
        jsonObj["LON"] = obj.longitude;
        jsonObj["ALT"] = obj.altitude;
        
        // Merge additional data
        for (auto it = obj.additionalData.begin(); it != obj.additionalData.end(); ++it) {
            jsonObj[it.key()] = it.value();
        }
        
        QJsonDocument doc(jsonObj);
        // Assuming mapCanvas has a slot to load objects
        // ui->mapCanvas->slotUpdateObject(doc);
    }
    
    // Load routes
    for (const ScenarioRoute &route : scenario.routes) {
        // Create route object
        QJsonObject jsonRoute;
        jsonRoute["ID"] = route.id;
        jsonRoute["NAME"] = route.name;
        
        QJsonArray waypointsArray;
        for (int i = 0; i < route.waypoints.size(); ++i) {
            QJsonObject wp;
            wp["LAT"] = route.waypoints[i].x();
            wp["LON"] = route.waypoints[i].y();
            if (i < route.altitudes.size()) {
                wp["ALT"] = route.altitudes[i];
            }
            waypointsArray.append(wp);
        }
        jsonRoute["WAYPOINTS"] = waypointsArray;
        
        QJsonDocument doc(jsonRoute);
        // Assuming mapCanvas has a slot to load routes
        ui->mapCanvas->slotLoadRoute(doc);
    }
    
    ui->statusBar->showMessage("Scenario loaded: " + scenario.name + " - " + 
                               QString::number(scenario.objects.size()) + " objects, " +
                               QString::number(scenario.routes.size()) + " routes", 5000);
}

Scenario CVistarPlanner::getCurrentScenarioFromCanvas()
{
    Scenario scenario;
    
    // Get current objects from canvas
    // This is a placeholder - you'll need to implement methods in mapCanvas to retrieve objects
    // For now, we'll create an empty scenario that can be populated
    
    scenario.name = "Untitled Scenario";
    scenario.description = "Scenario captured from current state";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // TODO: Implement methods to get objects and routes from mapCanvas
    // Example:
    // QList<QJsonObject> objects = ui->mapCanvas->getAllObjects();
    // for (const QJsonObject &obj : objects) {
    //     ScenarioObject scObj;
    //     scObj.id = obj["ID"].toString();
    //     scObj.type = obj["TYPE"].toString();
    //     scObj.latitude = obj["LAT"].toDouble();
    //     scObj.longitude = obj["LON"].toDouble();
    //     scObj.altitude = obj["ALT"].toDouble();
    //     scenario.objects.append(scObj);
    // }
    
    return scenario;
}

void CVistarPlanner::onScenarioLoaded(const Scenario &scenario)
{
    loadScenarioToCanvas(scenario);
}

void CVistarPlanner::onScenarioSaved(const QString &filePath)
{
    QMessageBox::information(this, "Success", "Scenario saved successfully to:\n" + filePath);
}

void CVistarPlanner::onScenarioError(const QString &message)
{
    QMessageBox::critical(this, "Scenario Error", message);
}

