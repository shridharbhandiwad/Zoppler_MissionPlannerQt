#include "cvistarroute.h"
#include <QJsonArray>

CVistarRoute::CVistarRoute()
    : m_id("")
    , m_name("")
{
}

CVistarRoute::CVistarRoute(const QString &id, const QString &name)
    : m_id(id)
    , m_name(name)
{
}

CVistarRoute::~CVistarRoute()
{
}

QJsonObject CVistarRoute::toJson() const
{
    QJsonObject obj;
    obj["ID"] = m_id;
    obj["NAME"] = m_name;
    
    QJsonArray waypointsArray;
    for (const QgsPointXYZ &wp : m_waypoints) {
        QJsonObject wpObj;
        wpObj["LAT"] = wp.y();
        wpObj["LON"] = wp.x();
        wpObj["ALT"] = wp.z();
        waypointsArray.append(wpObj);
    }
    obj["WAYPOINTS"] = waypointsArray;
    
    return obj;
}

CVistarRoute CVistarRoute::fromJson(const QJsonObject &json)
{
    QString id = json["ID"].toString();
    QString name = json["NAME"].toString();
    
    CVistarRoute route(id, name);
    
    QJsonArray waypointsArray = json["WAYPOINTS"].toArray();
    for (const QJsonValue &wpVal : waypointsArray) {
        QJsonObject wpObj = wpVal.toObject();
        double lat = wpObj["LAT"].toDouble();
        double lon = wpObj["LON"].toDouble();
        double alt = wpObj["ALT"].toDouble();
        route.addWaypoint(QgsPointXYZ(lon, lat, alt));
    }
    
    return route;
}
