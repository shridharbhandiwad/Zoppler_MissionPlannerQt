#include "cpathsettingsdialog.h"
#include "ui_cpathsettingsdialog.h"
#include <QMessageBox>

// Settings keys for path parameters
static const QString SETTINGS_GROUP = "PathGeneratorDefaults";
static const QString KEY_NUM_WAYPOINTS = "numWaypoints";
static const QString KEY_DEFAULT_ALTITUDE = "defaultAltitude";
static const QString KEY_CURVE_FACTOR = "curveFactor";
static const QString KEY_SPIRAL_TURNS = "spiralTurns";
static const QString KEY_ZIGZAG_AMPLITUDE = "zigzagAmplitude";
static const QString KEY_ZIGZAG_FREQUENCY = "zigzagFrequency";
static const QString KEY_MAX_TURN_RADIUS = "maxTurnRadius";
static const QString KEY_RANDOM_VARIANCE = "randomVariance";
static const QString KEY_SPREAD_RADIUS_KM = "spreadRadiusKm";

CPathSettingsDialog::CPathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPathSettingsDialog)
{
    ui->setupUi(this);
    
    // Initialize default parameters from saved settings (or hardcoded defaults)
    _m_defaultParams = loadDefaultParametersFromSettings();
    
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
    ui->doubleSpinBox_SpreadRadius->setValue(params.spreadRadiusKm);
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
    params.spreadRadiusKm = ui->doubleSpinBox_SpreadRadius->value();
    
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

void CPathSettingsDialog::on_pushButton_SaveAsDefaults_clicked()
{
    CPathGenerator::PathParameters currentParams = getParametersFromUI();
    saveDefaultParametersToSettings(currentParams);
    _m_defaultParams = currentParams;
    
    QMessageBox::information(this, "Defaults Saved",
        "Current path generation settings have been saved as the new defaults.\n\n"
        "These settings will be used as defaults for future sessions.");
}

void CPathSettingsDialog::saveDefaultParametersToSettings(const CPathGenerator::PathParameters &params)
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    
    settings.setValue(KEY_NUM_WAYPOINTS, params.numWaypoints);
    settings.setValue(KEY_DEFAULT_ALTITUDE, params.defaultAltitude);
    settings.setValue(KEY_CURVE_FACTOR, params.curveFactor);
    settings.setValue(KEY_SPIRAL_TURNS, params.spiralTurns);
    settings.setValue(KEY_ZIGZAG_AMPLITUDE, params.zigzagAmplitude);
    settings.setValue(KEY_ZIGZAG_FREQUENCY, params.zigzagFrequency);
    settings.setValue(KEY_MAX_TURN_RADIUS, params.maxTurnRadius);
    settings.setValue(KEY_RANDOM_VARIANCE, params.randomVariance);
    settings.setValue(KEY_SPREAD_RADIUS_KM, params.spreadRadiusKm);
    
    settings.endGroup();
    settings.sync();
}

CPathGenerator::PathParameters CPathSettingsDialog::loadDefaultParametersFromSettings() const
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    // Only override if the setting exists (preserves hardcoded defaults otherwise)
    if (settings.contains(KEY_NUM_WAYPOINTS))
        params.numWaypoints = settings.value(KEY_NUM_WAYPOINTS).toInt();
    if (settings.contains(KEY_DEFAULT_ALTITUDE))
        params.defaultAltitude = settings.value(KEY_DEFAULT_ALTITUDE).toDouble();
    if (settings.contains(KEY_CURVE_FACTOR))
        params.curveFactor = settings.value(KEY_CURVE_FACTOR).toDouble();
    if (settings.contains(KEY_SPIRAL_TURNS))
        params.spiralTurns = settings.value(KEY_SPIRAL_TURNS).toDouble();
    if (settings.contains(KEY_ZIGZAG_AMPLITUDE))
        params.zigzagAmplitude = settings.value(KEY_ZIGZAG_AMPLITUDE).toDouble();
    if (settings.contains(KEY_ZIGZAG_FREQUENCY))
        params.zigzagFrequency = settings.value(KEY_ZIGZAG_FREQUENCY).toInt();
    if (settings.contains(KEY_MAX_TURN_RADIUS))
        params.maxTurnRadius = settings.value(KEY_MAX_TURN_RADIUS).toDouble();
    if (settings.contains(KEY_RANDOM_VARIANCE))
        params.randomVariance = settings.value(KEY_RANDOM_VARIANCE).toDouble();
    if (settings.contains(KEY_SPREAD_RADIUS_KM))
        params.spreadRadiusKm = settings.value(KEY_SPREAD_RADIUS_KM).toDouble();
    
    settings.endGroup();
    
    return params;
}

CPathGenerator::PathParameters CPathSettingsDialog::getSavedDefaultParameters()
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    if (settings.contains(KEY_NUM_WAYPOINTS))
        params.numWaypoints = settings.value(KEY_NUM_WAYPOINTS).toInt();
    if (settings.contains(KEY_DEFAULT_ALTITUDE))
        params.defaultAltitude = settings.value(KEY_DEFAULT_ALTITUDE).toDouble();
    if (settings.contains(KEY_CURVE_FACTOR))
        params.curveFactor = settings.value(KEY_CURVE_FACTOR).toDouble();
    if (settings.contains(KEY_SPIRAL_TURNS))
        params.spiralTurns = settings.value(KEY_SPIRAL_TURNS).toDouble();
    if (settings.contains(KEY_ZIGZAG_AMPLITUDE))
        params.zigzagAmplitude = settings.value(KEY_ZIGZAG_AMPLITUDE).toDouble();
    if (settings.contains(KEY_ZIGZAG_FREQUENCY))
        params.zigzagFrequency = settings.value(KEY_ZIGZAG_FREQUENCY).toInt();
    if (settings.contains(KEY_MAX_TURN_RADIUS))
        params.maxTurnRadius = settings.value(KEY_MAX_TURN_RADIUS).toDouble();
    if (settings.contains(KEY_RANDOM_VARIANCE))
        params.randomVariance = settings.value(KEY_RANDOM_VARIANCE).toDouble();
    if (settings.contains(KEY_SPREAD_RADIUS_KM))
        params.spreadRadiusKm = settings.value(KEY_SPREAD_RADIUS_KM).toDouble();
    
    settings.endGroup();
    
    return params;
}
