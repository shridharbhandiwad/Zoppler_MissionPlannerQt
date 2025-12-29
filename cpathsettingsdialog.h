#ifndef CPATHSETTINGSDIALOG_H
#define CPATHSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include "MapDisplay/cpathgenerator.h"

namespace Ui {
class CPathSettingsDialog;
}

/**
 * @brief Dialog for configuring path generation parameters
 * 
 * This dialog allows users to customize various parameters used
 * in path/trajectory generation algorithms. Users can choose between
 * conventional (algorithmic) and AI-powered path generation methods.
 * Default parameters can be saved and persisted across application sessions.
 */
class CPathSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPathSettingsDialog(QWidget *parent = nullptr);
    ~CPathSettingsDialog();

    /**
     * @brief Set the current path parameters to display in the dialog
     */
    void setPathParameters(const CPathGenerator::PathParameters &params);

    /**
     * @brief Get the configured path parameters from the dialog
     */
    CPathGenerator::PathParameters getPathParameters() const;

    /**
     * @brief Get the saved default parameters (from QSettings or hardcoded defaults)
     */
    static CPathGenerator::PathParameters getSavedDefaultParameters();
    
    /**
     * @brief Check if AI generation mode is selected
     */
    bool isAIModeSelected() const;
    
    /**
     * @brief Get the selected AI mission type
     */
    eVISTAR_AI_MISSION_TYPE getSelectedAIMissionType() const;

private slots:
    void on_pushButton_Apply_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_ResetDefaults_clicked();
    void on_pushButton_SaveAsDefaults_clicked();
    void on_radioButton_AI_toggled(bool checked);
    void on_comboBox_MissionType_currentIndexChanged(int index);

private:
    Ui::CPathSettingsDialog *ui;
    CPathGenerator::PathParameters _m_defaultParams;

    void loadParametersToUI(const CPathGenerator::PathParameters &params);
    CPathGenerator::PathParameters getParametersFromUI() const;
    
    /**
     * @brief Save path parameters to QSettings for persistence
     */
    void saveDefaultParametersToSettings(const CPathGenerator::PathParameters &params);
    
    /**
     * @brief Load path parameters from QSettings
     * @return Saved parameters, or hardcoded defaults if none saved
     */
    CPathGenerator::PathParameters loadDefaultParametersFromSettings() const;
    
    /**
     * @brief Update UI state based on generation method selection
     */
    void updateUIState();
    
    /**
     * @brief Update the mission type description label
     */
    void updateMissionDescription();
};

#endif // CPATHSETTINGSDIALOG_H
