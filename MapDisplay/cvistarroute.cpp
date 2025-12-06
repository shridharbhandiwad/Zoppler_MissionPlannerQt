#include "cvistarroute.h"
#include <qgsgeometry.h>
#include <qgsfeature.h>
#include <qgsvectordataprovider.h>
#include <qgssinglesymbolrenderer.h>
#include <qgslinesymbol.h>
#include <qgsfield.h>
#include <qgslinestring.h>

CVistarRoute::CVistarRoute(QgsMapCanvas *pMapCanvas, QObject *parent)
    : QObject{parent}
    , m_pMapCanvas(pMapCanvas)
    , m_pLayer(nullptr)
    , m_color(Qt::cyan)
    , m_bVisible(false)
{
}

CVistarRoute::~CVistarRoute()
{
    if (m_pLayer) {
        if (m_pMapCanvas) {
            QList<QgsMapLayer*> layers = m_pMapCanvas->layers();
            if (layers.contains(m_pLayer)) {
                layers.removeOne(m_pLayer);
                m_pMapCanvas->setLayers(layers);
            }
        }
        delete m_pLayer;
        m_pLayer = nullptr;
    }
}

void CVistarRoute::Update(const QJsonObject &jsonData)
{
    if (jsonData.contains("ID")) {
        m_strRouteID = jsonData["ID"].toString();
    }
    
    if (jsonData.contains("WAYPOINTS") && jsonData["WAYPOINTS"].isArray()) {
        ClearWaypoints();
        QJsonArray waypoints = jsonData["WAYPOINTS"].toArray();
        for (const QJsonValue &wpValue : waypoints) {
            if (wpValue.isObject()) {
                QJsonObject wp = wpValue.toObject();
                double dLat = wp["LAT"].toDouble();
                double dLon = wp["LON"].toDouble();
                double dAlt = wp.contains("ALT") ? wp["ALT"].toDouble() : 0.0;
                AddWaypoint(dLat, dLon, dAlt);
            }
        }
    }
    
    if (jsonData.contains("COLOR")) {
        QString strColor = jsonData["COLOR"].toString();
        m_color = QColor(strColor);
    }
    
    _updateLayer();
}

void CVistarRoute::SetRouteID(const QString &strID)
{
    m_strRouteID = strID;
}

void CVistarRoute::AddWaypoint(double dLat, double dLon, double dAlt)
{
    m_waypoints.append(QgsPointXYZ(dLon, dLat, dAlt)); // Note: QGS uses Lon, Lat order
    _updateLayer();
}

void CVistarRoute::ClearWaypoints()
{
    m_waypoints.clear();
    _updateLayer();
}

void CVistarRoute::SetColor(const QColor &color)
{
    m_color = color;
    if (m_pLayer) {
        _createLayer(); // Recreate with new color
    }
}

void CVistarRoute::Show()
{
    m_bVisible = true;
    if (!m_pLayer) {
        _createLayer();
    }
}

void CVistarRoute::Hide()
{
    m_bVisible = false;
    if (m_pLayer) {
        m_pLayer->setOpacity(0.0);
    }
}

void CVistarRoute::_createLayer()
{
    if (!m_pMapCanvas)
        return;

    // Delete old layer if exists
    if (m_pLayer) {
        delete m_pLayer;
        m_pLayer = nullptr;
    }

    // Create memory layer
    m_pLayer = new QgsVectorLayer("LineString?crs=EPSG:4326", m_strRouteID, "memory");
    
    if (!m_pLayer->isValid()) {
        qWarning() << "Failed to create route layer for" << m_strRouteID;
        return;
    }

    // Add attributes
    QgsVectorDataProvider *pProvider = m_pLayer->dataProvider();
    QList<QgsField> attributes;
    attributes << QgsField("ID", QVariant::String);
    pProvider->addAttributes(attributes);
    m_pLayer->updateFields();

    // Set symbol
    QgsLineSymbol *pSymbol = QgsLineSymbol::createSimple(QVariantMap());
    pSymbol->setWidth(2.0);
    pSymbol->setColor(m_color);
    
    m_pLayer->setRenderer(new QgsSingleSymbolRenderer(pSymbol));
    
    _updateLayer();
}

void CVistarRoute::_updateLayer()
{
    if (!m_pLayer || !m_pMapCanvas || m_waypoints.isEmpty())
        return;

    // Clear existing features
    QgsVectorDataProvider *pProvider = m_pLayer->dataProvider();
    QgsFeatureIds featureIds;
    QgsFeatureIterator it = m_pLayer->getFeatures();
    QgsFeature feature;
    while (it.nextFeature(feature)) {
        featureIds.insert(feature.id());
    }
    pProvider->deleteFeatures(featureIds);

    // Create line geometry from waypoints
    if (m_waypoints.size() >= 2) {
        QVector<QgsPoint> points;
        for (const QgsPointXYZ &wp : m_waypoints) {
            points.append(QgsPoint(wp.x(), wp.y()));
        }
        
        QgsLineString *lineString = new QgsLineString(points);
        QgsGeometry geom(lineString);
        
        QgsFeature newFeature;
        newFeature.setGeometry(geom);
        
        QgsAttributes attrs;
        attrs << m_strRouteID;
        newFeature.setAttributes(attrs);
        
        pProvider->addFeature(newFeature);
        m_pLayer->updateExtents();
    }
    
    // Refresh canvas
    if (m_bVisible) {
        QList<QgsMapLayer*> layers = m_pMapCanvas->layers();
        if (!layers.contains(m_pLayer)) {
            layers.append(m_pLayer);
            m_pMapCanvas->setLayers(layers);
        }
        m_pMapCanvas->refresh();
    }
}
