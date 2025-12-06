#ifndef CVISTARROUTE_H
#define CVISTARROUTE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include "qgspointxyz.h"

class CVistarRoute : public QObject
{
    Q_OBJECT
public:
    explicit CVistarRoute(QgsMapCanvas *pMapCanvas, QObject *parent = nullptr);
    ~CVistarRoute();

    void Update(const QJsonObject &jsonData);
    void SetRouteID(const QString &strID);
    void AddWaypoint(double dLat, double dLon, double dAlt = 0.0);
    void ClearWaypoints();
    void SetColor(const QColor &color);
    
    QString GetRouteID() const { return m_strRouteID; }
    QList<QgsPointXYZ> GetWaypoints() const { return m_waypoints; }
    
    void Show();
    void Hide();
    bool IsVisible() const { return m_bVisible; }

private:
    void _createLayer();
    void _updateLayer();
    
private:
    QgsMapCanvas *m_pMapCanvas;
    QgsVectorLayer *m_pLayer;
    
    QString m_strRouteID;
    QList<QgsPointXYZ> m_waypoints;
    QColor m_color;
    bool m_bVisible;
    
signals:
};

#endif // CVISTARROUTE_H
