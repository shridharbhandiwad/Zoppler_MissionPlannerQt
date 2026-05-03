#ifndef RADARDISPLAYPANEL_H
#define RADARDISPLAYPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QTabBar>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMap>
#include "radarmanager.h"
#include "radarppiwidget.h"

// ─── RadarPPIWindow ──────────────────────────────────────────────────────────
// Standalone top-level window for a single radar PPI.
// Emits reattachRequested(radarId) when the user wants it back in the panel.
class RadarPPIWindow : public QWidget
{
    Q_OBJECT
public:
    explicit RadarPPIWindow(const RadarView::Radar &radar, QWidget *parent = nullptr);

    int radarId() const { return m_radarId; }
    void updateRadar(const RadarView::Radar &radar);

signals:
    void reattachRequested(int radarId);
    void windowClosed(int radarId);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    int              m_radarId;
    RadarPPIWidget  *m_ppiWidget;
    QPushButton     *m_reattachBtn;
    QString          m_radarName;
};

// ─── RadarDisplayPanel ───────────────────────────────────────────────────────
// Large tabbed panel that hosts PPI widgets for one or more radars.
// Designed to be placed inside a QDockWidget (filling most of the screen).
// Radars can be opened/closed; each tab can be detached to its own window.
class RadarDisplayPanel : public QWidget
{
    Q_OBJECT
public:
    explicit RadarDisplayPanel(QWidget *parent = nullptr);

    // Open (or bring to front) the PPI for a radar
    void openRadar(const RadarView::Radar &radar);

    // Push fresh data to any currently visible tab / window
    void updateRadar(const RadarView::Radar &radar);

    // Close / remove a radar (e.g. on scenario reset)
    void closeRadar(int radarId);
    void closeAll();

    bool hasRadar(int radarId) const;

signals:
    void panelClosed();   // emitted when the last tab is removed

private slots:
    void onTabCloseRequested(int index);
    void onDetachCurrentTab();
    void onDetachTab(int radarId);
    void onReattachRadar(int radarId);
    void onWindowClosed(int radarId);
    void onCurrentTabChanged(int index);
    void onTabBarDoubleClicked(int index);

private:
    struct TabEntry {
        int             radarId;
        RadarPPIWidget *ppiWidget;   // lives inside tab (or null if detached)
        RadarView::Radar radarData;
    };

    struct WindowEntry {
        RadarPPIWindow *window;
        RadarView::Radar radarData;
    };

    void buildUI();
    void showPlaceholder(bool show);
    int  tabIndexForRadar(int radarId) const;

    QTabWidget   *m_tabs;
    QToolBar     *m_toolbar;
    QAction      *m_actDetach;
    QAction      *m_actDetachAll;
    QLabel       *m_placeholder;

    QMap<int, TabEntry>    m_tabEntries;     // radarId → tab data
    QMap<int, WindowEntry> m_windowEntries;  // radarId → detached window
};

#endif // RADARDISPLAYPANEL_H
