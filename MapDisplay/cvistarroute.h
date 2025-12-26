#ifndef CVISTARROUTE_H
#define CVISTARROUTE_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include "qgspointxyz.h"

class CVistarRoute
{
public:
    CVistarRoute();
    CVistarRoute(const QString &id, const QString &name);
    ~CVistarRoute();
    
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QVector<QgsPointXYZ> waypoints() const { return m_waypoints; }
    
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setWaypoints(const QVector<QgsPointXYZ> &waypoints) { m_waypoints = waypoints; }
    void addWaypoint(const QgsPointXYZ &waypoint) { m_waypoints.append(waypoint); }
    void clearWaypoints() { m_waypoints.clear(); }
    
    QJsonObject toJson() const;
    static CVistarRoute fromJson(const QJsonObject &json);
    
private:
    QString m_id;
    QString m_name;
    QVector<QgsPointXYZ> m_waypoints;
};

#endif // CVISTARROUTE_H
