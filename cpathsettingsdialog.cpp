#include "cpathsettingsdialog.h"
#include "ui_cpathsettingsdialog.h"
#include <QMessageBox>

CPathSettingsDialog::CPathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPathSettingsDialog)
{
    ui->setupUi(this);
    
    // Initialize default parameters
    _m_defaultParams = CPathGenerator::PathParameters();
    
    // Load defaults to UI
    loadParametersToUI(_m_defaultParams);
    
    // Set window properties
    setWindowTitle("Path Generator Settings");
    setModal(true);
}

CPathSettingsDialog::~CPathSettingsDialog()
{
    delete ui;
}

void CPathSettingsDialog::setPathParameters(const CPathGenerator::PathParameters &params)
{
    loadParametersToUI(params);
}

CPathGenerator::PathParameters CPathSettingsDialog::getPathParameters() const
{
    return getParametersFromUI();
}

void CPathSettingsDialog::loadParametersToUI(const CPathGenerator::PathParameters &params)
{
    ui->spinBox_NumWaypoints->setValue(params.numWaypoints);
    ui->doubleSpinBox_Altitude->setValue(params.defaultAltitude);
    ui->doubleSpinBox_CurveFactor->setValue(params.curveFactor);
    ui->doubleSpinBox_SpiralTurns->setValue(params.spiralTurns);
    ui->doubleSpinBox_ZigzagAmplitude->setValue(params.zigzagAmplitude);
    ui->spinBox_ZigzagFrequency->setValue(params.zigzagFrequency);
    ui->doubleSpinBox_MaxTurnRadius->setValue(params.maxTurnRadius);
    ui->doubleSpinBox_RandomVariance->setValue(params.randomVariance);
}

CPathGenerator::PathParameters CPathSettingsDialog::getParametersFromUI() const
{
    CPathGenerator::PathParameters params;
    
    params.numWaypoints = ui->spinBox_NumWaypoints->value();
    params.defaultAltitude = ui->doubleSpinBox_Altitude->value();
    params.curveFactor = ui->doubleSpinBox_CurveFactor->value();
    params.spiralTurns = ui->doubleSpinBox_SpiralTurns->value();
    params.zigzagAmplitude = ui->doubleSpinBox_ZigzagAmplitude->value();
    params.zigzagFrequency = ui->spinBox_ZigzagFrequency->value();
    params.maxTurnRadius = ui->doubleSpinBox_MaxTurnRadius->value();
    params.randomVariance = ui->doubleSpinBox_RandomVariance->value();
    
    return params;
}

void CPathSettingsDialog::on_pushButton_Apply_clicked()
{
    accept();
}

void CPathSettingsDialog::on_pushButton_Cancel_clicked()
{
    reject();
}

void CPathSettingsDialog::on_pushButton_ResetDefaults_clicked()
{
    loadParametersToUI(_m_defaultParams);
}
