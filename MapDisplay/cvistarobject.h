#ifndef CVISTAROBJECT_H
#define CVISTAROBJECT_H

#include <qgsmapcanvasitem.h>
#include <qgsmapcanvas.h>
#include "qgspointxyz.h"
#include "../RadarView/radarattributes.h"
#include "../cclutterparamsdialog.h"

// Parameters for Jammer object
struct JammerParams {
    double txPowerW   = 1000.0;  // 1 kW
    double gainDbi    = 6.0;     // 6 dBi omni-directional
    double bandwidthHz = 30e6;
    double lossDb     = 2.0;
};

// Parameters for RF Detector object
struct RFDetectorParams {
    QString parentId     = "";
    double  mdsDbm       = -60.0;
    double  grDbm        = 0.0;
    double  noiseFigureDb = 10.0;
    double  bandwidth    = 20.0;
};

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
    QString getAttachedRoute();

    // Trajectory methods
    void setTrajectoryEnabled(bool enabled);
    bool isTrajectoryEnabled() const;
    void clearTrajectory();
    QList<QgsPointXYZ> getTrajectory() const;
    void setTrajectoryColor(const QColor &color);
    QColor getTrajectoryColor() const;
    void setTrajectoryWidth(int width);
    int getTrajectoryWidth() const;
    void addTrajectoryPoint(double dLon, double dLat, double dAlt);

    // Radar physics parameters (only meaningful when _m_nClass == VISTAR_CLASS_RADAR)
    void setRadarPhysicsParameters(const RadarView::RadarPhysicsParameters &params);
    RadarView::RadarPhysicsParameters radarPhysicsParameters() const;

    // Jammer parameters
    void setJammerParams(const JammerParams &params);
    JammerParams jammerParams() const;

    // Clutter parameters
    void setClutterParams(const ClutterParams &params);
    ClutterParams clutterParams() const;

    // Clutter density parameters
    void setClutterDensityParams(const ClutterDensityParams &params);
    ClutterDensityParams clutterDensityParams() const;

    // RF Detector parameters
    void setRFDetectorParams(const RFDetectorParams &params);
    RFDetectorParams rfDetectorParams() const;

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

    // Trajectory tracking
    bool _m_bTrajectoryEnabled;
    QList<QgsPointXYZ> _m_listTrajectoryPoints;
    QColor _m_trajectoryColor;
    int _m_nTrajectoryWidth;

    // Radar-specific physics/simulation parameters
    RadarView::RadarPhysicsParameters _m_radarPhysics;

    // New object-type parameters
    JammerParams         _m_jammerParams;
    ClutterParams        _m_clutterParams;
    ClutterDensityParams _m_clutterDensityParams;
    RFDetectorParams     _m_rfDetectorParams;

protected :
    void paint(QPainter *painter) override;

};

#endif // CVISTAROBJECT_H
