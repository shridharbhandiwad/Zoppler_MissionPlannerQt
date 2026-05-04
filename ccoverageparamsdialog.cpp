#include "ccoverageparamsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

static const char *kDlgStyle = R"(
QDialog {
    background-color: #2a2a38;
    color: #d0d8e8;
    font-size: 12px;
}
QGroupBox {
    color: #8ab4e0;
    border: 1px solid #3a4560;
    border-radius: 4px;
    margin-top: 12px;
    padding: 8px;
    font-weight: bold;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QLabel { color: #c0cce0; }
QDoubleSpinBox {
    background-color: #1a2035;
    color: #c8d4e8;
    border: 1px solid #3a4560;
    border-radius: 3px;
    padding: 4px 6px;
    min-width: 140px;
}
QPushButton {
    background-color: #2a5080;
    color: #d0e4ff;
    border: 1px solid #3a6090;
    border-radius: 4px;
    padding: 6px 16px;
    font-weight: bold;
}
QPushButton:hover { background-color: #3a6090; }
)";

CCoverageParamsDialog::CCoverageParamsDialog(const QString &radarId,
                                             const RadarView::RadarCoverageParameters &params,
                                             QWidget *parent)
    : QDialog(parent)
    , m_radarId(radarId)
    , m_params(params)
{
    setWindowTitle(QString("Coverage Parameters – %1").arg(radarId));
    setStyleSheet(kDlgStyle);
    setMinimumWidth(360);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGroupBox *grp = buildForm();
    mainLayout->addWidget(grp);

    // Button row
    QPushButton *btnOk     = new QPushButton("OK",     this);
    QPushButton *btnApply  = new QPushButton("Apply",  this);
    QPushButton *btnCancel = new QPushButton("Cancel", this);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnApply);
    btnRow->addWidget(btnCancel);

    mainLayout->addLayout(btnRow);
    setLayout(mainLayout);

    connect(btnOk,     &QPushButton::clicked, this, &CCoverageParamsDialog::onOk);
    connect(btnApply,  &QPushButton::clicked, this, &CCoverageParamsDialog::onApply);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QGroupBox *CCoverageParamsDialog::buildForm()
{
    QGroupBox  *grp  = new QGroupBox("Coverage Parameters", this);
    QFormLayout *form = new QFormLayout(grp);
    form->setSpacing(8);

    // Max Range
    m_spnMaxRange = new QDoubleSpinBox(grp);
    m_spnMaxRange->setRange(1.0, 5000.0);
    m_spnMaxRange->setDecimals(1);
    m_spnMaxRange->setSuffix(" km");
    m_spnMaxRange->setValue(m_params.maxRangeKm);
    form->addRow("Max Range:", m_spnMaxRange);

    // Min Azimuth
    m_spnMinAz = new QDoubleSpinBox(grp);
    m_spnMinAz->setRange(0.0, 360.0);
    m_spnMinAz->setDecimals(1);
    m_spnMinAz->setSuffix(" deg");
    m_spnMinAz->setValue(m_params.minAzimuthDeg);
    form->addRow("Min Azimuth:", m_spnMinAz);

    // Max Azimuth
    m_spnMaxAz = new QDoubleSpinBox(grp);
    m_spnMaxAz->setRange(0.0, 360.0);
    m_spnMaxAz->setDecimals(1);
    m_spnMaxAz->setSuffix(" deg");
    m_spnMaxAz->setValue(m_params.maxAzimuthDeg);
    form->addRow("Max Azimuth:", m_spnMaxAz);

    // Min Elevation
    m_spnMinEl = new QDoubleSpinBox(grp);
    m_spnMinEl->setRange(-90.0, 90.0);
    m_spnMinEl->setDecimals(1);
    m_spnMinEl->setSuffix(" deg");
    m_spnMinEl->setValue(m_params.minElevationDeg);
    form->addRow("Min Elevation:", m_spnMinEl);

    // Max Elevation
    m_spnMaxEl = new QDoubleSpinBox(grp);
    m_spnMaxEl->setRange(-90.0, 90.0);
    m_spnMaxEl->setDecimals(1);
    m_spnMaxEl->setSuffix(" deg");
    m_spnMaxEl->setValue(m_params.maxElevationDeg);
    form->addRow("Max Elevation:", m_spnMaxEl);

    // Range Ring Spacing
    m_spnRingSpacing = new QDoubleSpinBox(grp);
    m_spnRingSpacing->setRange(1.0, 500.0);
    m_spnRingSpacing->setDecimals(1);
    m_spnRingSpacing->setSuffix(" km");
    m_spnRingSpacing->setValue(m_params.rangeRingSpacingKm);
    form->addRow("Range Ring Spacing:", m_spnRingSpacing);

    grp->setLayout(form);
    return grp;
}

RadarView::RadarCoverageParameters CCoverageParamsDialog::coverageParams() const
{
    RadarView::RadarCoverageParameters p;
    p.maxRangeKm         = m_spnMaxRange->value();
    p.minAzimuthDeg      = m_spnMinAz->value();
    p.maxAzimuthDeg      = m_spnMaxAz->value();
    p.minElevationDeg    = m_spnMinEl->value();
    p.maxElevationDeg    = m_spnMaxEl->value();
    p.rangeRingSpacingKm = m_spnRingSpacing->value();
    return p;
}

void CCoverageParamsDialog::onApply()
{
    m_params = coverageParams();
    emit coverageApplied(m_params);
}

void CCoverageParamsDialog::onOk()
{
    onApply();
    accept();
}
