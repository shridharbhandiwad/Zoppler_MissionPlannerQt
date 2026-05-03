#ifndef RADARATTRIBUTESDIALOG_H
#define RADARATTRIBUTESDIALOG_H

#include <QDialog>
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
// An attribute-table editor that opens when the user right-clicks a RADAR
// object on the map and selects "Radar Parameters".
//
// Contains a single QTableWidget (Attribute / Value / Unit) displaying only
// the RadarPhysicsParameters (RF/signal-processing / radar-equation values).
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
    // Build the Radar Parameters table page
    QWidget *buildRadarParametersTab();

    // Helper to read values back from the table widget
    RadarView::RadarPhysicsParameters  readPhysics() const;

    // Generic helper: add a row with name + value to a table
    static void addRow(QTableWidget *tbl, const QString &name,
                       const QVariant &value,
                       const QString &unit  = QString(),
                       const QString &tooltip = QString());

    // Apply any uncommitted edits to m_attrs
    void collectEdits();

    RadarView::RadarAttributes m_attrs;
    RadarView::RadarAttributes m_originalAttrs;   // kept for Reset

    QTableWidget  *m_tblPhysics;

    QPushButton   *m_btnOk;
    QPushButton   *m_btnApply;
    QPushButton   *m_btnCancel;
    QPushButton   *m_btnReset;
};

#endif // RADARATTRIBUTESDIALOG_H
