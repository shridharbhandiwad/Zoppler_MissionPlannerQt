#include "radarattributesdialog.h"

#include <QHeaderView>
#include <QFont>
#include <QColor>
#include <QStringList>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QFrame>

// ─── stylesheet ──────────────────────────────────────────────────────────────
static const char *kDialogStyle = R"(
QDialog {
    background-color: #1e2430;
    color: #d0d8e8;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 12px;
}
QTableWidget {
    background-color: #1a2035;
    alternate-background-color: #1e263c;
    color: #c8d4e8;
    gridline-color: #2a3450;
    border: none;
    font-size: 12px;
    selection-background-color: #2a5080;
    selection-color: #ffffff;
}
QTableWidget::item {
    padding: 5px 8px;
    border-bottom: 1px solid #252f48;
}
QHeaderView::section {
    background-color: #253050;
    color: #8ab4e0;
    font-weight: bold;
    font-size: 11px;
    padding: 6px 10px;
    border: none;
    border-right: 1px solid #3a4560;
    border-bottom: 1px solid #3a4560;
}
QPushButton {
    background-color: #2a5080;
    color: #d0e4ff;
    border: 1px solid #3a6090;
    border-radius: 4px;
    padding: 7px 18px;
    font-weight: bold;
    font-size: 11px;
    min-width: 80px;
}
QPushButton:hover {
    background-color: #3a6090;
    border-color: #5a80c0;
}
QPushButton:pressed {
    background-color: #1a3060;
}
QPushButton#applyBtn {
    background-color: #1a5030;
    border-color: #2a7040;
}
QPushButton#applyBtn:hover {
    background-color: #2a7040;
}
QPushButton#resetBtn {
    background-color: #503018;
    border-color: #705028;
}
QPushButton#resetBtn:hover {
    background-color: #705028;
}
QLabel#titleLabel {
    color: #4a90d9;
    font-size: 14px;
    font-weight: bold;
    padding: 6px 0px 10px 0px;
}
)";

// ─── helper: create a styled table ───────────────────────────────────────────
static QTableWidget *makeTable(QWidget *parent)
{
    auto *t = new QTableWidget(0, 3, parent);
    t->setHorizontalHeaderLabels({"Attribute", "Value", "Unit"});
    t->verticalHeader()->setVisible(false);
    t->setAlternatingRowColors(true);
    t->setSelectionBehavior(QAbstractItemView::SelectItems);
    t->setEditTriggers(QAbstractItemView::AllEditTriggers);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    t->setShowGrid(true);
    t->setWordWrap(false);
    return t;
}

// ─── helper: add a row ───────────────────────────────────────────────────────
void RadarAttributesDialog::addRow(QTableWidget *tbl, const QString &name,
                                   const QVariant &value,
                                   const QString &unit,
                                   const QString &tooltip)
{
    int row = tbl->rowCount();
    tbl->insertRow(row);

    // Column 0 – name (read-only)
    auto *nameItem = new QTableWidgetItem(name);
    nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    nameItem->setForeground(QColor("#8ab4e0"));
    if (!tooltip.isEmpty()) nameItem->setToolTip(tooltip);
    tbl->setItem(row, 0, nameItem);

    // Column 1 – value (editable)
    auto *valItem = new QTableWidgetItem(value.toString());
    valItem->setForeground(QColor("#d0e4ff"));
    if (!tooltip.isEmpty()) valItem->setToolTip(tooltip);
    tbl->setItem(row, 1, valItem);

    // Column 2 – unit (read-only)
    auto *unitItem = new QTableWidgetItem(unit);
    unitItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    unitItem->setForeground(QColor("#607898"));
    tbl->setItem(row, 2, unitItem);
}

// ─── ctor ─────────────────────────────────────────────────────────────────────
RadarAttributesDialog::RadarAttributesDialog(const QString &radarName,
                                              const RadarView::RadarAttributes &attrs,
                                              QWidget *parent)
    : QDialog(parent)
    , m_attrs(attrs)
    , m_originalAttrs(attrs)
{
    setWindowTitle(QString("Radar Parameters – %1").arg(radarName));
    setMinimumSize(720, 580);
    setStyleSheet(kDialogStyle);
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(8);

    // Title bar
    auto *title = new QLabel(
        QString("<b>Radar Parameters</b>  <span style='color:#8ab4e0; font-size:12px;'>%1</span>")
            .arg(radarName), this);
    title->setObjectName("titleLabel");
    root->addWidget(title);

    // Radar Parameters table (no tabs – single view)
    root->addWidget(buildRadarParametersTab(), 1);

    // Button row
    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);

    m_btnReset  = new QPushButton("Reset to Defaults", this);
    m_btnReset->setObjectName("resetBtn");
    btnRow->addWidget(m_btnReset);

    btnRow->addStretch();

    m_btnApply  = new QPushButton("Apply", this);
    m_btnApply->setObjectName("applyBtn");
    m_btnOk     = new QPushButton("OK", this);
    m_btnCancel = new QPushButton("Cancel", this);

    btnRow->addWidget(m_btnApply);
    btnRow->addWidget(m_btnOk);
    btnRow->addWidget(m_btnCancel);
    root->addLayout(btnRow);

    connect(m_btnOk,     &QPushButton::clicked, this, &RadarAttributesDialog::onOk);
    connect(m_btnApply,  &QPushButton::clicked, this, &RadarAttributesDialog::onApply);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnReset,  &QPushButton::clicked, this, &RadarAttributesDialog::onReset);
}

// ─── Radar Parameters table ───────────────────────────────────────────────────
QWidget *RadarAttributesDialog::buildRadarParametersTab()
{
    auto *w = new QWidget;
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(4, 4, 4, 4);

    m_tblPhysics = makeTable(w);
    const auto &p = m_attrs.physics;

    addRow(m_tblPhysics, "Pd",            p.Pd,            "",    "Probability of detection (0–1)");
    addRow(m_tblPhysics, "Pfa",           p.Pfa,           "",    "Probability of false alarm (e.g. 1e-6)");
    addRow(m_tblPhysics, "Beamwidth Az",  p.beamwidth_az,  "°",   "Azimuth beamwidth (degrees)");
    addRow(m_tblPhysics, "Beamwidth El",  p.beamwidth_el,  "°",   "Elevation beamwidth (degrees)");
    addRow(m_tblPhysics, "Tx Power",      p.tx_power,      "W",   "Transmit power (watts)");
    addRow(m_tblPhysics, "Pulse Width",   p.pulse_width,   "s",   "Pulse width (seconds, e.g. 100e-6)");
    addRow(m_tblPhysics, "Bandwidth",     p.bandwidth,     "Hz",  "Signal bandwidth (Hz, e.g. 5e6)");
    addRow(m_tblPhysics, "Freq Min",      p.freq_min,      "Hz",  "Minimum frequency (Hz, e.g. 10.0e9)");
    addRow(m_tblPhysics, "Freq Max",      p.freq_max,      "Hz",  "Maximum frequency (Hz, e.g. 10.5e9)");
    addRow(m_tblPhysics, "Freq Center",   p.freq_center,   "Hz",  "Centre frequency – auto-computed as (Freq Min + Freq Max) / 2");
    addRow(m_tblPhysics, "Desired SNR",   p.desired_snr,   "dB",  "Required SNR at detection threshold");
    addRow(m_tblPhysics, "Noise Figure",  p.noise_figure,  "dB",  "Receiver noise figure");
    addRow(m_tblPhysics, "System Temp",   p.system_temp,   "K",   "System noise temperature (Kelvin)");
    addRow(m_tblPhysics, "Receiver Gain", p.receiver_gain, "dB",  "Receiver gain");
    addRow(m_tblPhysics, "PRF",           p.prf,           "Hz",  "Pulse repetition frequency");
    addRow(m_tblPhysics, "Scan Time",     p.scan_time,     "s",   "Time for one full scan (seconds)");
    addRow(m_tblPhysics, "RCS",           p.rcs,           "m²",  "Reference target radar cross-section");
    addRow(m_tblPhysics, "Loss",          p.loss,          "dB",  "Total system loss");

    l->addWidget(m_tblPhysics);
    return w;
}

// ─── collect edits from table ─────────────────────────────────────────────────
static QString cellText(QTableWidget *tbl, int row)
{
    auto *it = tbl->item(row, 1);
    return it ? it->text().trimmed() : QString();
}

RadarView::RadarPhysicsParameters RadarAttributesDialog::readPhysics() const
{
    RadarView::RadarPhysicsParameters p;
    p.Pd            = cellText(m_tblPhysics,  0).toDouble();
    p.Pfa           = cellText(m_tblPhysics,  1).toDouble();
    p.beamwidth_az  = cellText(m_tblPhysics,  2).toDouble();
    p.beamwidth_el  = cellText(m_tblPhysics,  3).toDouble();
    p.tx_power      = cellText(m_tblPhysics,  4).toDouble();
    p.pulse_width   = cellText(m_tblPhysics,  5).toDouble();
    p.bandwidth     = cellText(m_tblPhysics,  6).toDouble();
    p.freq_min      = cellText(m_tblPhysics,  7).toDouble();
    p.freq_max      = cellText(m_tblPhysics,  8).toDouble();
    // Row 9 (Freq Center) is derived; recompute rather than trust user input
    p.freq_center   = (p.freq_min + p.freq_max) / 2.0;
    p.desired_snr   = cellText(m_tblPhysics, 10).toDouble();
    p.noise_figure  = cellText(m_tblPhysics, 11).toDouble();
    p.system_temp   = cellText(m_tblPhysics, 12).toDouble();
    p.receiver_gain = cellText(m_tblPhysics, 13).toDouble();
    p.prf           = cellText(m_tblPhysics, 14).toDouble();
    p.scan_time     = cellText(m_tblPhysics, 15).toDouble();
    p.rcs           = cellText(m_tblPhysics, 16).toDouble();
    p.loss          = cellText(m_tblPhysics, 17).toDouble();
    return p;
}

void RadarAttributesDialog::collectEdits()
{
    m_attrs.physics = readPhysics();
    // Keep freq_center consistent after a manual freq_min/freq_max edit
    m_attrs.physics.updateDerivedFields();
}

// ─── button handlers ─────────────────────────────────────────────────────────
void RadarAttributesDialog::onApply()
{
    collectEdits();
    emit attributesApplied(m_attrs);
}

void RadarAttributesDialog::onOk()
{
    collectEdits();
    emit attributesApplied(m_attrs);
    accept();
}

void RadarAttributesDialog::onReset()
{
    m_attrs = RadarView::RadarAttributes::defaults();

    auto setCell = [](QTableWidget *tbl, int row, const QString &val) {
        if (auto *it = tbl->item(row, 1)) it->setText(val);
    };

    const auto &ph = m_attrs.physics;

    setCell(m_tblPhysics,  0, QString::number(ph.Pd));
    setCell(m_tblPhysics,  1, QString::number(ph.Pfa));
    setCell(m_tblPhysics,  2, QString::number(ph.beamwidth_az));
    setCell(m_tblPhysics,  3, QString::number(ph.beamwidth_el));
    setCell(m_tblPhysics,  4, QString::number(ph.tx_power));
    setCell(m_tblPhysics,  5, QString::number(ph.pulse_width));
    setCell(m_tblPhysics,  6, QString::number(ph.bandwidth));
    setCell(m_tblPhysics,  7, QString::number(ph.freq_min));
    setCell(m_tblPhysics,  8, QString::number(ph.freq_max));
    setCell(m_tblPhysics,  9, QString::number((ph.freq_min + ph.freq_max) / 2.0));
    setCell(m_tblPhysics, 10, QString::number(ph.desired_snr));
    setCell(m_tblPhysics, 11, QString::number(ph.noise_figure));
    setCell(m_tblPhysics, 12, QString::number(ph.system_temp));
    setCell(m_tblPhysics, 13, QString::number(ph.receiver_gain));
    setCell(m_tblPhysics, 14, QString::number(ph.prf));
    setCell(m_tblPhysics, 15, QString::number(ph.scan_time));
    setCell(m_tblPhysics, 16, QString::number(ph.rcs));
    setCell(m_tblPhysics, 17, QString::number(ph.loss));
}
