#include "cpathsettingsdialog.h"
#include "ui_cpathsettingsdialog.h"
#include "MapDisplay/caipathgenerator.h"
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
static const QString KEY_RANDOMNESS_LEVEL = "randomnessLevel";

// AI-specific settings keys
static const QString KEY_GENERATION_METHOD = "generationMethod";
static const QString KEY_AI_MISSION_TYPE = "aiMissionType";
static const QString KEY_AI_AVOID_DETECTION = "aiAvoidDetection";
static const QString KEY_AI_TERRAIN_FOLLOWING = "aiTerrainFollowing";
static const QString KEY_AI_MIN_ALTITUDE = "aiMinAltitude";
static const QString KEY_AI_MAX_ALTITUDE = "aiMaxAltitude";
static const QString KEY_AI_THREAT_RADIUS = "aiThreatRadius";
static const QString KEY_AI_OPTIMIZE_FUEL = "aiOptimizeFuel";

CPathSettingsDialog::CPathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPathSettingsDialog)
{
    ui->setupUi(this);
    
    // Initialize default parameters from saved settings (or hardcoded defaults)
    _m_defaultParams = loadDefaultParametersFromSettings();
    
    // Load defaults to UI
    loadParametersToUI(_m_defaultParams);
    
    // Connect AI mode toggle
    connect(ui->radioButton_AI, &QRadioButton::toggled, this, &CPathSettingsDialog::on_radioButton_AI_toggled);
    connect(ui->comboBox_MissionType, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CPathSettingsDialog::on_comboBox_MissionType_currentIndexChanged);
    
    // Set initial UI state
    updateUIState();
    
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
    // Conventional parameters
    ui->spinBox_NumWaypoints->setValue(params.numWaypoints);
    ui->doubleSpinBox_Altitude->setValue(params.defaultAltitude);
    ui->doubleSpinBox_CurveFactor->setValue(params.curveFactor);
    ui->doubleSpinBox_SpiralTurns->setValue(params.spiralTurns);
    ui->doubleSpinBox_ZigzagAmplitude->setValue(params.zigzagAmplitude);
    ui->spinBox_ZigzagFrequency->setValue(params.zigzagFrequency);
    ui->doubleSpinBox_MaxTurnRadius->setValue(params.maxTurnRadius);
    ui->doubleSpinBox_RandomVariance->setValue(params.randomVariance);
    ui->doubleSpinBox_SpreadRadius->setValue(params.spreadRadiusKm);
    ui->doubleSpinBox_RandomnessLevel->setValue(params.randomnessLevel);
    
    // Generation method
    if (params.generationMethod == PATH_METHOD_AI) {
        ui->radioButton_AI->setChecked(true);
    } else {
        ui->radioButton_Conventional->setChecked(true);
    }
    
    // AI-specific parameters
    ui->comboBox_MissionType->setCurrentIndex(static_cast<int>(params.aiMissionType));
    ui->doubleSpinBox_AIMinAlt->setValue(params.aiMinAltitude);
    ui->doubleSpinBox_AIMaxAlt->setValue(params.aiMaxAltitude);
    ui->doubleSpinBox_ThreatRadius->setValue(params.aiThreatRadius);
    ui->checkBox_AvoidDetection->setChecked(params.aiAvoidDetection);
    ui->checkBox_TerrainFollowing->setChecked(params.aiTerrainFollowing);
    ui->checkBox_OptimizeFuel->setChecked(params.aiOptimizeForFuel);
    
    updateUIState();
}

CPathGenerator::PathParameters CPathSettingsDialog::getParametersFromUI() const
{
    CPathGenerator::PathParameters params;
    
    // Conventional parameters
    params.numWaypoints = ui->spinBox_NumWaypoints->value();
    params.defaultAltitude = ui->doubleSpinBox_Altitude->value();
    params.curveFactor = ui->doubleSpinBox_CurveFactor->value();
    params.spiralTurns = ui->doubleSpinBox_SpiralTurns->value();
    params.zigzagAmplitude = ui->doubleSpinBox_ZigzagAmplitude->value();
    params.zigzagFrequency = ui->spinBox_ZigzagFrequency->value();
    params.maxTurnRadius = ui->doubleSpinBox_MaxTurnRadius->value();
    params.randomVariance = ui->doubleSpinBox_RandomVariance->value();
    params.spreadRadiusKm = ui->doubleSpinBox_SpreadRadius->value();
    params.randomnessLevel = ui->doubleSpinBox_RandomnessLevel->value();
    
    // Generation method
    params.generationMethod = ui->radioButton_AI->isChecked() ? PATH_METHOD_AI : PATH_METHOD_CONVENTIONAL;
    
    // AI-specific parameters
    params.aiMissionType = static_cast<eVISTAR_AI_MISSION_TYPE>(ui->comboBox_MissionType->currentIndex());
    params.aiMinAltitude = ui->doubleSpinBox_AIMinAlt->value();
    params.aiMaxAltitude = ui->doubleSpinBox_AIMaxAlt->value();
    params.aiThreatRadius = ui->doubleSpinBox_ThreatRadius->value();
    params.aiAvoidDetection = ui->checkBox_AvoidDetection->isChecked();
    params.aiTerrainFollowing = ui->checkBox_TerrainFollowing->isChecked();
    params.aiOptimizeForFuel = ui->checkBox_OptimizeFuel->isChecked();
    
    return params;
}

bool CPathSettingsDialog::isAIModeSelected() const
{
    return ui->radioButton_AI->isChecked();
}

eVISTAR_AI_MISSION_TYPE CPathSettingsDialog::getSelectedAIMissionType() const
{
    return static_cast<eVISTAR_AI_MISSION_TYPE>(ui->comboBox_MissionType->currentIndex());
}

void CPathSettingsDialog::on_radioButton_AI_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateUIState();
    updateMissionDescription();
}

void CPathSettingsDialog::on_comboBox_MissionType_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateMissionDescription();
}

void CPathSettingsDialog::updateUIState()
{
    bool isAIMode = ui->radioButton_AI->isChecked();
    
    // Enable/disable AI settings group
    ui->groupBox_AISettings->setEnabled(isAIMode);
    
    // Update method description
    if (isAIMode) {
        ui->label_MethodDescription->setText(
            "🤖 AI Mode: Generates intelligent mission-aware paths with threat avoidance, "
            "terrain following, and mission-specific optimization.");
    } else {
        ui->label_MethodDescription->setText(
            "⚙️ Conventional Mode: Uses mathematical algorithms (curves, spirals, zigzags) "
            "for predictable, pattern-based path generation.");
    }
}

void CPathSettingsDialog::updateMissionDescription()
{
    if (!ui->radioButton_AI->isChecked()) {
        return;
    }
    
    eVISTAR_AI_MISSION_TYPE missionType = getSelectedAIMissionType();
    QString description = CAIPathGenerator::getMissionTypeDescription(missionType);
    
    // Could update a description label here if added to UI
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
    
    // Conventional parameters
    settings.setValue(KEY_NUM_WAYPOINTS, params.numWaypoints);
    settings.setValue(KEY_DEFAULT_ALTITUDE, params.defaultAltitude);
    settings.setValue(KEY_CURVE_FACTOR, params.curveFactor);
    settings.setValue(KEY_SPIRAL_TURNS, params.spiralTurns);
    settings.setValue(KEY_ZIGZAG_AMPLITUDE, params.zigzagAmplitude);
    settings.setValue(KEY_ZIGZAG_FREQUENCY, params.zigzagFrequency);
    settings.setValue(KEY_MAX_TURN_RADIUS, params.maxTurnRadius);
    settings.setValue(KEY_RANDOM_VARIANCE, params.randomVariance);
    settings.setValue(KEY_SPREAD_RADIUS_KM, params.spreadRadiusKm);
    settings.setValue(KEY_RANDOMNESS_LEVEL, params.randomnessLevel);
    
    // AI-specific parameters
    settings.setValue(KEY_GENERATION_METHOD, static_cast<int>(params.generationMethod));
    settings.setValue(KEY_AI_MISSION_TYPE, static_cast<int>(params.aiMissionType));
    settings.setValue(KEY_AI_AVOID_DETECTION, params.aiAvoidDetection);
    settings.setValue(KEY_AI_TERRAIN_FOLLOWING, params.aiTerrainFollowing);
    settings.setValue(KEY_AI_MIN_ALTITUDE, params.aiMinAltitude);
    settings.setValue(KEY_AI_MAX_ALTITUDE, params.aiMaxAltitude);
    settings.setValue(KEY_AI_THREAT_RADIUS, params.aiThreatRadius);
    settings.setValue(KEY_AI_OPTIMIZE_FUEL, params.aiOptimizeForFuel);
    
    settings.endGroup();
    settings.sync();
}

CPathGenerator::PathParameters CPathSettingsDialog::loadDefaultParametersFromSettings() const
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    // Conventional parameters - only override if the setting exists
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
    if (settings.contains(KEY_RANDOMNESS_LEVEL))
        params.randomnessLevel = settings.value(KEY_RANDOMNESS_LEVEL).toDouble();
    
    // AI-specific parameters
    if (settings.contains(KEY_GENERATION_METHOD))
        params.generationMethod = static_cast<eVISTAR_PATH_METHOD>(settings.value(KEY_GENERATION_METHOD).toInt());
    if (settings.contains(KEY_AI_MISSION_TYPE))
        params.aiMissionType = static_cast<eVISTAR_AI_MISSION_TYPE>(settings.value(KEY_AI_MISSION_TYPE).toInt());
    if (settings.contains(KEY_AI_AVOID_DETECTION))
        params.aiAvoidDetection = settings.value(KEY_AI_AVOID_DETECTION).toBool();
    if (settings.contains(KEY_AI_TERRAIN_FOLLOWING))
        params.aiTerrainFollowing = settings.value(KEY_AI_TERRAIN_FOLLOWING).toBool();
    if (settings.contains(KEY_AI_MIN_ALTITUDE))
        params.aiMinAltitude = settings.value(KEY_AI_MIN_ALTITUDE).toDouble();
    if (settings.contains(KEY_AI_MAX_ALTITUDE))
        params.aiMaxAltitude = settings.value(KEY_AI_MAX_ALTITUDE).toDouble();
    if (settings.contains(KEY_AI_THREAT_RADIUS))
        params.aiThreatRadius = settings.value(KEY_AI_THREAT_RADIUS).toDouble();
    if (settings.contains(KEY_AI_OPTIMIZE_FUEL))
        params.aiOptimizeForFuel = settings.value(KEY_AI_OPTIMIZE_FUEL).toBool();
    
    settings.endGroup();
    
    return params;
}

CPathGenerator::PathParameters CPathSettingsDialog::getSavedDefaultParameters()
{
    CPathGenerator::PathParameters params;  // Start with hardcoded defaults
    QSettings settings;
    
    settings.beginGroup(SETTINGS_GROUP);
    
    // Conventional parameters
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
    if (settings.contains(KEY_RANDOMNESS_LEVEL))
        params.randomnessLevel = settings.value(KEY_RANDOMNESS_LEVEL).toDouble();
    
    // AI-specific parameters
    if (settings.contains(KEY_GENERATION_METHOD))
        params.generationMethod = static_cast<eVISTAR_PATH_METHOD>(settings.value(KEY_GENERATION_METHOD).toInt());
    if (settings.contains(KEY_AI_MISSION_TYPE))
        params.aiMissionType = static_cast<eVISTAR_AI_MISSION_TYPE>(settings.value(KEY_AI_MISSION_TYPE).toInt());
    if (settings.contains(KEY_AI_AVOID_DETECTION))
        params.aiAvoidDetection = settings.value(KEY_AI_AVOID_DETECTION).toBool();
    if (settings.contains(KEY_AI_TERRAIN_FOLLOWING))
        params.aiTerrainFollowing = settings.value(KEY_AI_TERRAIN_FOLLOWING).toBool();
    if (settings.contains(KEY_AI_MIN_ALTITUDE))
        params.aiMinAltitude = settings.value(KEY_AI_MIN_ALTITUDE).toDouble();
    if (settings.contains(KEY_AI_MAX_ALTITUDE))
        params.aiMaxAltitude = settings.value(KEY_AI_MAX_ALTITUDE).toDouble();
    if (settings.contains(KEY_AI_THREAT_RADIUS))
        params.aiThreatRadius = settings.value(KEY_AI_THREAT_RADIUS).toDouble();
    if (settings.contains(KEY_AI_OPTIMIZE_FUEL))
        params.aiOptimizeForFuel = settings.value(KEY_AI_OPTIMIZE_FUEL).toBool();
    
    settings.endGroup();
    
    return params;
}
