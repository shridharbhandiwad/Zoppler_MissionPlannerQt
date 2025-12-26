#include "cmapcanvas.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <qgsapplication.h>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include <qgsproject.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsgeometry.h>
#include <qgspointxy.h>
#include <qgsfeature.h>
#include <qgssymbol.h>
#include <qgsrenderer.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsmarkersymbol.h>

CMapCanvas::CMapCanvas(QWidget *parent)
    : QWidget(parent)
    , m_mapCanvas(nullptr)
    , m_layout(nullptr)
    , m_rasterLayer(nullptr)
    , m_objectLayer(nullptr)
    , m_selectedObjectClass(0)
{
    qDebug() << "CMapCanvas constructed";
    
    // Create layout
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    
    // Create QGIS map canvas
    m_mapCanvas = new QgsMapCanvas(this);
    m_mapCanvas->setCanvasColor(QColor(30, 30, 30));
    m_layout->addWidget(m_mapCanvas);
    
    // Set default CRS to WGS84 (EPSG:4326)
    m_mapCanvas->setDestinationCrs(QgsCoordinateReferenceSystem("EPSG:4326"));
    
    // Connect mouse move signal
    connect(m_mapCanvas, &QgsMapCanvas::xyCoordinates, 
            this, &CMapCanvas::onMapCanvasMouseMove);
}

CMapCanvas::~CMapCanvas()
{
    qDebug() << "CMapCanvas destroyed";
    if (m_rasterLayer) {
        QgsProject::instance()->removeMapLayer(m_rasterLayer);
    }
    if (m_objectLayer) {
        QgsProject::instance()->removeMapLayer(m_objectLayer);
    }
}

void CMapCanvas::Initialize()
{
    qDebug() << "CMapCanvas::Initialize() called";
    
    // Initialize QGIS Application
    QgsApplication::setPrefixPath("/usr", true);
    QgsApplication::initQgis();
    
    // Create a vector layer for objects if it doesn't exist
    if (!m_objectLayer) {
        m_objectLayer = new QgsVectorLayer("Point?crs=EPSG:4326", "Objects", "memory");
        if (m_objectLayer->isValid()) {
            QgsProject::instance()->addMapLayer(m_objectLayer, false);
            qDebug() << "Object layer created successfully";
        } else {
            qWarning() << "Failed to create object layer";
        }
    }
    
    // Set initial extent to show a reasonable default area (e.g., world view)
    m_mapCanvas->setExtent(QgsRectangle(-180, -90, 180, 90));
    m_mapCanvas->refresh();
    
    qDebug() << "Map canvas initialized successfully";
}

void CMapCanvas::InitializeAllObjects()
{
    qDebug() << "CMapCanvas::InitializeAllObjects() called";
    
    // Clear all objects
    m_objects.clear();
    m_routes.clear();
    
    // Clear object layer features
    if (m_objectLayer && m_objectLayer->isValid()) {
        m_objectLayer->dataProvider()->truncate();
        m_objectLayer->triggerRepaint();
    }
    
    m_mapCanvas->refresh();
    qDebug() << "All objects cleared from canvas";
}

void CMapCanvas::SetObjectToLoadOnClick(int nClass)
{
    m_selectedObjectClass = nClass;
    qDebug() << "CMapCanvas::SetObjectToLoadOnClick() called with class:" << nClass;
}

void CMapCanvas::importRasterMap(const QString &filePath)
{
    qDebug() << "Importing raster map:" << filePath;
    
    if (filePath.isEmpty()) {
        qWarning() << "Empty file path provided";
        return;
    }
    
    // Remove existing raster layer if present
    if (m_rasterLayer) {
        QgsProject::instance()->removeMapLayer(m_rasterLayer);
        m_rasterLayer = nullptr;
    }
    
    // Create new raster layer
    m_rasterLayer = new QgsRasterLayer(filePath, "Map Layer");
    
    if (!m_rasterLayer->isValid()) {
        qWarning() << "Failed to load raster layer:" << filePath;
        qWarning() << "Error:" << m_rasterLayer->error().message();
        QMessageBox::warning(nullptr, "Map Load Error", 
            "Failed to load map file:\n" + filePath + "\n\nError: " + m_rasterLayer->error().message());
        delete m_rasterLayer;
        m_rasterLayer = nullptr;
        return;
    }
    
    // Add layer to project and canvas
    QgsProject::instance()->addMapLayer(m_rasterLayer, false);
    
    // Update canvas layers
    QList<QgsMapLayer*> layers;
    if (m_objectLayer && m_objectLayer->isValid()) {
        layers.append(m_objectLayer);
    }
    layers.append(m_rasterLayer);
    
    m_mapCanvas->setLayers(layers);
    
    // Zoom to layer extent
    m_mapCanvas->setExtent(m_rasterLayer->extent());
    m_mapCanvas->refresh();
    
    qDebug() << "Raster map loaded successfully";
    qDebug() << "Map extent:" << m_rasterLayer->extent().toString();
}

void CMapCanvas::slotUpdateObject(QJsonDocument doc)
{
    qDebug() << "CMapCanvas::slotUpdateObject() called";
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON document for object update";
        return;
    }
    
    QJsonObject obj = doc.object();
    
    // Extract object data - handle different field name variations
    QString id = obj["OBJ_ID"].toString();
    if (id.isEmpty()) id = obj["ID"].toString();
    
    QString type = obj["OBJ_CLASS"].toString();
    if (type.isEmpty()) type = obj["TYPE"].toString();
    
    double lat = obj["LAT"].toDouble();
    double lon = obj["LON"].toDouble();
    double alt = obj["ALT"].toDouble();
    
    qDebug() << "Updating object - ID:" << id << "Type:" << type 
             << "Position:" << lat << "," << lon << "," << alt;
    
    // Store object data
    m_objects[id] = obj;
    
    // Create/update feature on map
    if (m_objectLayer && m_objectLayer->isValid()) {
        // For now, just mark the position with a point
        // In a full implementation, you would create custom symbols based on object type
        QgsPointXY point(lon, lat);
        
        m_mapCanvas->refresh();
    }
}

void CMapCanvas::slotLoadRoute(QJsonDocument doc)
{
    qDebug() << "CMapCanvas::slotLoadRoute() called";
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON document for route loading";
        return;
    }
    
    QJsonObject routeObj = doc.object();
    
    // Extract route data
    QString id = routeObj["ROUTE_ID"].toString();
    if (id.isEmpty()) id = routeObj["ID"].toString();
    
    QString name = routeObj["ROUTE_NAME"].toString();
    if (name.isEmpty()) name = routeObj["NAME"].toString();
    
    QJsonArray waypoints = routeObj["WAYPOINTS"].toArray();
    
    qDebug() << "Loading route - ID:" << id << "Name:" << name << "Waypoints:" << waypoints.size();
    
    // Store route data
    m_routes[id] = routeObj;
    
    // Process each waypoint
    QVector<QgsPointXY> routePoints;
    for (const QJsonValue &wpVal : waypoints) {
        QJsonObject wp = wpVal.toObject();
        double lat = wp["LAT"].toDouble();
        double lon = wp["LON"].toDouble();
        double alt = wp["ALT"].toDouble();
        
        qDebug() << "  Waypoint: Lat=" << lat << "Lon=" << lon << "Alt=" << alt;
        routePoints.append(QgsPointXY(lon, lat));
    }
    
    // In a full implementation, you would create a line feature to visualize the route
    m_mapCanvas->refresh();
}

void CMapCanvas::onMapCanvasMouseMove(const QgsPointXY &point)
{
    // Format coordinates for display
    QString coordInfo = QString("Lon: %1, Lat: %2")
        .arg(point.x(), 0, 'f', 6)
        .arg(point.y(), 0, 'f', 6);
    
    emit signalMouseRead(coordInfo);
}
