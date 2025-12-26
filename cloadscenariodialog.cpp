#include "cloadscenariodialog.h"
#include "ui_cloadscenariodialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

CLoadScenarioDialog::CLoadScenarioDialog(CScenarioManager *scenarioManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CLoadScenarioDialog)
    , _m_scenarioManager(scenarioManager)
    , _m_loadSampleScenario(false)
    , _m_selectedSampleIndex(-1)
{
    ui->setupUi(this);
    setWindowTitle("Load Scenario");
    
    loadAvailableScenarios();
    loadSampleScenarios();
}

CLoadScenarioDialog::~CLoadScenarioDialog()
{
    delete ui;
}

void CLoadScenarioDialog::loadAvailableScenarios()
{
    ui->listWidget_Scenarios->clear();
    
    QStringList scenarios = _m_scenarioManager->getAvailableScenarios();
    
    if (scenarios.isEmpty()) {
        ui->listWidget_Scenarios->addItem("No saved scenarios found");
        ui->listWidget_Scenarios->item(0)->setFlags(Qt::NoItemFlags);
    } else {
        for (const QString &scenario : scenarios) {
            ui->listWidget_Scenarios->addItem(scenario);
        }
    }
}

void CLoadScenarioDialog::loadSampleScenarios()
{
    ui->listWidget_Samples->clear();
    
    QListWidgetItem *item1 = new QListWidgetItem("Basic Defense Setup");
    item1->setToolTip("A basic air defense scenario with radar, launcher, and patrol route");
    ui->listWidget_Samples->addItem(item1);
    
    QListWidgetItem *item2 = new QListWidgetItem("Air Patrol Mission");
    item2->setToolTip("Multiple UAVs on patrol routes with surveillance coverage");
    ui->listWidget_Samples->addItem(item2);
    
    QListWidgetItem *item3 = new QListWidgetItem("Multi-Target Engagement");
    item3->setToolTip("Complex scenario with multiple threats and defense assets");
    ui->listWidget_Samples->addItem(item3);
}

QString CLoadScenarioDialog::getSelectedScenarioPath() const
{
    return _m_selectedScenarioPath;
}

bool CLoadScenarioDialog::shouldLoadSampleScenario() const
{
    return _m_loadSampleScenario;
}

int CLoadScenarioDialog::getSelectedSampleScenarioIndex() const
{
    return _m_selectedSampleIndex;
}

void CLoadScenarioDialog::on_pushButton_Load_clicked()
{
    if (ui->tabWidget->currentIndex() == 0) {
        // Load from saved scenarios
        QListWidgetItem *item = ui->listWidget_Scenarios->currentItem();
        if (!item) {
            QMessageBox::warning(this, "No Selection", "Please select a scenario to load.");
            return;
        }
        
        if (item->flags() == Qt::NoItemFlags) {
            QMessageBox::warning(this, "Invalid Selection", "No saved scenarios available.");
            return;
        }
        
        QString scenarioDir = _m_scenarioManager->getDefaultScenariosDirectory();
        _m_selectedScenarioPath = scenarioDir + "/" + item->text();
        _m_loadSampleScenario = false;
        
    } else {
        // Load sample scenario
        QListWidgetItem *item = ui->listWidget_Samples->currentItem();
        if (!item) {
            QMessageBox::warning(this, "No Selection", "Please select a sample scenario to load.");
            return;
        }
        
        _m_selectedSampleIndex = ui->listWidget_Samples->row(item);
        _m_loadSampleScenario = true;
    }
    
    accept();
}

void CLoadScenarioDialog::on_pushButton_Cancel_clicked()
{
    reject();
}

void CLoadScenarioDialog::on_pushButton_Browse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Scenario File",
        _m_scenarioManager->getDefaultScenariosDirectory(),
        "Scenario Files (*.json *.scenario);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        _m_selectedScenarioPath = filePath;
        _m_loadSampleScenario = false;
        accept();
    }
}

void CLoadScenarioDialog::on_listWidget_Scenarios_itemDoubleClicked(QListWidgetItem *item)
{
    if (item && item->flags() != Qt::NoItemFlags) {
        on_pushButton_Load_clicked();
    }
}

void CLoadScenarioDialog::on_listWidget_Samples_itemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        on_pushButton_Load_clicked();
    }
}

void CLoadScenarioDialog::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index);
    // Could add functionality here if needed
}
