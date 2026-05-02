#ifndef CCLUTTERPARAMSDIALOG_H
#define CCLUTTERPARAMSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QString>

// Parameters for Clutter object
struct ClutterParams {
    QString type        = "GROUND";   // GROUND | RAIN | SEA | CHAFF
    int     radiusM     = 1000;
    double  sigma0Db    = -20.0;
    QString surfaceType = "URBAN";    // URBAN, FARMLAND, FOREST, DESERT, RAIN
    QString statModel   = "WEIBULL";
    double  weibullC    = 1.5;
    double  windSpeedMs = 5.0;
    double  windDirDeg  = 0.0;
    double  dopplerSpreadMs = 0.5;
};

// Parameters for Clutter Density object
struct ClutterDensityParams {
    QString preset       = "SPARSE";  // SPARSE | LOW | MEDIUM | HIGH | EXTREME
    int     groundPatches = -1;
    int     rainPatches   = -1;
    double  cellsScale    = -1.0;
    double  pfa           = -1.0;
};

class CClutterParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CClutterParamsDialog(QWidget *parent = nullptr);

    ClutterParams        getClutterParams()        const;
    ClutterDensityParams getClutterDensityParams() const;

private:
    void buildClutterTab(QWidget *tab);
    void buildDensityTab(QWidget *tab);

    // Clutter controls
    QComboBox       *_cmbType;
    QSpinBox        *_spnRadius;
    QDoubleSpinBox  *_spnSigma0;
    QComboBox       *_cmbSurface;
    QComboBox       *_cmbStatModel;
    QDoubleSpinBox  *_spnWeibullC;
    QDoubleSpinBox  *_spnWindSpeed;
    QDoubleSpinBox  *_spnWindDir;
    QDoubleSpinBox  *_spnDoppler;

    // Density controls
    QComboBox       *_cmbPreset;
    QSpinBox        *_spnGroundPatches;
    QSpinBox        *_spnRainPatches;
    QDoubleSpinBox  *_spnCellsScale;
    QDoubleSpinBox  *_spnPfa;
};

#endif // CCLUTTERPARAMSDIALOG_H
