#include "cvistarobject.h"
#include <qgsmaptopixel.h>
#include <QtMath>
#include <QPainter>
#include "globalConstants.h"
#include "cnetworkinterface.h"
#include "globalConstants.h"

CVistarObject::CVistarObject(QgsMapCanvas *canvas) :
    QgsMapCanvasItem(canvas), _m_canvas(canvas) {
    setZValue(101);
    _m_sObjectID = "";
    _m_nClass = 0;
    _m_dLon = 0;
    _m_dLat = 0;
    _m_dAlt = 50;
    _m_dHeading = 0;
    _m_dVelocity = 0.0;
    _m_nChildId = 0;
    _m_sAttachedRoute = "";
    _m_sParentObject = "";
    _m_bHighlight = false;
    _m_bRefresh = false;
}

CVistarObject::CVistarObject(QgsMapCanvas *canvas,QString sObjectID,
                             int nClass, double dLon, double dLat) :

    CVistarObject(canvas) {
    _m_sObjectID = sObjectID;
    _m_nClass = nClass;
    _m_dLon = dLon;
    _m_dLat = dLat;



    QString strPath = "";
    int nSize = 50;
    switch ( _m_nClass ) {
    case VISTAR_CLASS_DRONE : strPath = ":/icons/cursor/drone.png"; nSize = 40; _m_dAlt = 1000;
        break;
    case VISTAR_CLASS_DRONE_SWARM : strPath = ":/icons/cursor/drone_swarm.png"; _m_dAlt = 1000;
        break;
    case VISTAR_CLASS_FIGHTER : strPath = ":/icons/cursor/fighter.png"; _m_dAlt = 1000;
        break;
    case VISTAR_CLASS_UAV : strPath = ":/icons/cursor/uav.png"; _m_dAlt = 1000;
        break;
    case VISTAR_CLASS_RADAR : strPath = ":/icons/cursor/radar.png"; nSize = 40; _m_dAlt = 0;
        break;
    case VISTAR_CLASS_LAUNCHER : strPath = ":/icons/cursor/launcher.png"; _m_dAlt = 0;
        break;
    case VISTAR_CLASS_MISSILE : strPath = ":/icons/cursor/missile.png"; _m_dAlt = 0;
        break;
    }
    _m_nImageSize = nSize;
    _m_Image = QImage(strPath).scaledToWidth(nSize,Qt::SmoothTransformation);
}

QgsPointXY CVistarObject::getPoint() {
    return QgsPointXY(_m_dLon,_m_dLat);
}

QgsPointXYZ CVistarObject::getPointXYZ() {
    return QgsPointXYZ(_m_dLon,_m_dLat,_m_dAlt);
}

QString CVistarObject::getObjectId() {
    return _m_sObjectID;
}

bool CVistarObject::isValid() {
    return (_m_canvas != nullptr);
}

void CVistarObject::attachRoute( QString sRoute ) {
    _m_sAttachedRoute = sRoute;
}

void CVistarObject::setParent( QString sParent, int nChildId ) {
    _m_sParentObject = sParent;
    _m_nChildId = nChildId;
}

QString CVistarObject::getParent() {
    return _m_sParentObject;
}

int CVistarObject::getChildId() {
    return _m_nChildId;
}

QRectF CVistarObject::boundingRect() const
{
    // Each map canvas item must define a bounding rect in MAP COORDINATES
    //double halfSizePx = 1; // 10 pixel radius for example
    double halfSizeMap = 1.;//0.00001;//halfSizePx * _m_canvas->mapUnitsPerPixel();

    // Center of your object in map coordinates
    double x = _m_dLon;
    double y = _m_dLat;

    QRectF rect = QRectF(x - halfSizeMap, y - halfSizeMap,
           2 * halfSizeMap, 2 * halfSizeMap);
    //qDebug()<<rect;

    return rect;
}

void CVistarObject::paint(QPainter *pPainter)
{
    if (!pPainter) return;

    if ( _m_nChildId != 0 ) return;

    pPainter->setRenderHint(QPainter::Antialiasing, true);

    // Transform geographic positions to screen coordinates
    const QgsMapToPixel &mapToPixel = _m_canvas->mapSettings().mapToPixel();

    QPointF ptScreen = mapToPixel.transform(QgsPointXY(_m_dLon, _m_dLat)).toQPointF();
    double pixelPerDegree = 1.0 / _m_canvas->mapUnitsPerPixel();

    pPainter->save();
    pPainter->translate(ptScreen);
    double dRotate = _m_dHeading;
    pPainter->rotate(dRotate);

    QColor  clr = Qt::cyan;

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(3);

    QBrush brush;
    brush.setColor(clr);

    // Core dot
    pPainter->setPen(pen);
    pPainter->setBrush(brush);

    pen.setColor(Qt::black);
    pen.setWidth(3);
    pPainter->setPen(pen);

    double dVelVector = -1 * (_m_dVelocity * 0.05 + 20);

    pPainter->drawLine(QPoint(0,0), QPoint(0,dVelVector));

    pen.setColor(Qt::cyan);
    pen.setWidth(2);
    pPainter->setPen(pen);


    pPainter->drawLine(QPoint(0,0), QPoint(0,dVelVector));

    if ( _m_Image.isNull() ) {
        pPainter->drawEllipse(QPoint(0,0), _m_nImageSize, _m_nImageSize);
    }
    else {
        pPainter->drawImage(QPointF(-_m_nImageSize/2,-_m_nImageSize/2),_m_Image);
    }

    pPainter->rotate(-dRotate);

    if ( _m_nChildId == 0 ) {
        if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
            pPainter->setFont(QFont("century",11,80,true));
            // Label
            pPainter->setPen(Qt::white);
            pPainter->drawText(QPointF(-1 * _m_sObjectID.length() * 6,40), _m_sObjectID);
        }
        if (isHighlighted()) {
            pen.setColor(Qt::blue);
            pen.setWidth(4);
            pPainter->setPen(pen);
            pPainter->drawEllipse(QPoint(0,0),_m_nImageSize/2,_m_nImageSize/2);
        }
    }

    pPainter->restore();

}

QString CVistarObject::getClassAsString() {

    QString sClass = "";
    switch ( _m_nClass ) {
    case VISTAR_CLASS_DRONE : sClass = "drone";
        break;
    case VISTAR_CLASS_DRONE_SWARM : sClass = "drone_swarm";
        break;
    case VISTAR_CLASS_FIGHTER : sClass = "fighter";
        break;
    case VISTAR_CLASS_UAV : sClass = "uav";
        break;
    case VISTAR_CLASS_RADAR : sClass = "radar";
        break;
    case VISTAR_CLASS_LAUNCHER : sClass = "launcher";
        break;
    case VISTAR_CLASS_MISSILE : sClass = "missile";
        break;
    }
    return sClass;
}

void CVistarObject::UpdateLocation(double dLat,double dLon,double dAlt) {
    _m_dLon = dLon;
    _m_dLat = dLat;
    _m_dAlt = dAlt;

    refresh();
}

void CVistarObject::UpdateObject( QJsonObject jsonObject ) {


    QJsonObject stLocation = jsonObject.value("LOCATION").toObject();
    // _m_dLon = stLocation["X"].toString().toDouble();
    // _m_dLat = stLocation["Y"].toString().toDouble();
    // _m_dAlt = stLocation["Z"].toString().toDouble();

    _m_dLon = stLocation["X"].toDouble();
    _m_dLat = stLocation["Y"].toDouble();
    _m_dAlt = stLocation["Z"].toDouble();


    // static double offset = 0.1;

    // _m_dLon = 76.63+offset;
    // _m_dLat = 22.02+offset;
    // _m_dAlt = 100.0;

    // offset = offset + 0.1;
    qDebug()<<"stLocation "<< stLocation;
    qDebug()<<"Data "<<_m_dLon<<_m_dLat<<_m_dAlt<<jsonObject["LOCATION"];

    QJsonObject stRotation = jsonObject.value("ROTATION").toObject();
    _m_dHeading = stRotation["YAW"].toDouble();

    _m_nChildId = 0;

    refresh();
}

void CVistarObject::setHighlighted(bool bHighlight) {
    _m_bHighlight = bHighlight;
}

bool CVistarObject::isHighlighted() {
    return _m_bHighlight;
}

void CVistarObject::refresh() {
    updatePosition();
    //setCenter(QgsPointXY(x, y));
    update();
    update(boundingRect());

    //_m_bRefresh = true;
}

void CVistarObject::TransmitSelfInfo() {
    QJsonObject jsonRoot;

    // Location object
    QJsonObject jsonLocation;
    jsonLocation["X"] = _m_dLon;
    jsonLocation["Y"] = _m_dLat;
    jsonLocation["Z"] = _m_dAlt;

    // Rotation object
    QJsonObject jsonRotation;
    jsonRotation["YAW"] = _m_dHeading;
    jsonRotation["PITCH"] = 0;
    jsonRotation["ROLL"] = 0;

    // Rotation object
    QJsonObject jsonSlew;
    jsonSlew["SLEW_AZ"] = 180;
    jsonSlew["SLEW_ELEV"] = 45;

    // Root key-value pairs
    jsonRoot["SRC"] = VISTAR_SRC_MISSION_PLANNER;
    jsonRoot["ID"] = _m_sObjectID;
    jsonRoot["CLASS"] = getClassAsString();
    jsonRoot["STREAM"] = "create";
    jsonRoot["TRAJECTORY"] = _m_sAttachedRoute;
    jsonRoot["PARENT"] = _m_sParentObject;
    jsonRoot["CHILD_ID"] = _m_nChildId;
    jsonRoot["LOCATION"] = jsonLocation;
    jsonRoot["ROTATION"] = jsonRotation;
    jsonRoot["SLEW"] = jsonSlew;


    qDebug()<<"Publish object "<<_m_sObjectID<<_m_sParentObject<<_m_nChildId;
    // Convert to string
    QJsonDocument doc(jsonRoot);

    CNetworkInterface::PublishMessage(doc);


}
