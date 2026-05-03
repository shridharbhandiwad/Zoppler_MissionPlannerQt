#include "cvistarobject.h"
#include <qgsmaptopixel.h>
#include <QtMath>
#include <QPainter>
#include <QDateTime>
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

    // Initialize trajectory tracking
    _m_bTrajectoryEnabled = true;  // Enable trajectory by default
    _m_trajectoryColor = QColor(255, 165, 0, 200);  // Orange with some transparency
    _m_nTrajectoryWidth = 2;
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
    case VISTAR_CLASS_JAMMER : strPath = ":/icons/cursor/jammer.png"; nSize = 44; _m_dAlt = 0;
        break;
    case VISTAR_CLASS_CLUTTER : strPath = ":/icons/cursor/clutter.png"; nSize = 44; _m_dAlt = 0;
        break;
    case VISTAR_CLASS_RF_DETECTOR : strPath = ":/icons/cursor/rf_detector.png"; nSize = 44; _m_dAlt = 0;
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

QString CVistarObject::getAttachedRoute() {
    return _m_sAttachedRoute;
}

QRectF CVistarObject::boundingRect() const
{
    // If trajectory is enabled and has points, we need to include the full trajectory
    // in the bounding rect to ensure proper rendering
    if (_m_bTrajectoryEnabled && !_m_listTrajectoryPoints.isEmpty()) {
        // Return the full canvas rect to ensure trajectory is always visible
        return _m_canvas->rect();
    }

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

    // Draw trajectory line (path traveled) before drawing the object
    if (_m_bTrajectoryEnabled && _m_listTrajectoryPoints.size() > 0) {
        pPainter->save();

        // Set up pen for trajectory with gradient effect
        QPen trajectoryPen;
        trajectoryPen.setColor(_m_trajectoryColor);
        trajectoryPen.setWidth(_m_nTrajectoryWidth);
        trajectoryPen.setCapStyle(Qt::RoundCap);
        trajectoryPen.setJoinStyle(Qt::RoundJoin);
        pPainter->setPen(trajectoryPen);

        // Draw trajectory segments with fading effect (older segments more transparent)
        int numPoints = _m_listTrajectoryPoints.size();

        for (int i = 0; i < numPoints; i++) {
            QgsPointXYZ pt = _m_listTrajectoryPoints.at(i);
            QPointF ptScreenTraj = mapToPixel.transform(QgsPointXY(pt.x(), pt.y())).toQPointF();

            if (i > 0) {
                // Calculate alpha based on position in trajectory (older = more faded)
                int alpha = qMin(255, 100 + (155 * i / numPoints));
                QColor segmentColor = _m_trajectoryColor;
                segmentColor.setAlpha(alpha);

                QPen segmentPen;
                segmentPen.setColor(segmentColor);
                segmentPen.setWidth(_m_nTrajectoryWidth);
                segmentPen.setCapStyle(Qt::RoundCap);
                pPainter->setPen(segmentPen);

                QgsPointXYZ ptPrev = _m_listTrajectoryPoints.at(i - 1);
                QPointF ptScreenPrev = mapToPixel.transform(QgsPointXY(ptPrev.x(), ptPrev.y())).toQPointF();
                pPainter->drawLine(ptScreenPrev, ptScreenTraj);
            }

            // Draw small dots at waypoints for better visibility
            if (i > 0 && i < numPoints - 1) {
                pPainter->setBrush(QBrush(_m_trajectoryColor));
                pPainter->drawEllipse(ptScreenTraj, 3, 3);
            }
        }

        // Draw line from last trajectory point to current position
        if (numPoints > 0) {
            QgsPointXYZ lastPt = _m_listTrajectoryPoints.last();
            QPointF lastPtScreen = mapToPixel.transform(QgsPointXY(lastPt.x(), lastPt.y())).toQPointF();

            QPen currentPen;
            currentPen.setColor(_m_trajectoryColor);
            currentPen.setWidth(_m_nTrajectoryWidth);
            currentPen.setCapStyle(Qt::RoundCap);
            pPainter->setPen(currentPen);

            pPainter->drawLine(lastPtScreen, ptScreen);
        }

        pPainter->restore();
    }

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
    case VISTAR_CLASS_JAMMER : sClass = "jammer";
        break;
    case VISTAR_CLASS_CLUTTER : sClass = "clutter";
        break;
    case VISTAR_CLASS_RF_DETECTOR : sClass = "rf_detector";
        break;
    }
    return sClass;
}

void CVistarObject::UpdateLocation(double dLat,double dLon,double dAlt) {
    // Add current position to trajectory before moving (if trajectory is enabled)
    if (_m_bTrajectoryEnabled && _m_nChildId == 0) {
        // Only add point if it's different from the last point (avoid duplicates)
        if (_m_listTrajectoryPoints.isEmpty() ||
            (_m_dLon != _m_listTrajectoryPoints.last().x() ||
             _m_dLat != _m_listTrajectoryPoints.last().y())) {
            _m_listTrajectoryPoints.append(QgsPointXYZ(_m_dLon, _m_dLat, _m_dAlt));
        }
    }

    _m_dLon = dLon;
    _m_dLat = dLat;
    _m_dAlt = dAlt;

    refresh();
}

void CVistarObject::UpdateObject( QJsonObject jsonObject ) {

    // Store previous position for trajectory
    double prevLon = _m_dLon;
    double prevLat = _m_dLat;
    double prevAlt = _m_dAlt;

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

    // Add previous position to trajectory if enabled and position has changed
    if (_m_bTrajectoryEnabled) {
        // Check if position has actually changed (to avoid trajectory points when stationary)
        bool positionChanged = (prevLon != _m_dLon || prevLat != _m_dLat);

        if (positionChanged) {
            // Add the previous position to trajectory
            if (_m_listTrajectoryPoints.isEmpty() ||
                (prevLon != _m_listTrajectoryPoints.last().x() ||
                 prevLat != _m_listTrajectoryPoints.last().y())) {
                // Only add if previous position was valid (non-zero)
                if (prevLon != 0 || prevLat != 0) {
                    _m_listTrajectoryPoints.append(QgsPointXYZ(prevLon, prevLat, prevAlt));
                }
            }
        }
    }

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
    jsonRotation["YAW"]   = _m_dHeading;
    jsonRotation["PITCH"] = 0;
    jsonRotation["ROLL"]  = 0;

    // Slew (legacy – kept for backward compatibility)
    QJsonObject jsonSlew;
    jsonSlew["SLEW_AZ"]   = 180;
    jsonSlew["SLEW_ELEV"] = 45;

    // ISO-8601 UTC timestamp
    QString timestamp = QDateTime::currentDateTimeUtc()
                            .toString(Qt::ISODateWithMs);

    // Root key-value pairs
    jsonRoot["SRC"]       = VISTAR_SRC_MISSION_PLANNER;
    jsonRoot["ID"]        = _m_sObjectID;
    jsonRoot["CLASS"]     = getClassAsString();
    jsonRoot["STREAM"]    = "create";
    jsonRoot["TIMESTAMP"] = timestamp;
    jsonRoot["LOCATION"]  = jsonLocation;
    jsonRoot["ROTATION"]  = jsonRotation;
    jsonRoot["TRAJECTORY"]= _m_sAttachedRoute;
    jsonRoot["PARENT"]    = _m_sParentObject;
    jsonRoot["CHILD_ID"]  = _m_nChildId;
    jsonRoot["SLEW"]      = jsonSlew;

    // For radar objects include the physics/simulation parameters block
    if (_m_nClass == VISTAR_CLASS_RADAR) {
        jsonRoot["parameters"] = _m_radarPhysics.toJson();
    }

    // Jammer parameters
    if (_m_nClass == VISTAR_CLASS_JAMMER) {
        QJsonObject params;
        params["tx_power"]  = _m_jammerParams.txPowerW;
        params["gain"]      = _m_jammerParams.gainDbi;
        params["bandwidth"] = _m_jammerParams.bandwidthHz;
        params["loss"]      = _m_jammerParams.lossDb;
        jsonRoot["parameters"] = params;
    }

    // Clutter parameters
    if (_m_nClass == VISTAR_CLASS_CLUTTER) {
        QJsonObject params;
        params["TYPE"]             = _m_clutterParams.type;
        params["RADIUS_M"]         = _m_clutterParams.radiusM;
        params["SIGMA0_DB"]        = _m_clutterParams.sigma0Db;
        params["SURFACE_TYPE"]     = _m_clutterParams.surfaceType;
        params["STAT_MODEL"]       = _m_clutterParams.statModel;
        params["WEIBULL_C"]        = _m_clutterParams.weibullC;
        params["WIND_SPEED_MS"]    = _m_clutterParams.windSpeedMs;
        params["WIND_DIR_DEG"]     = _m_clutterParams.windDirDeg;
        params["DOPPLER_SPREAD_MS"] = _m_clutterParams.dopplerSpreadMs;
        jsonRoot["parameters"] = params;
    }

    // RF Detector parameters
    if (_m_nClass == VISTAR_CLASS_RF_DETECTOR) {
        if (!_m_rfDetectorParams.parentId.isEmpty()) {
            jsonRoot["PARENT"] = _m_rfDetectorParams.parentId;
        }
        QJsonObject params;
        params["MDS_DBM"]        = _m_rfDetectorParams.mdsDbm;
        params["GR_DBM"]         = _m_rfDetectorParams.grDbm;
        params["NOISE_FIGURE_DB"] = _m_rfDetectorParams.noiseFigureDb;
        params["BANDWIDTH"]      = _m_rfDetectorParams.bandwidth;
        jsonRoot["parameters"] = params;
    }

    qDebug()<<"Publish object "<<_m_sObjectID<<_m_sParentObject<<_m_nChildId;
    QJsonDocument doc(jsonRoot);
    CNetworkInterface::PublishMessage(doc);
}

void CVistarObject::setRadarPhysicsParameters(const RadarView::RadarPhysicsParameters &params)
{
    _m_radarPhysics = params;
}

RadarView::RadarPhysicsParameters CVistarObject::radarPhysicsParameters() const
{
    return _m_radarPhysics;
}

// ============ Trajectory Control Methods ============

void CVistarObject::setTrajectoryEnabled(bool enabled) {
    _m_bTrajectoryEnabled = enabled;
    if (!enabled) {
        // Optionally clear trajectory when disabled
        // _m_listTrajectoryPoints.clear();
    }
    refresh();
}

bool CVistarObject::isTrajectoryEnabled() const {
    return _m_bTrajectoryEnabled;
}

void CVistarObject::clearTrajectory() {
    _m_listTrajectoryPoints.clear();
    refresh();
}

QList<QgsPointXYZ> CVistarObject::getTrajectory() const {
    return _m_listTrajectoryPoints;
}

void CVistarObject::setTrajectoryColor(const QColor &color) {
    _m_trajectoryColor = color;
    refresh();
}

QColor CVistarObject::getTrajectoryColor() const {
    return _m_trajectoryColor;
}

void CVistarObject::setTrajectoryWidth(int width) {
    _m_nTrajectoryWidth = qBound(1, width, 5);  // Clamp between 1 and 10 pixels
    refresh();
}

int CVistarObject::getTrajectoryWidth() const {
    return _m_nTrajectoryWidth;
}

void CVistarObject::addTrajectoryPoint(double dLon, double dLat, double dAlt) {
    // Manually add a trajectory point (useful for replaying saved trajectories)
    _m_listTrajectoryPoints.append(QgsPointXYZ(dLon, dLat, dAlt));
    refresh();
}

void CVistarObject::setJammerParams(const JammerParams &params)       { _m_jammerParams = params; }
JammerParams CVistarObject::jammerParams() const                       { return _m_jammerParams; }

void CVistarObject::setClutterParams(const ClutterParams &params)      { _m_clutterParams = params; }
ClutterParams CVistarObject::clutterParams() const                     { return _m_clutterParams; }

void CVistarObject::setRFDetectorParams(const RFDetectorParams &params) { _m_rfDetectorParams = params; }
RFDetectorParams CVistarObject::rfDetectorParams() const                { return _m_rfDetectorParams; }
