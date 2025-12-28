#ifndef CVISTARROUTE_H
#define CVISTARROUTE_H

#include <qgsmapcanvasitem.h>
#include <qgsmapcanvas.h>
#include "qgspointxyz.h"
#include <QStringList>

class CVistarRoute : public QgsMapCanvasItem
{
public:
    CVistarRoute(QgsMapCanvas *canvas);
    CVistarRoute(QgsMapCanvas *canvas,QString sObjectID,
                 double dStartLon, double dStartLat);

    void addPoint(QgsPointXY pt);
    void TransmitSelfInfo();
    void refresh();
    QString getObjectId();
    int getPointCount();
    QgsPointXY getPointAt(int index);
    void setHighlighted(bool bHighlight);
    bool isHighlighted();
    QList<QgsPointXYZ> getPoints();
    void UpdatePoints(QList<QgsPointXYZ> points);
    void UpdatePoints(QList<QgsPointXYZ> points, QStringList maneuverTypes);
    QStringList getManeuverTypes();
    void setManeuverTypes(QStringList maneuverTypes);
private :
    QgsMapCanvas *_m_canvas;
    QString _m_sObjectID;
    QList<QgsPointXYZ> _m_listPoints;
    QStringList _m_listManeuverTypes;
    bool _m_bHighlight;

    QImage _m_Image;
    int _m_nImageSize;

protected :
    void paint(QPainter *painter) override;
    QRectF boundingRect() const override;
};

#endif // CVISTARROUTE_H
