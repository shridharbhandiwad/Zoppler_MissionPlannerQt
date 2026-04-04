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
    color: #f0f0f0;
    font-weight: bold;
    font-size: 11px;
    background-color: #0a0f14;
    border: 1px solid #5a5a6a;
}
QDockWidget::title {
    background-color: #48485a;
    padding: 6px 10px;
    border-bottom: 1px solid #5a5a6a;
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
    background: rgba(74, 144, 217, 0.25);
    border-radius: 3px;
}
)");
}

void RadarDockWidget::updateRadar(const RadarView::Radar &radar)
{
    setWindowTitle(QString("[R%1] %2  —  PPI").arg(radar.radarId).arg(radar.radarName));
    m_ppiWidget->updateRadar(radar);
}
