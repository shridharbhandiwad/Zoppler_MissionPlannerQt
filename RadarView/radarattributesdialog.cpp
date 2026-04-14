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
QTabWidget::pane {
    border: 1px solid #3a4560;
    border-radius: 4px;
    background-color: #1e2430;
}
QTabBar::tab {
    background-color: #2a3550;
    color: #8a9abc;
    padding: 8px 20px;
    border: 1px solid #3a4560;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    min-width: 140px;
    font-weight: bold;
}
QTabBar::tab:selected {
    background-color: #334070;
    color: #e0e8ff;
    border-bottom: 2px solid #4a90d9;
}
QTabBar::tab:hover:!selected {
    background-color: #2f3d60;
    color: #c0cce0;
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
    setWindowTitle(QString("Radar Attributes – %1").arg(radarName));
    setMinimumSize(720, 580);
    setStyleSheet(kDialogStyle);
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(8);

    // Title bar
    auto *title = new QLabel(
        QString("<b>Radar Attributes</b>  <span style='color:#8ab4e0; font-size:12px;'>%1</span>")
            .arg(radarName), this);
    title->setObjectName("titleLabel");
    root->addWidget(title);

    // Tabs
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(buildDesignTab(),      "  Design  ");
    m_tabs->addTab(buildOperationalTab(), "  Operational  ");
    m_tabs->addTab(buildMaintenanceTab(), "  Maintenance  ");
    root->addWidget(m_tabs, 1);

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

// ─── Design tab ───────────────────────────────────────────────────────────────
QWidget *RadarAttributesDialog::buildDesignTab()
{
    auto *w = new QWidget;
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(4, 4, 4, 4);

    m_tblDesign = makeTable(w);
    const auto &d = m_attrs.design;

    addRow(m_tblDesign, "Radar Type",          d.radarType,            "",     "Surveillance | Fire Control | Weather | SAR | Tracking");
    addRow(m_tblDesign, "Manufacturer",         d.manufacturer);
    addRow(m_tblDesign, "Model Designation",    d.modelDesignation);
    addRow(m_tblDesign, "Frequency",            d.frequencyMHz,         "MHz",  "Centre operating frequency");
    addRow(m_tblDesign, "Bandwidth",            d.bandwidthMHz,         "MHz");
    addRow(m_tblDesign, "Polarization",         d.polarization,         "",     "Linear | Circular | Dual");
    addRow(m_tblDesign, "Max Range",            d.maxRangeKm,           "km");
    addRow(m_tblDesign, "Min Range",            d.minRangeKm,           "km");
    addRow(m_tblDesign, "Azimuth Coverage",     d.azimuthCovDeg,        "°",    "360 = full omnidirectional");
    addRow(m_tblDesign, "Elevation Min",        d.elevationMinDeg,      "°");
    addRow(m_tblDesign, "Elevation Max",        d.elevationMaxDeg,      "°");
    addRow(m_tblDesign, "Peak Power",           d.peakPowerKw,          "kW");
    addRow(m_tblDesign, "Range Resolution",     d.rangeResolutionM,     "m");
    addRow(m_tblDesign, "Azimuth Resolution",   d.azimuthResolutionDeg, "°");
    addRow(m_tblDesign, "Antenna Height",       d.antennaHeightM,       "m");
    addRow(m_tblDesign, "Weight",               d.weightKg,             "kg");
    addRow(m_tblDesign, "Mounting Type",        d.mountingType,         "",     "Fixed | Mobile | Ship-borne | Airborne");

    l->addWidget(m_tblDesign);
    return w;
}

// ─── Operational tab ──────────────────────────────────────────────────────────
QWidget *RadarAttributesDialog::buildOperationalTab()
{
    auto *w = new QWidget;
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(4, 4, 4, 4);

    m_tblOp = makeTable(w);
    const auto &op = m_attrs.operational;

    addRow(m_tblOp, "Operational Mode",    op.operationalMode,    "",  "Standby | Active | Maintenance | Degraded | Off");
    addRow(m_tblOp, "Assigned Mission",    op.assignedMission);
    addRow(m_tblOp, "Track Capacity",      op.trackCapacity,      "tracks");
    addRow(m_tblOp, "Current Tracks",      op.currentTracks,      "tracks");
    addRow(m_tblOp, "EMCON Active",        op.emconActive ? "true" : "false", "", "Emission control (silent mode)");
    addRow(m_tblOp, "Transmit Power",      op.transmitPowerPct,   "%");
    addRow(m_tblOp, "Scan Rate",           op.scanRateDegPerSec,  "°/s");
    addRow(m_tblOp, "Sector Start",        op.sectorStartDeg,     "°");
    addRow(m_tblOp, "Sector End",          op.sectorEndDeg,       "°");
    addRow(m_tblOp, "IFF Enabled",         op.iffEnabled ? "true" : "false");
    addRow(m_tblOp, "IFF Mode",            op.iffMode,            "",  "Mode-1 | Mode-2 | Mode-3 | Mode-S");
    addRow(m_tblOp, "Jamming Detected",    op.jammingDetected ? "true" : "false");
    addRow(m_tblOp, "Jam Signal",          op.jamSignalDbm,       "dBm");

    l->addWidget(m_tblOp);
    return w;
}

// ─── Maintenance tab ──────────────────────────────────────────────────────────
QWidget *RadarAttributesDialog::buildMaintenanceTab()
{
    auto *w = new QWidget;
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(4, 4, 4, 4);

    m_tblMaint = makeTable(w);
    const auto &m = m_attrs.maintenance;

    addRow(m_tblMaint, "System Health",       m.systemHealth,    "",  "Nominal | Degraded | Critical | Failed");
    addRow(m_tblMaint, "Health",              m.healthPct,       "%");
    addRow(m_tblMaint, "Transmitter OK",      m.transmitterOk  ? "true" : "false");
    addRow(m_tblMaint, "Receiver OK",         m.receiverOk     ? "true" : "false");
    addRow(m_tblMaint, "Antenna OK",          m.antennaOk      ? "true" : "false");
    addRow(m_tblMaint, "Cooling System OK",   m.coolingSysOk   ? "true" : "false");
    addRow(m_tblMaint, "Power Supply OK",     m.powerSupplyOk  ? "true" : "false");
    addRow(m_tblMaint, "Last Service Date",   m.lastServiceDate);
    addRow(m_tblMaint, "Next Service Date",   m.nextServiceDate);
    addRow(m_tblMaint, "Operating Hours",     m.operatingHours,  "h");
    addRow(m_tblMaint, "Service Interval",    m.serviceIntervalH,"h");
    addRow(m_tblMaint, "MTBF",                m.mtbfHours,       "h");
    addRow(m_tblMaint, "MTTR",                m.mttrHours,       "h");
    addRow(m_tblMaint, "Notes",               m.maintenanceNotes);

    l->addWidget(m_tblMaint);
    return w;
}

// ─── collect edits from all three tables ─────────────────────────────────────
static QString cellText(QTableWidget *tbl, int row)
{
    auto *it = tbl->item(row, 1);
    return it ? it->text().trimmed() : QString();
}

RadarView::DesignAttributes RadarAttributesDialog::readDesign() const
{
    RadarView::DesignAttributes d;
    d.radarType            = cellText(m_tblDesign,  0);
    d.manufacturer         = cellText(m_tblDesign,  1);
    d.modelDesignation     = cellText(m_tblDesign,  2);
    d.frequencyMHz         = cellText(m_tblDesign,  3).toDouble();
    d.bandwidthMHz         = cellText(m_tblDesign,  4).toDouble();
    d.polarization         = cellText(m_tblDesign,  5);
    d.maxRangeKm           = cellText(m_tblDesign,  6).toDouble();
    d.minRangeKm           = cellText(m_tblDesign,  7).toDouble();
    d.azimuthCovDeg        = cellText(m_tblDesign,  8).toDouble();
    d.elevationMinDeg      = cellText(m_tblDesign,  9).toDouble();
    d.elevationMaxDeg      = cellText(m_tblDesign, 10).toDouble();
    d.peakPowerKw          = cellText(m_tblDesign, 11).toDouble();
    d.rangeResolutionM     = cellText(m_tblDesign, 12).toDouble();
    d.azimuthResolutionDeg = cellText(m_tblDesign, 13).toDouble();
    d.antennaHeightM       = cellText(m_tblDesign, 14).toDouble();
    d.weightKg             = cellText(m_tblDesign, 15).toDouble();
    d.mountingType         = cellText(m_tblDesign, 16);
    return d;
}

RadarView::OperationalAttributes RadarAttributesDialog::readOperational() const
{
    RadarView::OperationalAttributes op;
    op.operationalMode  = cellText(m_tblOp,  0);
    op.assignedMission  = cellText(m_tblOp,  1);
    op.trackCapacity    = cellText(m_tblOp,  2).toInt();
    op.currentTracks    = cellText(m_tblOp,  3).toInt();
    op.emconActive      = (cellText(m_tblOp, 4).toLower() == "true");
    op.transmitPowerPct = cellText(m_tblOp,  5).toDouble();
    op.scanRateDegPerSec= cellText(m_tblOp,  6).toDouble();
    op.sectorStartDeg   = cellText(m_tblOp,  7).toDouble();
    op.sectorEndDeg     = cellText(m_tblOp,  8).toDouble();
    op.iffEnabled       = (cellText(m_tblOp, 9).toLower() == "true");
    op.iffMode          = cellText(m_tblOp, 10);
    op.jammingDetected  = (cellText(m_tblOp,11).toLower() == "true");
    op.jamSignalDbm     = cellText(m_tblOp, 12).toDouble();
    return op;
}

RadarView::MaintenanceAttributes RadarAttributesDialog::readMaintenance() const
{
    RadarView::MaintenanceAttributes m;
    m.systemHealth     = cellText(m_tblMaint,  0);
    m.healthPct        = cellText(m_tblMaint,  1).toDouble();
    m.transmitterOk    = (cellText(m_tblMaint, 2).toLower() == "true");
    m.receiverOk       = (cellText(m_tblMaint, 3).toLower() == "true");
    m.antennaOk        = (cellText(m_tblMaint, 4).toLower() == "true");
    m.coolingSysOk     = (cellText(m_tblMaint, 5).toLower() == "true");
    m.powerSupplyOk    = (cellText(m_tblMaint, 6).toLower() == "true");
    m.lastServiceDate  = cellText(m_tblMaint,  7);
    m.nextServiceDate  = cellText(m_tblMaint,  8);
    m.operatingHours   = cellText(m_tblMaint,  9).toInt();
    m.serviceIntervalH = cellText(m_tblMaint, 10).toInt();
    m.mtbfHours        = cellText(m_tblMaint, 11).toDouble();
    m.mttrHours        = cellText(m_tblMaint, 12).toDouble();
    m.maintenanceNotes = cellText(m_tblMaint, 13);
    return m;
}

void RadarAttributesDialog::collectEdits()
{
    m_attrs.design      = readDesign();
    m_attrs.operational = readOperational();
    m_attrs.maintenance = readMaintenance();
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

    // Rebuild each tab to reflect defaults
    // Easier: close & re-open the dialog in reset state.
    // We instead update each cell directly.
    auto setCell = [](QTableWidget *tbl, int row, const QString &val) {
        if (auto *it = tbl->item(row, 1)) it->setText(val);
    };

    const auto &d  = m_attrs.design;
    const auto &op = m_attrs.operational;
    const auto &ma = m_attrs.maintenance;

    // Design
    setCell(m_tblDesign,  0, d.radarType);
    setCell(m_tblDesign,  1, d.manufacturer);
    setCell(m_tblDesign,  2, d.modelDesignation);
    setCell(m_tblDesign,  3, QString::number(d.frequencyMHz));
    setCell(m_tblDesign,  4, QString::number(d.bandwidthMHz));
    setCell(m_tblDesign,  5, d.polarization);
    setCell(m_tblDesign,  6, QString::number(d.maxRangeKm));
    setCell(m_tblDesign,  7, QString::number(d.minRangeKm));
    setCell(m_tblDesign,  8, QString::number(d.azimuthCovDeg));
    setCell(m_tblDesign,  9, QString::number(d.elevationMinDeg));
    setCell(m_tblDesign, 10, QString::number(d.elevationMaxDeg));
    setCell(m_tblDesign, 11, QString::number(d.peakPowerKw));
    setCell(m_tblDesign, 12, QString::number(d.rangeResolutionM));
    setCell(m_tblDesign, 13, QString::number(d.azimuthResolutionDeg));
    setCell(m_tblDesign, 14, QString::number(d.antennaHeightM));
    setCell(m_tblDesign, 15, QString::number(d.weightKg));
    setCell(m_tblDesign, 16, d.mountingType);

    // Operational
    setCell(m_tblOp,  0, op.operationalMode);
    setCell(m_tblOp,  1, op.assignedMission);
    setCell(m_tblOp,  2, QString::number(op.trackCapacity));
    setCell(m_tblOp,  3, QString::number(op.currentTracks));
    setCell(m_tblOp,  4, op.emconActive    ? "true" : "false");
    setCell(m_tblOp,  5, QString::number(op.transmitPowerPct));
    setCell(m_tblOp,  6, QString::number(op.scanRateDegPerSec));
    setCell(m_tblOp,  7, QString::number(op.sectorStartDeg));
    setCell(m_tblOp,  8, QString::number(op.sectorEndDeg));
    setCell(m_tblOp,  9, op.iffEnabled    ? "true" : "false");
    setCell(m_tblOp, 10, op.iffMode);
    setCell(m_tblOp, 11, op.jammingDetected ? "true" : "false");
    setCell(m_tblOp, 12, QString::number(op.jamSignalDbm));

    // Maintenance
    setCell(m_tblMaint,  0, ma.systemHealth);
    setCell(m_tblMaint,  1, QString::number(ma.healthPct));
    setCell(m_tblMaint,  2, ma.transmitterOk  ? "true" : "false");
    setCell(m_tblMaint,  3, ma.receiverOk     ? "true" : "false");
    setCell(m_tblMaint,  4, ma.antennaOk      ? "true" : "false");
    setCell(m_tblMaint,  5, ma.coolingSysOk   ? "true" : "false");
    setCell(m_tblMaint,  6, ma.powerSupplyOk  ? "true" : "false");
    setCell(m_tblMaint,  7, ma.lastServiceDate);
    setCell(m_tblMaint,  8, ma.nextServiceDate);
    setCell(m_tblMaint,  9, QString::number(ma.operatingHours));
    setCell(m_tblMaint, 10, QString::number(ma.serviceIntervalH));
    setCell(m_tblMaint, 11, QString::number(ma.mtbfHours));
    setCell(m_tblMaint, 12, QString::number(ma.mttrHours));
    setCell(m_tblMaint, 13, ma.maintenanceNotes);
}
