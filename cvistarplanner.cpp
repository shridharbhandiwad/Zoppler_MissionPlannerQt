#include "cvistarplanner.h"
#include "ui_cvistarplanner.h"
#include "globalConstants.h"
#include <QFileDialog>
#include <QMenu>

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

