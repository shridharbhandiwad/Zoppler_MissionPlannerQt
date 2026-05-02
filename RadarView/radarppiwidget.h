#ifndef RADARPPIWIDGET_H
#define RADARPPIWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "radarmanager.h"

class RadarPPIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarPPIWidget(const RadarView::Radar &radar, QWidget *parent = nullptr);

    void updateRadar(const RadarView::Radar &radar);

protected:
    void paintEvent(QPaintEvent *event)     override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event)          override;
    void resizeEvent(QResizeEvent *event)   override;
    QSize sizeHint() const                 override;

private:
    QPointF polarToScreen(double range, double azimuthDeg) const;
    QRectF  scopeRect()   const;
    double  scopeRadius() const;
    QPointF scopeCenter() const;

    RadarView::Radar m_radar;

    int    m_hoveredTrackId;
    QPoint m_hoverPos;

    struct DetectionScreen {
        QPointF pos;
        int     trackId;
        double  range;
        double  azimuth;
        double  elevation;
    };
    QVector<DetectionScreen> m_screenDetections;

    void rebuildScreenPositions();
};

#endif // RADARPPIWIDGET_H
