#ifndef CCOVERAGEPARAMSDIALOG_H
#define CCOVERAGEPARAMSDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QDialogButtonBox>
#include "RadarView/radarattributes.h"

// ---------------------------------------------------------------------------
// CCoverageParamsDialog
//
// Opened from the radar right-click context menu ("Update Coverage Parameters").
// Lets the operator edit the five coverage parameters plus the range-ring
// spacing used on the map canvas and in the PPI view.
// ---------------------------------------------------------------------------
class CCoverageParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CCoverageParamsDialog(const QString &radarId,
                                   const RadarView::RadarCoverageParameters &params,
                                   QWidget *parent = nullptr);

    RadarView::RadarCoverageParameters coverageParams() const;

signals:
    void coverageApplied(RadarView::RadarCoverageParameters params);

private slots:
    void onApply();
    void onOk();

private:
    QGroupBox *buildForm();

    QString               m_radarId;
    RadarView::RadarCoverageParameters m_params;

    QDoubleSpinBox *m_spnMaxRange    = nullptr;
    QDoubleSpinBox *m_spnMinAz       = nullptr;
    QDoubleSpinBox *m_spnMaxAz       = nullptr;
    QDoubleSpinBox *m_spnMinEl       = nullptr;
    QDoubleSpinBox *m_spnMaxEl       = nullptr;
    QDoubleSpinBox *m_spnRingSpacing = nullptr;
};

#endif // CCOVERAGEPARAMSDIALOG_H
