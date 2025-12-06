#include "cvistarobject.h"
#include <qgsgeometry.h>
#include <qgsfeature.h>
#include <qgsvectordataprovider.h>
#include <qgssinglesymbolrenderer.h>
#include <qgssymbollayerutils.h>
#include <qgsfillsymbol.h>
#include <qgsfield.h>

CVistarObject::CVistarObject(QgsMapCanvas *pMapCanvas, QObject *parent)
    : QObject{parent}
    , m_pMapCanvas(pMapCanvas)
    , m_pLayer(nullptr)
    , m_nObjectClass(VISTAR_CLASS_NONE)
    , m_position(0, 0, 0)
    , m_dHeading(0.0)
    , m_dVelocity(0.0)
    , m_bVisible(false)
{
}

CVistarObject::~CVistarObject()
{
    if (m_pLayer) {
        if (m_pMapCanvas) {
            QList<QgsMapLayer*> layers = m_pMapCanvas->layers();
            if (layers.contains(m_pLayer)) {
                m_pMapCanvas->setLayers(QList<QgsMapLayer*>());
            }
        }
        delete m_pLayer;
        m_pLayer = nullptr;
    }
}

void CVistarObject::Update(const QJsonObject &jsonData)
{
    // Parse JSON data
    if (jsonData.contains("ID")) {
        m_strObjectID = jsonData["ID"].toString();
    }
    
    if (jsonData.contains("CLASS")) {
        m_nObjectClass = jsonData["CLASS"].toInt();
    }
    
    if (jsonData.contains("LAT") && jsonData.contains("LON")) {
        double dLat = jsonData["LAT"].toDouble();
        double dLon = jsonData["LON"].toDouble();
        double dAlt = jsonData.contains("ALT") ? jsonData["ALT"].toDouble() : 0.0;
        SetPosition(dLat, dLon, dAlt);
    }
    
    if (jsonData.contains("HEADING")) {
        m_dHeading = jsonData["HEADING"].toDouble();
    }
    
    if (jsonData.contains("VELOCITY")) {
        m_dVelocity = jsonData["VELOCITY"].toDouble();
    }
    
    // Update display
    _updateLayer();
}

void CVistarObject::SetObjectID(const QString &strID)
{
    m_strObjectID = strID;
}

void CVistarObject::SetObjectClass(int nClass)
{
    m_nObjectClass = nClass;
    if (m_pLayer) {
        _createLayer(); // Recreate layer with new class
    }
}

void CVistarObject::SetPosition(double dLat, double dLon, double dAlt)
{
    m_position = QgsPointXYZ(dLon, dLat, dAlt); // Note: QGS uses Lon, Lat order
    _updateLayer();
}

void CVistarObject::SetHeading(double dHeading)
{
    m_dHeading = dHeading;
    _updateLayer();
}

void CVistarObject::SetVelocity(double dVelocity)
{
    m_dVelocity = dVelocity;
}

void CVistarObject::Show()
{
    m_bVisible = true;
    if (!m_pLayer) {
        _createLayer();
    }
}

void CVistarObject::Hide()
{
    m_bVisible = false;
    if (m_pLayer) {
        m_pLayer->setOpacity(0.0);
    }
}

void CVistarObject::_createLayer()
{
    if (!m_pMapCanvas)
        return;

    // Delete old layer if exists
    if (m_pLayer) {
        delete m_pLayer;
        m_pLayer = nullptr;
    }

    // Create memory layer
    m_pLayer = new QgsVectorLayer("Point?crs=EPSG:4326", m_strObjectID, "memory");
    
    if (!m_pLayer->isValid()) {
        qWarning() << "Failed to create layer for" << m_strObjectID;
        return;
    }

    // Add attributes
    QgsVectorDataProvider *pProvider = m_pLayer->dataProvider();
    QList<QgsField> attributes;
    attributes << QgsField("ID", QVariant::String);
    attributes << QgsField("CLASS", QVariant::Int);
    attributes << QgsField("HEADING", QVariant::Double);
    attributes << QgsField("VELOCITY", QVariant::Double);
    pProvider->addAttributes(attributes);
    m_pLayer->updateFields();

    // Set symbol
    QColor color = _getColorForClass(m_nObjectClass);
    QgsMarkerSymbol *pSymbol = QgsMarkerSymbol::createSimple(QVariantMap());
    pSymbol->setSize(8);
    pSymbol->setColor(color);
    
    m_pLayer->setRenderer(new QgsSingleSymbolRenderer(pSymbol));
    
    _updateLayer();
}

void CVistarObject::_updateLayer()
{
    if (!m_pLayer || !m_pMapCanvas)
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

    // Add new feature
    QgsFeature newFeature;
    newFeature.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(m_position.x(), m_position.y())));
    
    QgsAttributes attrs;
    attrs << m_strObjectID << m_nObjectClass << m_dHeading << m_dVelocity;
    newFeature.setAttributes(attrs);
    
    pProvider->addFeature(newFeature);
    m_pLayer->updateExtents();
    
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

QColor CVistarObject::_getColorForClass(int nClass)
{
    switch (nClass) {
        case VISTAR_CLASS_DRONE:
            return QColor(0, 255, 0); // Green
        case VISTAR_CLASS_DRONE_SWARM:
            return QColor(0, 200, 100); // Light green
        case VISTAR_CLASS_FIGHTER:
            return QColor(255, 0, 0); // Red
        case VISTAR_CLASS_UAV:
            return QColor(0, 150, 255); // Light blue
        case VISTAR_CLASS_RADAR:
            return QColor(255, 165, 0); // Orange
        case VISTAR_CLASS_LAUNCHER:
            return QColor(128, 0, 128); // Purple
        case VISTAR_CLASS_MISSILE:
            return QColor(255, 255, 0); // Yellow
        default:
            return QColor(200, 200, 200); // Gray
    }
}

QString CVistarObject::_getIconForClass(int nClass)
{
    // Icon names for future use
    switch (nClass) {
        case VISTAR_CLASS_DRONE:
            return "drone";
        case VISTAR_CLASS_FIGHTER:
            return "fighter";
        case VISTAR_CLASS_UAV:
            return "uav";
        case VISTAR_CLASS_RADAR:
            return "radar";
        case VISTAR_CLASS_LAUNCHER:
            return "launcher";
        case VISTAR_CLASS_MISSILE:
            return "missile";
        default:
            return "default";
    }
}
