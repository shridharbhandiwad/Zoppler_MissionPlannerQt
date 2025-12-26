#ifndef CVISTAROBJECT_H
#define CVISTAROBJECT_H

#include <qgsmapcanvasitem.h>
#include <qgsmapcanvas.h>
#include "qgspointxyz.h"
class CVistarObject : public QgsMapCanvasItem
{

public:
    CVistarObject(QgsMapCanvas *canvas);
    CVistarObject(QgsMapCanvas *canvas,QString sObjectID, int nClass, double dLon, double dLat);

    bool isValid();
    void attachRoute(QString sRoute);
    void setParent(QString sParent , int nChildId);
    void TransmitSelfInfo();
    QString getClassAsString();
    void UpdateObject(QJsonObject doc);
    void refresh();
    QRectF boundingRect() const override;
    QgsPointXY getPoint();
    QgsPointXYZ getPointXYZ();
    void setHighlighted(bool bHighlight);
    bool isHighlighted();
    QString getObjectId();
    void UpdateLocation(double dLat, double dLon, double dAlt);
    QString getParent();
    int getChildId();
private :
    QgsMapCanvas *_m_canvas;
    QString _m_sObjectID;
    int _m_nClass;
    double _m_dLon;
    double _m_dLat;
    double _m_dAlt;
    double _m_dHeading;
    double _m_dVelocity;

    bool _m_bRefresh;

    QImage _m_Image;
    int _m_nImageSize;

    bool _m_bHighlight;

    int _m_nChildId;
    QString _m_sParentObject;
    QString _m_sAttachedRoute;

protected :
    void paint(QPainter *painter) override;

};

#endif // CVISTAROBJECT_H
