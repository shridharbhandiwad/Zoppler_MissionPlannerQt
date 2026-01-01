#include "cpathsettingsdialog.h"
#include "ui_cpathsettingsdialog.h"
#include <QMessageBox>
#include <QSettings>

// Settings keys for path parameters
static const QString SETTINGS_GROUP = "PathGeneratorDefaults";
static const QString KEY_NUM_WAYPOINTS = "numWaypoints";
static const QString KEY_DEFAULT_ALTITUDE = "defaultAltitude";
static const QString KEY_CURVE_FACTOR = "curveFactor";
static const QString KEY_SPREAD_RADIUS_KM = "spreadRadiusKm";

// Spiral parameters
static const QString KEY_SPIRAL_TURNS = "spiralTurns";
static const QString KEY_SPIRAL_EXPANSION_RATE = "spiralExpansionRate";
static const QString KEY_SPIRAL_TIGHTNESS = "spiralTightness";
static const QString KEY_SPIRAL_CLOCKWISE = "spiralClockwise";

// Zigzag parameters
static const QString KEY_ZIGZAG_AMPLITUDE = "zigzagAmplitude";
static const QString KEY_ZIGZAG_FREQUENCY = "zigzagFrequency";

// Turn parameters
static const QString KEY_MAX_TURN_RADIUS = "maxTurnRadius";

// Randomness parameters
static const QString KEY_RANDOM_VARIANCE = "randomVariance";
static const QString KEY_RANDOMNESS_LEVEL = "randomnessLevel";
static const QString KEY_NOISE_SCALE = "noiseScale";
static const QString KEY_ALTITUDE_VARIATION = "altitudeVariation";
static const QString KEY_WOBBLE_INTENSITY = "wobbleIntensity";
static const QString KEY_PATH_ASYMMETRY = "pathAsymmetry";
static const QString KEY_CLUSTER_TENDENCY = "clusterTendency";

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
    // General parameters
    ui->spinBox_NumWaypoints->setValue(params.numWaypoints);
    ui->doubleSpinBox_Altitude->setValue(params.defaultAltitude);
    ui->doubleSpinBox_CurveFactor->setValue(params.curveFactor);
    ui->doubleSpinBox_SpreadRadius->setValue(params.spreadRadiusKm);
    
    // Spiral parameters
    ui->doubleSpinBox_SpiralTurns->setValue(params.spiralTurns);
    ui->doubleSpinBox_SpiralExpansion->setValue(params.spiralExpansionRate);
    ui->doubleSpinBox_SpiralTightness->setValue(params.spiralTightness);
    ui->checkBox_SpiralClockwise->setChecked(params.spiralClockwise);
    
    // Zigzag parameters
    ui->doubleSpinBox_ZigzagAmplitude->setValue(params.zigzagAmplitude);
    ui->spinBox_ZigzagFrequency->setValue(params.zigzagFrequency);
    
    // Turn parameters
    ui->doubleSpinBox_MaxTurnRadius->setValue(params.maxTurnRadius);
    
    // Randomness parameters
    ui->doubleSpinBox_RandomVariance->setValue(params.randomVariance);
    ui->doubleSpinBox_RandomnessLevel->setValue(params.randomnessLevel);
    ui->doubleSpinBox_NoiseScale->setValue(params.noiseScale);
    ui->doubleSpinBox_AltitudeVariation->setValue(params.altitudeVariation);
    ui->doubleSpinBox_WobbleIntensity->setValue(params.wobbleIntensity);
    ui->doubleSpinBox_PathAsymmetry->setValue(params.pathAsymmetry);
    ui->doubleSpinBox_ClusterTendency->setValue(params.clusterTendency);
}

CPathGenerator::PathParameters CPathSettingsDialog::getParametersFromUI() const
{
    CPathGenerator::PathParameters params;
    
    // General parameters
    params.numWaypoints = ui->spinBox_NumWaypoints->value();
    params.defaultAltitude = ui->doubleSpinBox_Altitude->value();
    params.curveFactor = ui->doubleSpinBox_CurveFactor->value();
    params.spreadRadiusKm = ui->doubleSpinBox_SpreadRadius->value();
    
    // Spiral parameters
    params.spiralTurns = ui->doubleSpinBox_SpiralTurns->value();
    params.spiralExpansionRate = ui->doubleSpinBox_SpiralExpansion->value();
    params.spiralTightness = ui->doubleSpinBox_SpiralTightness->value();
    params.spiralClockwise = ui->checkBox_SpiralClockwise->isChecked();
    
    // Zigzag parameters
    params.zigzagAmplitude = ui->doubleSpinBox_ZigzagAmplitude->value();
    params.zigzagFrequency = ui->spinBox_ZigzagFrequency->value();
    
    // Turn parameters
    params.maxTurnRadius = ui->doubleSpinBox_MaxTurnRadius->value();
    
    // Randomness parameters
    params.randomVariance = ui->doubleSpinBox_RandomVariance->value();
    params.randomnessLevel = ui->doubleSpinBox_RandomnessLevel->value();
    params.noiseScale = ui->doubleSpinBox_NoiseScale->value();
    params.altitudeVariation = ui->doubleSpinBox_AltitudeVariation->value();
    params.wobbleIntensity = ui->doubleSpinBox_WobbleIntensity->value();
    params.pathAsymmetry = ui->doubleSpinBox_PathAsymmetry->value();
    params.clusterTendency = ui->doubleSpinBox_ClusterTendency->value();
    
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
    
    // General parameters
    settings.setValue(KEY_NUM_WAYPOINTS, params.numWaypoints);
    settings.setValue(KEY_DEFAULT_ALTITUDE, params.defaultAltitude);
    settings.setValue(KEY_CURVE_FACTOR, params.curveFactor);
    settings.setValue(KEY_SPREAD_RADIUS_KM, params.spreadRadiusKm);
    
    // Spiral parameters
    settings.setValue(KEY_SPIRAL_TURNS, params.spiralTurns);
    settings.setValue(KEY_SPIRAL_EXPANSION_RATE, params.spiralExpansionRate);
    settings.setValue(KEY_SPIRAL_TIGHTNESS, params.spiralTightness);
    settings.setValue(KEY_SPIRAL_CLOCKWISE, params.spiralClockwise);
    
    // Zigzag parameters
    settings.setValue(KEY_ZIGZAG_AMPLITUDE, params.zigzagAmplitude);
    settings.setValue(KEY_ZIGZAG_FREQUENCY, params.zigzagFrequency);
    
    // Turn parameters
    settings.setValue(KEY_MAX_TURN_RADIUS, params.maxTurnRadius);
    
    // Randomness parameters
    settings.setValue(KEY_RANDOM_VARIANCE, params.randomVariance);
    settings.setValue(KEY_RANDOMNESS_LEVEL, params.randomnessLevel);
    settings.setValue(KEY_NOISE_SCALE, params.noiseScale);
    settings.setValue(KEY_ALTITUDE_VARIATION, params.altitudeVariation);
    settings.setValue(KEY_WOBBLE_INTENSITY, params.wobbleIntensity);
    settings.setValue(KEY_PATH_ASYMMETRY, params.pathAsymmetry);
    settings.setValue(KEY_CLUSTER_TENDENCY, params.clusterTendency);
    
    settings.endGroup();
    settings.sync();
}

CPathGenerator::PathParameters CPathSettingsDialog::loadDefaultParametersFromSettings() const
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    // General parameters - only override if the setting exists
    if (settings.contains(KEY_NUM_WAYPOINTS))
        params.numWaypoints = settings.value(KEY_NUM_WAYPOINTS).toInt();
    if (settings.contains(KEY_DEFAULT_ALTITUDE))
        params.defaultAltitude = settings.value(KEY_DEFAULT_ALTITUDE).toDouble();
    if (settings.contains(KEY_CURVE_FACTOR))
        params.curveFactor = settings.value(KEY_CURVE_FACTOR).toDouble();
    if (settings.contains(KEY_SPREAD_RADIUS_KM))
        params.spreadRadiusKm = settings.value(KEY_SPREAD_RADIUS_KM).toDouble();
    
    // Spiral parameters
    if (settings.contains(KEY_SPIRAL_TURNS))
        params.spiralTurns = settings.value(KEY_SPIRAL_TURNS).toDouble();
    if (settings.contains(KEY_SPIRAL_EXPANSION_RATE))
        params.spiralExpansionRate = settings.value(KEY_SPIRAL_EXPANSION_RATE).toDouble();
    if (settings.contains(KEY_SPIRAL_TIGHTNESS))
        params.spiralTightness = settings.value(KEY_SPIRAL_TIGHTNESS).toDouble();
    if (settings.contains(KEY_SPIRAL_CLOCKWISE))
        params.spiralClockwise = settings.value(KEY_SPIRAL_CLOCKWISE).toBool();
    
    // Zigzag parameters
    if (settings.contains(KEY_ZIGZAG_AMPLITUDE))
        params.zigzagAmplitude = settings.value(KEY_ZIGZAG_AMPLITUDE).toDouble();
    if (settings.contains(KEY_ZIGZAG_FREQUENCY))
        params.zigzagFrequency = settings.value(KEY_ZIGZAG_FREQUENCY).toInt();
    
    // Turn parameters
    if (settings.contains(KEY_MAX_TURN_RADIUS))
        params.maxTurnRadius = settings.value(KEY_MAX_TURN_RADIUS).toDouble();
    
    // Randomness parameters
    if (settings.contains(KEY_RANDOM_VARIANCE))
        params.randomVariance = settings.value(KEY_RANDOM_VARIANCE).toDouble();
    if (settings.contains(KEY_RANDOMNESS_LEVEL))
        params.randomnessLevel = settings.value(KEY_RANDOMNESS_LEVEL).toDouble();
    if (settings.contains(KEY_NOISE_SCALE))
        params.noiseScale = settings.value(KEY_NOISE_SCALE).toDouble();
    if (settings.contains(KEY_ALTITUDE_VARIATION))
        params.altitudeVariation = settings.value(KEY_ALTITUDE_VARIATION).toDouble();
    if (settings.contains(KEY_WOBBLE_INTENSITY))
        params.wobbleIntensity = settings.value(KEY_WOBBLE_INTENSITY).toDouble();
    if (settings.contains(KEY_PATH_ASYMMETRY))
        params.pathAsymmetry = settings.value(KEY_PATH_ASYMMETRY).toDouble();
    if (settings.contains(KEY_CLUSTER_TENDENCY))
        params.clusterTendency = settings.value(KEY_CLUSTER_TENDENCY).toDouble();
    
    settings.endGroup();
    
    return params;
}

CPathGenerator::PathParameters CPathSettingsDialog::getSavedDefaultParameters()
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    // General parameters
    if (settings.contains(KEY_NUM_WAYPOINTS))
        params.numWaypoints = settings.value(KEY_NUM_WAYPOINTS).toInt();
    if (settings.contains(KEY_DEFAULT_ALTITUDE))
        params.defaultAltitude = settings.value(KEY_DEFAULT_ALTITUDE).toDouble();
    if (settings.contains(KEY_CURVE_FACTOR))
        params.curveFactor = settings.value(KEY_CURVE_FACTOR).toDouble();
    if (settings.contains(KEY_SPREAD_RADIUS_KM))
        params.spreadRadiusKm = settings.value(KEY_SPREAD_RADIUS_KM).toDouble();
    
    // Spiral parameters
    if (settings.contains(KEY_SPIRAL_TURNS))
        params.spiralTurns = settings.value(KEY_SPIRAL_TURNS).toDouble();
    if (settings.contains(KEY_SPIRAL_EXPANSION_RATE))
        params.spiralExpansionRate = settings.value(KEY_SPIRAL_EXPANSION_RATE).toDouble();
    if (settings.contains(KEY_SPIRAL_TIGHTNESS))
        params.spiralTightness = settings.value(KEY_SPIRAL_TIGHTNESS).toDouble();
    if (settings.contains(KEY_SPIRAL_CLOCKWISE))
        params.spiralClockwise = settings.value(KEY_SPIRAL_CLOCKWISE).toBool();
    
    // Zigzag parameters
    if (settings.contains(KEY_ZIGZAG_AMPLITUDE))
        params.zigzagAmplitude = settings.value(KEY_ZIGZAG_AMPLITUDE).toDouble();
    if (settings.contains(KEY_ZIGZAG_FREQUENCY))
        params.zigzagFrequency = settings.value(KEY_ZIGZAG_FREQUENCY).toInt();
    
    // Turn parameters
    if (settings.contains(KEY_MAX_TURN_RADIUS))
        params.maxTurnRadius = settings.value(KEY_MAX_TURN_RADIUS).toDouble();
    
    // Randomness parameters
    if (settings.contains(KEY_RANDOM_VARIANCE))
        params.randomVariance = settings.value(KEY_RANDOM_VARIANCE).toDouble();
    if (settings.contains(KEY_RANDOMNESS_LEVEL))
        params.randomnessLevel = settings.value(KEY_RANDOMNESS_LEVEL).toDouble();
    if (settings.contains(KEY_NOISE_SCALE))
        params.noiseScale = settings.value(KEY_NOISE_SCALE).toDouble();
    if (settings.contains(KEY_ALTITUDE_VARIATION))
        params.altitudeVariation = settings.value(KEY_ALTITUDE_VARIATION).toDouble();
    if (settings.contains(KEY_WOBBLE_INTENSITY))
        params.wobbleIntensity = settings.value(KEY_WOBBLE_INTENSITY).toDouble();
    if (settings.contains(KEY_PATH_ASYMMETRY))
        params.pathAsymmetry = settings.value(KEY_PATH_ASYMMETRY).toDouble();
    if (settings.contains(KEY_CLUSTER_TENDENCY))
        params.clusterTendency = settings.value(KEY_CLUSTER_TENDENCY).toDouble();
    
    settings.endGroup();
    
    return params;
}
