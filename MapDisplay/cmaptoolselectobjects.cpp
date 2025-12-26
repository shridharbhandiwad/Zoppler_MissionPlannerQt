#include "cmaptoolselectobjects.h"
#include <qgsmapcanvas.h>
#include <qgsmapmouseevent.h>
#include <QDebug>

CMapToolSelectObjects::CMapToolSelectObjects(QgsMapCanvas *canvas)
    : QgsMapTool(canvas)
{
    qDebug() << "CMapToolSelectObjects created";
}

CMapToolSelectObjects::~CMapToolSelectObjects()
{
    qDebug() << "CMapToolSelectObjects destroyed";
}

void CMapToolSelectObjects::canvasPressEvent(QgsMapMouseEvent *event)
{
    m_clickPoint = event->mapPoint();
    qDebug() << "Map clicked at:" << m_clickPoint.toString();
}

void CMapToolSelectObjects::canvasMoveEvent(QgsMapMouseEvent *event)
{
    // Handle mouse move if needed
    Q_UNUSED(event);
}

void CMapToolSelectObjects::canvasReleaseEvent(QgsMapMouseEvent *event)
{
    QgsPointXY releasePoint = event->mapPoint();
    
    // Check if this was a click (not a drag)
    double tolerance = 0.001; // Small tolerance for click detection
    if (qAbs(releasePoint.x() - m_clickPoint.x()) < tolerance &&
        qAbs(releasePoint.y() - m_clickPoint.y()) < tolerance) {
        
        qDebug() << "Point selected at:" << releasePoint.toString();
        emit pointSelected(releasePoint);
    }
}
