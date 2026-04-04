#include "radardockwidget.h"
#include <QSizePolicy>

RadarDockWidget::RadarDockWidget(const RadarView::Radar &radar, QWidget *parent)
    : QDockWidget(parent)
    , m_radarId(radar.radarId)
{
    setWindowTitle(QString("[R%1] %2  —  PPI").arg(radar.radarId).arg(radar.radarName));
    setObjectName(QString("RadarDock_%1").arg(radar.radarId));

    // Allow all dock features: floating, movable, closeable, minimizable
    setFeatures(QDockWidget::DockWidgetMovable  |
                QDockWidget::DockWidgetFloatable |
                QDockWidget::DockWidgetClosable);

    setAllowedAreas(Qt::AllDockWidgetAreas);

    m_ppiWidget = new RadarPPIWidget(radar, this);
    m_ppiWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWidget(m_ppiWidget);

    // Start floating at a comfortable size
    resize(520, 540);

    setStyleSheet(R"(
QDockWidget {
    color: #00ff88;
    font-weight: bold;
    font-size: 11px;
    background-color: #0a0f14;
    border: 1px solid #003322;
    titlebar-close-icon: url(none);
    titlebar-normal-icon: url(none);
}
QDockWidget::title {
    background-color: #0d1a12;
    padding: 6px 10px;
    border-bottom: 1px solid #003322;
    text-align: left;
}
QDockWidget::close-button,
QDockWidget::float-button {
    border: none;
    background: transparent;
    icon-size: 12px;
}
QDockWidget::close-button:hover,
QDockWidget::float-button:hover {
    background: rgba(0,255,136,0.15);
    border-radius: 3px;
}
)");
}

void RadarDockWidget::updateRadar(const RadarView::Radar &radar)
{
    setWindowTitle(QString("[R%1] %2  —  PPI").arg(radar.radarId).arg(radar.radarName));
    m_ppiWidget->updateRadar(radar);
}
