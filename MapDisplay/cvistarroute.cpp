#include "cvistarroute.h"
#include "globalConstants.h"
#include <QJsonArray>
#include "cnetworkinterface.h"

CVistarRoute::CVistarRoute(QgsMapCanvas *canvas) :
    QgsMapCanvasItem(canvas), _m_canvas(canvas) {
    setZValue(101);
    _m_sObjectID = "";
    _m_bHighlight = false;
}

CVistarRoute::CVistarRoute(QgsMapCanvas *canvas,QString sObjectID,
                             double dStartLon, double dStartLat) :

    CVistarRoute(canvas) {
    _m_sObjectID = sObjectID;
    _m_bHighlight = false;
    _m_nImageSize = 40;
    _m_Image = QImage(":/icons/cursor/route.png").scaledToWidth(_m_nImageSize,Qt::SmoothTransformation);
    addPoint(QgsPointXY(dStartLon,dStartLat));
}

void CVistarRoute::addPoint(QgsPointXY pt) {
    _m_listPoints << QgsPointXYZ(pt,1000);
    refresh();
}

void CVistarRoute::UpdatePoints(QList<QgsPointXYZ> points) {
    _m_listPoints.clear();
    _m_listPoints.append(points);
    refresh();
}

int CVistarRoute::getPointCount() {
    return _m_listPoints.size();
}

QgsPointXY CVistarRoute::getPointAt(int index) {
    return _m_listPoints.at(index);
}

QList<QgsPointXYZ> CVistarRoute::getPoints() {
    return _m_listPoints;
}


QString CVistarRoute::getObjectId() {
    return _m_sObjectID;
}

void CVistarRoute::refresh() {
    update(boundingRect());
}

void CVistarRoute::setHighlighted(bool bHighlight) {
    _m_bHighlight = bHighlight;
}

bool CVistarRoute::isHighlighted() {
    return _m_bHighlight;
}


QRectF CVistarRoute::boundingRect() const
{
    // // Each map canvas item must define a bounding rect in MAP COORDINATES
    // double halfSizePx = 10; // 10 pixel radius for example
    // double halfSizeMap = halfSizePx * _m_canvas->mapUnitsPerPixel();

    // // Center of your object in map coordinates
    // double x = _m_dLon;
    // double y = _m_dLat;

    // return QRectF(x - halfSizeMap, y - halfSizeMap,
    //               2 * halfSizeMap, 2 * halfSizeMap);

    return _m_canvas->rect();
}

void CVistarRoute::paint(QPainter *pPainter)
{
    if (!pPainter) return;

    pPainter->setRenderHint(QPainter::Antialiasing, true);

    // Transform geographic positions to screen coordinates
    const QgsMapToPixel &mapToPixel = _m_canvas->mapSettings().mapToPixel();



    QPen pen;
    pen.setColor(Qt::yellow);
    pen.setWidth(3);
    if ( isHighlighted() ) {
        pen.setColor(Qt::blue);
        pen.setWidth(4);
    }

    pPainter->setPen(pen);

    for ( int i = 0; i < _m_listPoints.size(); i++) {

        QgsPointXY pt = _m_listPoints.at(i);

        const QPointF ptScreen = mapToPixel.transform(pt).toQPointF();

        if ( i > 0 ) {
            QgsPointXY ptPrev = _m_listPoints.at(i-1);
            const QPointF ptScreenPrev = mapToPixel.transform(ptPrev).toQPointF();
            pPainter->drawLine(QLineF(ptScreenPrev.x(),ptScreenPrev.y(),ptScreen.x(),ptScreen.y()));
        }

        pPainter->save();
        pPainter->translate(ptScreen);
        pPainter->drawImage(QPointF(-_m_nImageSize/2,-_m_nImageSize),_m_Image);

        if ( i == 0 ) {
            double pixelPerDegree = 1.0 / _m_canvas->mapUnitsPerPixel();
            if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
                pPainter->setFont(QFont("century",12,80,true));
                // Label
                pPainter->setPen(Qt::white);
                pPainter->drawText(QPointF(-1 * _m_sObjectID.length() * 6,40), _m_sObjectID);
            }
        }

        pPainter->setFont(QFont("century",12,100,true));
        pPainter->drawText(QPointF(-1 * 1 * 6,30), QString::number(i));
        pPainter->restore();
    }
}


void CVistarRoute::TransmitSelfInfo() {
    QJsonObject jsonRoot;

    QJsonArray jsonLocationsList;
    for (int i = 0; i < _m_listPoints.size(); ++i)
    {
        QgsPointXYZ pt = _m_listPoints.at(i);

        QJsonObject jsonLocation;
        jsonLocation["X"] = pt.x();
        jsonLocation["Y"] = pt.y();
        jsonLocation["Z"] = pt.z();
        jsonLocationsList.append(jsonLocation);
    }

    // Root key-value pairs
    jsonRoot["SRC"] = VISTAR_SRC_MISSION_PLANNER;
    jsonRoot["ID"] = _m_sObjectID;
    jsonRoot["CLASS"] = "route";
    jsonRoot["STREAM"] = "Create";
    jsonRoot["POINTS"] = jsonLocationsList;
    jsonRoot["COUNT"] = _m_listPoints.size();

    // Convert to string
    QJsonDocument doc(jsonRoot);
    qDebug("Sending waypoint");
    CNetworkInterface::PublishMessage(doc);

}
