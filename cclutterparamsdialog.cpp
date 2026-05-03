#include "cclutterparamsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

static const char *kStyle = R"(
QDialog {
    background-color: #2a2a38;
    color: #d0d8e8;
    font-size: 12px;
}
QTabWidget::pane {
    border: 1px solid #3a4560;
    border-radius: 4px;
    background-color: #2a2a38;
}
QTabBar::tab {
    background-color: #3a3a50;
    color: #8a9abc;
    padding: 7px 18px;
    border: 1px solid #3a4560;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    font-weight: bold;
}
QTabBar::tab:selected {
    background-color: #3a5080;
    color: #e0e8ff;
    border-bottom: 2px solid #4a90d9;
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
QComboBox, QSpinBox, QDoubleSpinBox {
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

CClutterParamsDialog::CClutterParamsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Clutter Parameters");
    setStyleSheet(kStyle);
    setMinimumWidth(380);

    QWidget *clutterTab  = new QWidget;
    buildClutterTab(clutterTab);

    QDialogButtonBox *bbox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(clutterTab);
    mainLayout->addWidget(bbox);
    setLayout(mainLayout);
}

void CClutterParamsDialog::buildClutterTab(QWidget *tab)
{
    QFormLayout *form = new QFormLayout(tab);
    form->setSpacing(8);

    _cmbType = new QComboBox(tab);
    _cmbType->addItems({"GROUND", "RAIN", "SEA", "CHAFF"});
    form->addRow("Type:", _cmbType);

    _spnRadius = new QSpinBox(tab);
    _spnRadius->setRange(1, 100000);
    _spnRadius->setValue(1000);
    _spnRadius->setSuffix(" m");
    form->addRow("Radius:", _spnRadius);

    _spnSigma0 = new QDoubleSpinBox(tab);
    _spnSigma0->setRange(-60.0, 20.0);
    _spnSigma0->setValue(-20.0);
    _spnSigma0->setSuffix(" dB");
    _spnSigma0->setDecimals(1);
    form->addRow("Sigma0:", _spnSigma0);

    _cmbSurface = new QComboBox(tab);
    _cmbSurface->addItems({"URBAN", "FARMLAND", "FOREST", "DESERT", "RAIN"});
    form->addRow("Surface Type:", _cmbSurface);

    _cmbStatModel = new QComboBox(tab);
    _cmbStatModel->addItems({"WEIBULL", "RAYLEIGH", "LOG_NORMAL"});
    form->addRow("Stat Model:", _cmbStatModel);

    _spnWeibullC = new QDoubleSpinBox(tab);
    _spnWeibullC->setRange(0.1, 10.0);
    _spnWeibullC->setValue(1.5);
    _spnWeibullC->setDecimals(2);
    form->addRow("Weibull C:", _spnWeibullC);

    _spnWindSpeed = new QDoubleSpinBox(tab);
    _spnWindSpeed->setRange(0.0, 100.0);
    _spnWindSpeed->setValue(5.0);
    _spnWindSpeed->setSuffix(" m/s");
    _spnWindSpeed->setDecimals(1);
    form->addRow("Wind Speed:", _spnWindSpeed);

    _spnWindDir = new QDoubleSpinBox(tab);
    _spnWindDir->setRange(0.0, 360.0);
    _spnWindDir->setValue(0.0);
    _spnWindDir->setSuffix(" deg");
    _spnWindDir->setDecimals(1);
    form->addRow("Wind Dir:", _spnWindDir);

    _spnDoppler = new QDoubleSpinBox(tab);
    _spnDoppler->setRange(0.0, 10.0);
    _spnDoppler->setValue(0.5);
    _spnDoppler->setSuffix(" m/s");
    _spnDoppler->setDecimals(2);
    form->addRow("Doppler Spread:", _spnDoppler);

    tab->setLayout(form);
}

ClutterParams CClutterParamsDialog::getClutterParams() const
{
    ClutterParams p;
    p.type           = _cmbType->currentText();
    p.radiusM        = _spnRadius->value();
    p.sigma0Db       = _spnSigma0->value();
    p.surfaceType    = _cmbSurface->currentText();
    p.statModel      = _cmbStatModel->currentText();
    p.weibullC       = _spnWeibullC->value();
    p.windSpeedMs    = _spnWindSpeed->value();
    p.windDirDeg     = _spnWindDir->value();
    p.dopplerSpreadMs = _spnDoppler->value();
    return p;
}

