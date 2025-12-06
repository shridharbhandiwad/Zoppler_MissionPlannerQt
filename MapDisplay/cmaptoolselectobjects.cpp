#include "cmaptoolselectobjects.h"
#include <qgsmapmouseevent.h>
#include <qgscoordinatetransform.h>
#include <qgsproject.h>
#include "globalConstants.h"

CMapToolSelectObjects::CMapToolSelectObjects(QgsMapCanvas *pCanvas)
    : QgsMapTool(pCanvas)
    , m_nObjectClass(VISTAR_CLASS_NONE)
{
}

CMapToolSelectObjects::~CMapToolSelectObjects()
{
}

void CMapToolSelectObjects::canvasPressEvent(QgsMapMouseEvent *e)
{
    // Handle mouse press
    Q_UNUSED(e);
}

void CMapToolSelectObjects::canvasMoveEvent(QgsMapMouseEvent *e)
{
    // Update cursor or show preview
    Q_UNUSED(e);
}

void CMapToolSelectObjects::canvasReleaseEvent(QgsMapMouseEvent *e)
{
    if (!e)
        return;

    // Get the click position in map coordinates
    QgsPointXY mapPoint = e->mapPoint();
    
    if (m_nObjectClass != VISTAR_CLASS_NONE) {
        // Emit signal to create object at this location
        emit signalObjectCreated(m_nObjectClass, mapPoint.y(), mapPoint.x()); // Lat, Lon
    }
}

void CMapToolSelectObjects::SetObjectToCreate(int nClass)
{
    m_nObjectClass = nClass;
}
