#ifndef RADARATTRIBUTESDIALOG_H
#define RADARATTRIBUTESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include "radarattributes.h"

// ---------------------------------------------------------------------------
// RadarAttributesDialog
//
// A tabbed attribute-table editor that opens when the user right-clicks a
// RADAR object on the map and selects "Attributes".
//
// Layout:
//   Tab 1 – Design Attributes      (static / HW parameters)
//   Tab 2 – Operational Attributes (live / mission-time parameters)
//   Tab 3 – Maintenance Attributes (health / logistics)
//
// Each tab contains a QTableWidget with two columns:
//   Column 0 – Attribute name  (read-only)
//   Column 1 – Value           (editable)
//
// The dialog works entirely in "edit mode": all value cells are editable
// from the moment it opens.  Changes are committed with OK / Apply, or
// discarded with Cancel.
// ---------------------------------------------------------------------------

class RadarAttributesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RadarAttributesDialog(const QString &radarName,
                                   const RadarView::RadarAttributes &attrs,
                                   QWidget *parent = nullptr);

    // Returns the (possibly edited) attributes after the dialog is accepted.
    RadarView::RadarAttributes attributes() const { return m_attrs; }

signals:
    // Emitted when the user presses Apply (without closing) or OK.
    void attributesApplied(RadarView::RadarAttributes attrs);

private slots:
    void onApply();
    void onOk();
    void onReset();

private:
    // Build individual tab pages
    QWidget *buildDesignTab();
    QWidget *buildOperationalTab();
    QWidget *buildMaintenanceTab();

    // Helpers to read values back from the table widgets
    RadarView::DesignAttributes      readDesign()      const;
    RadarView::OperationalAttributes readOperational() const;
    RadarView::MaintenanceAttributes readMaintenance() const;

    // Generic helper: add a row with name + value to a table
    static void addRow(QTableWidget *tbl, const QString &name,
                       const QVariant &value,
                       const QString &unit  = QString(),
                       const QString &tooltip = QString());

    // Apply any uncommitted edits to m_attrs
    void collectEdits();

    RadarView::RadarAttributes m_attrs;
    RadarView::RadarAttributes m_originalAttrs;   // kept for Reset

    QTabWidget    *m_tabs;
    QTableWidget  *m_tblDesign;
    QTableWidget  *m_tblOp;
    QTableWidget  *m_tblMaint;

    QPushButton   *m_btnOk;
    QPushButton   *m_btnApply;
    QPushButton   *m_btnCancel;
    QPushButton   *m_btnReset;
};

#endif // RADARATTRIBUTESDIALOG_H
