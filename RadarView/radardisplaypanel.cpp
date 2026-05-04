#include "radardisplaypanel.h"

#include <QCloseEvent>
#include <QSizePolicy>
#include <QApplication>
#include <QScreen>
#include <QToolButton>
#include <QFrame>

// ─── style constants ────────────────────────────────────────────────────────

static const QString kPanelStyle = R"(
QWidget#RadarDisplayPanel {
    background-color: #0a0f14;
}
QTabWidget::pane {
    border: 1px solid #2a3a2a;
    background-color: #0a0f14;
}
QTabWidget::tab-bar {
    left: 0;
}
QTabBar::tab {
    background-color: #1a2a1a;
    color: #88ccaa;
    border: 1px solid #2a3a2a;
    border-bottom: none;
    padding: 6px 14px;
    font-size: 11px;
    font-family: Monospace;
    min-width: 120px;
}
QTabBar::tab:selected {
    background-color: #0a1a0a;
    color: #00ff88;
    border-top: 2px solid #00ff88;
}
QTabBar::tab:hover:!selected {
    background-color: #162816;
    color: #aaffcc;
}
QTabBar::tab:first {
    border-left: 1px solid #2a3a2a;
}
QToolBar {
    background-color: #0d1a0d;
    border-bottom: 1px solid #2a3a2a;
    spacing: 2px;
    padding: 2px 4px;
}
QToolBar QToolButton {
    background-color: transparent;
    color: #88ccaa;
    border: 1px solid transparent;
    border-radius: 3px;
    padding: 3px 8px;
    font-size: 10px;
    font-family: Monospace;
}
QToolBar QToolButton:hover {
    background-color: rgba(0,255,136,0.12);
    border-color: #00aa55;
    color: #00ff88;
}
QToolBar QToolButton:pressed {
    background-color: rgba(0,255,136,0.22);
}
QToolBar QToolButton:disabled {
    color: #445544;
}
QLabel#placeholder {
    color: #334433;
    font-size: 14px;
    font-family: Monospace;
}
)";

static const QString kWindowStyle = R"(
QWidget {
    background-color: #0a0f14;
}
QPushButton#reattachBtn {
    background-color: #1a2a1a;
    color: #88ccaa;
    border: 1px solid #2a4a2a;
    border-radius: 3px;
    padding: 3px 10px;
    font-size: 10px;
    font-family: Monospace;
}
QPushButton#reattachBtn:hover {
    background-color: rgba(0,255,136,0.15);
    color: #00ff88;
    border-color: #00aa55;
}
)";

// ════════════════════════════════════════════════════════════════════════════
//  RadarPPIWindow
// ════════════════════════════════════════════════════════════════════════════

RadarPPIWindow::RadarPPIWindow(const RadarView::Radar &radar, QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_radarId(radar.radarId)
    , m_radarName(radar.radarName)
{
    setObjectName("RadarPPIWindow");
    setWindowTitle(QString("[R%1] %2  —  PPI").arg(radar.radarId).arg(radar.radarName));
    setAttribute(Qt::WA_DeleteOnClose, false);
    setMinimumSize(400, 420);
    resize(620, 640);
    setStyleSheet(kWindowStyle);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Thin header bar with "Re-attach" button
    auto *header = new QWidget(this);
    header->setFixedHeight(32);
    header->setStyleSheet("background-color:#0d1a0d; border-bottom:1px solid #2a3a2a;");
    auto *hlay = new QHBoxLayout(header);
    hlay->setContentsMargins(8, 2, 8, 2);

    auto *titleLbl = new QLabel(
        QString("[R%1] %2").arg(radar.radarId).arg(radar.radarName), header);
    titleLbl->setStyleSheet("color:#00ff88; font:bold 10px Monospace;");
    hlay->addWidget(titleLbl, 1);

    m_reattachBtn = new QPushButton("Re-attach to Panel", header);
    m_reattachBtn->setObjectName("reattachBtn");
    m_reattachBtn->setCursor(Qt::PointingHandCursor);
    hlay->addWidget(m_reattachBtn);
    root->addWidget(header);

    // PPI widget fills the rest
    m_ppiWidget = new RadarPPIWidget(radar, this);
    m_ppiWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    root->addWidget(m_ppiWidget, 1);

    connect(m_reattachBtn, &QPushButton::clicked, this, [this]() {
        emit reattachRequested(m_radarId);
    });
}

void RadarPPIWindow::updateRadar(const RadarView::Radar &radar)
{
    setWindowTitle(QString("[R%1] %2  —  PPI").arg(radar.radarId).arg(radar.radarName));
    m_ppiWidget->updateRadar(radar);
}

void RadarPPIWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit windowClosed(m_radarId);
}

// ════════════════════════════════════════════════════════════════════════════
//  RadarDisplayPanel
// ════════════════════════════════════════════════════════════════════════════

RadarDisplayPanel::RadarDisplayPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("RadarDisplayPanel");
    setStyleSheet(kPanelStyle);
    buildUI();
}

void RadarDisplayPanel::buildUI()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── top toolbar ───────────────────────────────────────────────────────
    m_toolbar = new QToolBar(this);
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(QSize(14, 14));

    auto *lblRadar = new QLabel("  RADAR DISPLAY  ", m_toolbar);
    lblRadar->setStyleSheet("color:#00ff88; font:bold 10px Monospace; padding:0 4px;");
    m_toolbar->addWidget(lblRadar);

    auto *sep = new QFrame(m_toolbar);
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("color:#2a4a2a; margin:4px 2px;");
    m_toolbar->addWidget(sep);

    m_actDetach = new QAction("Detach Tab", this);
    m_actDetach->setToolTip("Open selected radar PPI in its own window");
    m_actDetach->setEnabled(false);
    connect(m_actDetach, &QAction::triggered,
            this, &RadarDisplayPanel::onDetachCurrentTab);
    m_toolbar->addAction(m_actDetach);

    m_actDetachAll = new QAction("Detach All", this);
    m_actDetachAll->setToolTip("Open every radar PPI in its own window");
    m_actDetachAll->setEnabled(false);
    connect(m_actDetachAll, &QAction::triggered, this, [this]() {
        QList<int> ids;
        for (auto it = m_tabEntries.begin(); it != m_tabEntries.end(); ++it)
            ids << it.key();
        for (int id : ids)
            onDetachTab(id);
    });
    m_toolbar->addAction(m_actDetachAll);

    root->addWidget(m_toolbar);

    // ── stacked: tabs or placeholder ─────────────────────────────────────
    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(false);
    m_tabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_tabs, &QTabWidget::tabCloseRequested,
            this, &RadarDisplayPanel::onTabCloseRequested);
    connect(m_tabs, &QTabWidget::currentChanged,
            this, &RadarDisplayPanel::onCurrentTabChanged);
    connect(m_tabs->tabBar(), &QTabBar::tabBarDoubleClicked,
            this, &RadarDisplayPanel::onTabBarDoubleClicked);

    root->addWidget(m_tabs, 1);

    // ── empty placeholder ─────────────────────────────────────────────────
    m_placeholder = new QLabel(
        "No radar selected.\n\n"
        "Select a radar from the Radar View panel\n"
        "and click  Open PPI View.", this);
    m_placeholder->setObjectName("placeholder");
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    root->addWidget(m_placeholder, 1, Qt::AlignCenter);

    showPlaceholder(true);
}

void RadarDisplayPanel::showPlaceholder(bool show)
{
    m_placeholder->setVisible(show);
    m_tabs->setVisible(!show);
    m_actDetach->setEnabled(!show && m_tabs->count() > 0);
    m_actDetachAll->setEnabled(!m_tabEntries.isEmpty());
}

// ─── public API ─────────────────────────────────────────────────────────────

void RadarDisplayPanel::openRadar(const RadarView::Radar &radar)
{
    // Already detached → bring window forward
    if (m_windowEntries.contains(radar.radarId)) {
        auto *win = m_windowEntries[radar.radarId].window;
        win->show();
        win->raise();
        win->activateWindow();
        return;
    }

    // Already a tab → switch to it
    if (m_tabEntries.contains(radar.radarId)) {
        int idx = tabIndexForRadar(radar.radarId);
        if (idx >= 0) m_tabs->setCurrentIndex(idx);
        return;
    }

    // Create PPI widget and new tab
    auto *ppi = new RadarPPIWidget(radar, this);
    ppi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QString tabTitle = QString("[R%1] %2").arg(radar.radarId).arg(radar.radarName);
    int idx = m_tabs->addTab(ppi, tabTitle);
    m_tabs->setCurrentIndex(idx);

    // Small detach icon button embedded in the tab bar
    auto *detachBtn = new QToolButton();
    detachBtn->setText(QString::fromUtf8("\xe2\xa4\xa2"));  // ⤢ U+2922
    detachBtn->setToolTip("Detach to separate window (or double-click tab)");
    detachBtn->setFixedSize(18, 18);
    detachBtn->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            color: #336644;
            border: none;
            font-size: 11px;
            padding: 0;
        }
        QToolButton:hover { color: #00ff88; }
    )");
    int rid = radar.radarId;
    connect(detachBtn, &QToolButton::clicked, this, [this, rid]() {
        onDetachTab(rid);
    });
    m_tabs->tabBar()->setTabButton(idx, QTabBar::LeftSide, detachBtn);

    TabEntry entry;
    entry.radarId   = radar.radarId;
    entry.ppiWidget = ppi;
    entry.radarData = radar;
    m_tabEntries.insert(radar.radarId, entry);

    showPlaceholder(false);
    m_actDetachAll->setEnabled(true);
    m_actDetach->setEnabled(true);
}

void RadarDisplayPanel::updateRadar(const RadarView::Radar &radar)
{
    if (m_tabEntries.contains(radar.radarId)) {
        m_tabEntries[radar.radarId].radarData = radar;
        if (m_tabEntries[radar.radarId].ppiWidget)
            m_tabEntries[radar.radarId].ppiWidget->updateRadar(radar);
    }
    if (m_windowEntries.contains(radar.radarId)) {
        m_windowEntries[radar.radarId].radarData = radar;
        m_windowEntries[radar.radarId].window->updateRadar(radar);
    }
}

void RadarDisplayPanel::closeRadar(int radarId)
{
    // Close detached window if present
    if (m_windowEntries.contains(radarId)) {
        m_windowEntries[radarId].window->close();
        m_windowEntries[radarId].window->deleteLater();
        m_windowEntries.remove(radarId);
    }

    // Remove tab if present
    if (m_tabEntries.contains(radarId)) {
        int idx = tabIndexForRadar(radarId);
        if (idx >= 0) {
            // Remove detach button to avoid dangling pointer
            m_tabs->tabBar()->setTabButton(idx, QTabBar::LeftSide, nullptr);
            m_tabs->removeTab(idx);
        }
        m_tabEntries.remove(radarId);
    }

    if (m_tabEntries.isEmpty() && m_windowEntries.isEmpty())
        showPlaceholder(true);
    m_actDetachAll->setEnabled(!m_tabEntries.isEmpty());
    m_actDetach->setEnabled(m_tabs->count() > 0);
}

void RadarDisplayPanel::closeAll()
{
    for (auto it = m_windowEntries.begin(); it != m_windowEntries.end(); ++it) {
        it.value().window->close();
        it.value().window->deleteLater();
    }
    m_windowEntries.clear();

    m_tabs->clear();
    m_tabEntries.clear();

    showPlaceholder(true);
    m_actDetachAll->setEnabled(false);
    m_actDetach->setEnabled(false);
}

bool RadarDisplayPanel::hasRadar(int radarId) const
{
    return m_tabEntries.contains(radarId) || m_windowEntries.contains(radarId);
}

// ─── private helpers ─────────────────────────────────────────────────────────

int RadarDisplayPanel::tabIndexForRadar(int radarId) const
{
    for (int i = 0; i < m_tabs->count(); ++i) {
        QWidget *w = m_tabs->widget(i);
        for (auto it = m_tabEntries.constBegin(); it != m_tabEntries.constEnd(); ++it) {
            if (it->radarId == radarId && it->ppiWidget == w)
                return i;
        }
    }
    return -1;
}

// ─── private slots ───────────────────────────────────────────────────────────

void RadarDisplayPanel::onTabCloseRequested(int index)
{
    QWidget *w = m_tabs->widget(index);
    for (auto it = m_tabEntries.begin(); it != m_tabEntries.end(); ++it) {
        if (it->ppiWidget == w) {
            m_tabs->tabBar()->setTabButton(index, QTabBar::LeftSide, nullptr);
            m_tabs->removeTab(index);
            m_tabEntries.erase(it);
            break;
        }
    }
    if (m_tabEntries.isEmpty() && m_windowEntries.isEmpty())
        showPlaceholder(true);
    m_actDetachAll->setEnabled(!m_tabEntries.isEmpty());
    m_actDetach->setEnabled(m_tabs->count() > 0);
}

void RadarDisplayPanel::onDetachCurrentTab()
{
    int idx = m_tabs->currentIndex();
    if (idx < 0) return;
    QWidget *w = m_tabs->widget(idx);
    for (auto it = m_tabEntries.begin(); it != m_tabEntries.end(); ++it) {
        if (it->ppiWidget == w) {
            onDetachTab(it->radarId);
            return;
        }
    }
}

void RadarDisplayPanel::onDetachTab(int radarId)
{
    if (!m_tabEntries.contains(radarId)) return;
    if (m_windowEntries.contains(radarId)) {
        // Already detached — bring forward
        m_windowEntries[radarId].window->show();
        m_windowEntries[radarId].window->raise();
        return;
    }

    TabEntry entry = m_tabEntries[radarId];

    // Create standalone window
    auto *win = new RadarPPIWindow(entry.radarData, nullptr);

    // Centre on primary screen
    if (QScreen *screen = QApplication::primaryScreen()) {
        QRect sr = screen->availableGeometry();
        win->move(sr.center() - QPoint(win->width() / 2, win->height() / 2));
    }

    connect(win, &RadarPPIWindow::reattachRequested,
            this, &RadarDisplayPanel::onReattachRadar);
    connect(win, &RadarPPIWindow::windowClosed,
            this, &RadarDisplayPanel::onWindowClosed);

    // Remove tab (the RadarPPIWidget inside is no longer needed once we have window)
    int idx = tabIndexForRadar(radarId);
    if (idx >= 0) {
        m_tabs->tabBar()->setTabButton(idx, QTabBar::LeftSide, nullptr);
        m_tabs->removeTab(idx);
        if (entry.ppiWidget)
            entry.ppiWidget->deleteLater();
    }
    m_tabEntries.remove(radarId);

    WindowEntry we;
    we.window    = win;
    we.radarData = entry.radarData;
    m_windowEntries.insert(radarId, we);

    win->show();
    win->raise();

    // If no tabs left, show placeholder in the panel
    if (m_tabEntries.isEmpty())
        showPlaceholder(true);
    m_actDetachAll->setEnabled(!m_tabEntries.isEmpty());
    m_actDetach->setEnabled(m_tabs->count() > 0);
}

void RadarDisplayPanel::onReattachRadar(int radarId)
{
    if (!m_windowEntries.contains(radarId)) return;

    WindowEntry we = m_windowEntries[radarId];
    RadarView::Radar saved = we.radarData;

    we.window->hide();
    we.window->deleteLater();
    m_windowEntries.remove(radarId);

    // Re-open as tab
    openRadar(saved);
}

void RadarDisplayPanel::onWindowClosed(int radarId)
{
    if (!m_windowEntries.contains(radarId)) return;
    m_windowEntries[radarId].window->deleteLater();
    m_windowEntries.remove(radarId);

    if (m_tabEntries.isEmpty() && m_windowEntries.isEmpty())
        showPlaceholder(true);
}

void RadarDisplayPanel::onCurrentTabChanged(int index)
{
    m_actDetach->setEnabled(index >= 0);
}

void RadarDisplayPanel::onTabBarDoubleClicked(int index)
{
    if (index < 0) return;
    QWidget *w = m_tabs->widget(index);
    for (auto it = m_tabEntries.begin(); it != m_tabEntries.end(); ++it) {
        if (it->ppiWidget == w) {
            onDetachTab(it->radarId);
            return;
        }
    }
}
