#include "cmapcanvas.h"
#include "globalConstants.h"
#include <qgslayertree.h>
#include <qgsproject.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include <qgsmaplayerstore.h>
#include <QDebug>

CMapCanvas::CMapCanvas(QWidget *parent)
    : QWidget{parent}
    , m_pMapCanvas(nullptr)
    , m_pLayout(nullptr)
    , m_pMapTool(nullptr)
    , m_nObjectToCreate(VISTAR_CLASS_NONE)
    , m_nObjectCounter(0)
{
}

CMapCanvas::~CMapCanvas()
{
    // Clean up objects
    qDeleteAll(m_objects);
    m_objects.clear();
    
    qDeleteAll(m_routes);
    m_routes.clear();
    
    if (m_pMapTool) {
        delete m_pMapTool;
        m_pMapTool = nullptr;
    }
    
    if (m_pMapCanvas) {
        delete m_pMapCanvas;
        m_pMapCanvas = nullptr;
    }
}

void CMapCanvas::Initialize()
{
    _setupCanvas();
    _addBaseMap();
}

void CMapCanvas::_setupCanvas()
{
    // Create layout
    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create map canvas
    m_pMapCanvas = new QgsMapCanvas(this);
    m_pMapCanvas->setCanvasColor(QColor(0, 0, 0));
    m_pMapCanvas->enableAntiAliasing(true);
    
    // Set initial extent (world view)
    QgsRectangle extent(-180, -90, 180, 90);
    m_pMapCanvas->setExtent(extent);
    
    m_pLayout->addWidget(m_pMapCanvas);
    
    // Create map tool
    m_pMapTool = new CMapToolSelectObjects(m_pMapCanvas);
    connect(m_pMapTool, &CMapToolSelectObjects::signalObjectCreated,
            this, &CMapCanvas::_slotObjectCreated);
    
    m_pMapCanvas->setMapTool(m_pMapTool);
    
    qDebug() << "[CMapCanvas] Initialized successfully";
}

void CMapCanvas::_addBaseMap()
{
    // Create a simple base layer (can be replaced with actual map tiles)
    QgsVectorLayer *pBaseLayer = new QgsVectorLayer("Polygon?crs=EPSG:4326", "Base", "memory");
    if (pBaseLayer && pBaseLayer->isValid()) {
        QList<QgsMapLayer*> layers;
        layers.append(pBaseLayer);
        m_pMapCanvas->setLayers(layers);
        m_pMapCanvas->refresh();
    }
}

void CMapCanvas::InitializeAllObjects()
{
    // Initialize all objects for display
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        it.value()->Show();
    }
    
    for (auto it = m_routes.begin(); it != m_routes.end(); ++it) {
        it.value()->Show();
    }
    
    m_pMapCanvas->refresh();
    qDebug() << "[CMapCanvas] All objects initialized";
}

void CMapCanvas::SetObjectToLoadOnClick(int nClass)
{
    m_nObjectToCreate = nClass;
    if (m_pMapTool) {
        m_pMapTool->SetObjectToCreate(nClass);
    }
    qDebug() << "[CMapCanvas] Object to create set to class:" << nClass;
}

void CMapCanvas::slotUpdateObject(QJsonDocument doc)
{
    if (!doc.isObject()) {
        qWarning() << "[CMapCanvas] Received invalid JSON document";
        return;
    }
    
    QJsonObject jsonRoot = doc.object();
    
    qDebug() << "[CMapCanvas] Received data:" << doc.toJson(QJsonDocument::Compact);
    
    // Check what type of data this is
    QString strStream = jsonRoot.value("STREAM").toString();
    QString strType = jsonRoot.value("TYPE").toString();
    
    if (strStream == "Object" || jsonRoot.contains("ID")) {
        // This is an object update
        QString strID = jsonRoot.value("ID").toString();
        int nClass = jsonRoot.value("CLASS").toInt();
        
        if (strID.isEmpty()) {
            qWarning() << "[CMapCanvas] Object has no ID";
            return;
        }
        
        // Check if object exists
        if (!m_objects.contains(strID)) {
            // Create new object
            qDebug() << "[CMapCanvas] Creating new object:" << strID << "class:" << nClass;
            CVistarObject *pObject = new CVistarObject(m_pMapCanvas, this);
            pObject->SetObjectID(strID);
            pObject->SetObjectClass(nClass);
            m_objects[strID] = pObject;
        }
        
        // Update object
        m_objects[strID]->Update(jsonRoot);
        m_objects[strID]->Show();
        
        // Display status
        QString strStatus = QString("Updated: %1 [Class:%2] Lat:%3 Lon:%4")
            .arg(strID)
            .arg(nClass)
            .arg(jsonRoot.value("LAT").toDouble(), 0, 'f', 6)
            .arg(jsonRoot.value("LON").toDouble(), 0, 'f', 6);
        emit signalMouseRead(strStatus);
        
    } else if (strStream == "Route" || jsonRoot.contains("WAYPOINTS")) {
        // This is a route update
        QString strID = jsonRoot.value("ID").toString();
        
        if (strID.isEmpty()) {
            qWarning() << "[CMapCanvas] Route has no ID";
            return;
        }
        
        // Check if route exists
        if (!m_routes.contains(strID)) {
            qDebug() << "[CMapCanvas] Creating new route:" << strID;
            CVistarRoute *pRoute = new CVistarRoute(m_pMapCanvas, this);
            pRoute->SetRouteID(strID);
            m_routes[strID] = pRoute;
        }
        
        // Update route
        m_routes[strID]->Update(jsonRoot);
        m_routes[strID]->Show();
        
    } else if (strStream == "Event") {
        // Handle event messages
        qDebug() << "[CMapCanvas] Event received:" << strType;
        
        if (strType == "Start") {
            InitializeAllObjects();
        } else if (strType == "Stop") {
            // Handle stop event
        } else if (strType == "Clear") {
            // Clear all objects
            qDeleteAll(m_objects);
            m_objects.clear();
            qDeleteAll(m_routes);
            m_routes.clear();
            m_pMapCanvas->refresh();
        }
    } else {
        qDebug() << "[CMapCanvas] Unknown message type - treating as object update";
        // Try to treat it as an object anyway
        QString strID = jsonRoot.value("ID").toString();
        if (!strID.isEmpty()) {
            int nClass = jsonRoot.value("CLASS").toInt(VISTAR_CLASS_DRONE);
            
            if (!m_objects.contains(strID)) {
                CVistarObject *pObject = new CVistarObject(m_pMapCanvas, this);
                pObject->SetObjectID(strID);
                pObject->SetObjectClass(nClass);
                m_objects[strID] = pObject;
            }
            
            m_objects[strID]->Update(jsonRoot);
            m_objects[strID]->Show();
        }
    }
    
    m_pMapCanvas->refresh();
}

void CMapCanvas::_slotObjectCreated(int nClass, double dLat, double dLon)
{
    // Create new object at clicked location
    QString strID = _generateObjectID(nClass);
    
    qDebug() << "[CMapCanvas] Creating object" << strID << "at" << dLat << "," << dLon;
    
    CVistarObject *pObject = new CVistarObject(m_pMapCanvas, this);
    pObject->SetObjectID(strID);
    pObject->SetObjectClass(nClass);
    pObject->SetPosition(dLat, dLon, 0.0);
    pObject->Show();
    
    m_objects[strID] = pObject;
    
    // Publish object creation to network
    QJsonObject jsonObj;
    jsonObj["STREAM"] = "Object";
    jsonObj["TYPE"] = "Create";
    jsonObj["ID"] = strID;
    jsonObj["CLASS"] = nClass;
    jsonObj["LAT"] = dLat;
    jsonObj["LON"] = dLon;
    jsonObj["ALT"] = 0.0;
    jsonObj["SOURCE"] = VISTAR_SRC_MISSION_PLANNER;
    
    // Could send this via network interface
    // CNetworkInterface::PublishMessage(QJsonDocument(jsonObj));
    
    m_pMapCanvas->refresh();
    
    // Clear selection
    emit signalClearObjectSelection();
}

QString CMapCanvas::_generateObjectID(int nClass)
{
    QString strPrefix;
    switch (nClass) {
        case VISTAR_CLASS_DRONE:
            strPrefix = "DRONE";
            break;
        case VISTAR_CLASS_DRONE_SWARM:
            strPrefix = "SWARM";
            break;
        case VISTAR_CLASS_FIGHTER:
            strPrefix = "FIGHTER";
            break;
        case VISTAR_CLASS_UAV:
            strPrefix = "UAV";
            break;
        case VISTAR_CLASS_RADAR:
            strPrefix = "RADAR";
            break;
        case VISTAR_CLASS_LAUNCHER:
            strPrefix = "LAUNCHER";
            break;
        case VISTAR_CLASS_MISSILE:
            strPrefix = "MISSILE";
            break;
        case VISTAR_CLASS_ROUTE:
            strPrefix = "ROUTE";
            break;
        default:
            strPrefix = "OBJ";
    }
    
    return QString("%1_%2").arg(strPrefix).arg(++m_nObjectCounter, 3, 10, QChar('0'));
}

void CMapCanvas::importRasterMap(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return;
    }
    
    QgsRasterLayer *pRasterLayer = new QgsRasterLayer(filePath, "Map", "gdal");
    
    if (pRasterLayer && pRasterLayer->isValid()) {
        QList<QgsMapLayer*> layers = m_pMapCanvas->layers();
        layers.prepend(pRasterLayer); // Add at bottom
        m_pMapCanvas->setLayers(layers);
        m_pMapCanvas->setExtent(pRasterLayer->extent());
        m_pMapCanvas->refresh();
        qDebug() << "[CMapCanvas] Raster map loaded:" << filePath;
    } else {
        qWarning() << "[CMapCanvas] Failed to load raster map:" << filePath;
        delete pRasterLayer;
    }
}
