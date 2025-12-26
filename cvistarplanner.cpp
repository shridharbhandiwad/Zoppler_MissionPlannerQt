#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include "cloadscenariodialog.h"
#include "csavescenariodialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
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

void CVistarPlanner::on_pushButton_ScenarioMenu_clicked()
{
    QMenu menu(this);
    
    QAction *loadAction = menu.addAction("Load Scenario");
    QAction *saveAction = menu.addAction("Save Scenario");
    
    // Style the menu
    menu.setStyleSheet(
        "QMenu {"
        "    background-color: white;"
        "    border: 2px solid #2196F3;"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "}"
        "QMenu::item {"
        "    background-color: transparent;"
        "    padding: 8px 20px;"
        "    font-size: 12pt;"
        "    font-weight: bold;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #2196F3;"
        "    color: white;"
        "}"
    );
    
    QAction *selectedAction = menu.exec(QCursor::pos());
    
    if (selectedAction == loadAction) {
        // Open Load Scenario Dialog
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
                        QMessageBox::warning(this, "Error", "Invalid sample scenario selected.");
                        return;
                }
                loadScenarioToMap(scenario);
                QMessageBox::information(this, "Scenario Loaded", 
                    "Sample scenario '" + scenario.name + "' has been loaded successfully.");
            } else {
                // Load from file
                QString filePath = dialog.getSelectedScenarioPath();
                if (_m_scenarioManager->loadScenario(filePath, scenario)) {
                    loadScenarioToMap(scenario);
                    QMessageBox::information(this, "Scenario Loaded", 
                        "Scenario '" + scenario.name + "' has been loaded successfully.");
                } else {
                    QMessageBox::warning(this, "Load Failed", 
                        "Failed to load scenario from file: " + filePath);
                }
            }
        }
    } else if (selectedAction == saveAction) {
        // Open Save Scenario Dialog
        CSaveScenarioDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            Scenario scenario = getCurrentScenarioFromMap();
            scenario.name = dialog.getScenarioName();
            scenario.description = dialog.getScenarioDescription();
            scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
            
            QString filePath = dialog.getFilePath();
            if (_m_scenarioManager->saveScenario(scenario, filePath)) {
                QMessageBox::information(this, "Scenario Saved", 
                    "Scenario '" + scenario.name + "' has been saved successfully to:\n" + filePath);
            } else {
                QMessageBox::warning(this, "Save Failed", 
                    "Failed to save scenario to file: " + filePath);
            }
        }
    }
}

void CVistarPlanner::loadScenarioToMap(const Scenario &scenario)
{
    // Clear current objects
    ui->mapCanvas->InitializeAllObjects();
    
    // Load all objects from scenario
    for (const ScenarioObject &obj : scenario.objects) {
        QJsonObject jsonObj;
        jsonObj["STREAM"] = "Update";
        jsonObj["TYPE"] = obj.type;
        jsonObj["ID"] = obj.id;
        jsonObj["LAT"] = obj.latitude;
        jsonObj["LON"] = obj.longitude;
        jsonObj["ALT"] = obj.altitude;
        
        // Add additional data
        for (auto it = obj.additionalData.begin(); it != obj.additionalData.end(); ++it) {
            jsonObj[it.key()] = it.value();
        }
        
        QJsonDocument doc(jsonObj);
        ui->mapCanvas->slotUpdateObject(doc);
    }
    
    // Load all routes from scenario
    for (const ScenarioRoute &route : scenario.routes) {
        QJsonObject jsonRoute;
        jsonRoute["STREAM"] = "Update";
        jsonRoute["TYPE"] = "ROUTE";
        jsonRoute["ID"] = route.id;
        jsonRoute["NAME"] = route.name;
        
        QJsonArray waypointsArray;
        for (int i = 0; i < route.waypoints.size(); ++i) {
            QJsonObject waypointObj;
            waypointObj["LAT"] = route.waypoints[i].x();
            waypointObj["LON"] = route.waypoints[i].y();
            if (i < route.altitudes.size()) {
                waypointObj["ALT"] = route.altitudes[i];
            }
            waypointsArray.append(waypointObj);
        }
        jsonRoute["WAYPOINTS"] = waypointsArray;
        
        // Add additional data
        for (auto it = route.additionalData.begin(); it != route.additionalData.end(); ++it) {
            jsonRoute[it.key()] = it.value();
        }
        
        QJsonDocument doc(jsonRoute);
        ui->mapCanvas->slotUpdateObject(doc);
    }
    
    ui->statusBar->showMessage("Scenario '" + scenario.name + "' loaded successfully", 5000);
}

Scenario CVistarPlanner::getCurrentScenarioFromMap()
{
    Scenario scenario;
    scenario.name = "Untitled Scenario";
    scenario.description = "Auto-saved scenario";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // TODO: Implement method to retrieve all objects and routes from map canvas
    // This requires adding methods to CMapCanvas to export current state
    // For now, we'll return an empty scenario structure
    
    // Note: You'll need to add methods like:
    // - ui->mapCanvas->getAllObjects() to get all ScenarioObjects
    // - ui->mapCanvas->getAllRoutes() to get all ScenarioRoutes
    
    scenario.metadata["savedFrom"] = "CVistarPlanner";
    scenario.metadata["version"] = "1.0";
    
    return scenario;
}

