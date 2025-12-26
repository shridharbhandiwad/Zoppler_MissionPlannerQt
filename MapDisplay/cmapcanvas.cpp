#include "cmapcanvas.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

CMapCanvas::CMapCanvas(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "CMapCanvas constructed";
}

CMapCanvas::~CMapCanvas()
{
    qDebug() << "CMapCanvas destroyed";
}

void CMapCanvas::Initialize()
{
    // Initialize the map canvas
    qDebug() << "CMapCanvas::Initialize() called";
    // TODO: Add initialization code for map display
}

void CMapCanvas::InitializeAllObjects()
{
    // Clear and reinitialize all objects on the canvas
    qDebug() << "CMapCanvas::InitializeAllObjects() called";
    // TODO: Implement object initialization/clearing
}

void CMapCanvas::SetObjectToLoadOnClick(int nClass)
{
    // Set the type of object to be loaded when clicking on the map
    qDebug() << "CMapCanvas::SetObjectToLoadOnClick() called with class:" << nClass;
    // TODO: Implement object class setting
}

void CMapCanvas::slotUpdateObject(QJsonDocument doc)
{
    // Update or add an object on the canvas
    qDebug() << "CMapCanvas::slotUpdateObject() called";
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON document for object update";
        return;
    }
    
    QJsonObject obj = doc.object();
    
    // Extract object data
    QString id = obj["OBJ_ID"].toString();
    QString type = obj["OBJ_CLASS"].toString();
    
    qDebug() << "Updating object - ID:" << id << "Type:" << type;
    
    // TODO: Implement actual object creation/update on the map canvas
    // This would typically involve:
    // 1. Parsing the position data (LAT, LON, ALT)
    // 2. Creating or updating a visual representation
    // 3. Adding it to the map display
}

void CMapCanvas::slotLoadRoute(QJsonDocument doc)
{
    // Load a route on the canvas
    qDebug() << "CMapCanvas::slotLoadRoute() called";
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON document for route loading";
        return;
    }
    
    QJsonObject routeObj = doc.object();
    
    // Extract route data
    QString id = routeObj["ROUTE_ID"].toString();
    QString name = routeObj["ROUTE_NAME"].toString();
    QJsonArray waypoints = routeObj["WAYPOINTS"].toArray();
    
    qDebug() << "Loading route - ID:" << id << "Name:" << name << "Waypoints:" << waypoints.size();
    
    // Process each waypoint
    for (const QJsonValue &wpVal : waypoints) {
        QJsonObject wp = wpVal.toObject();
        double lat = wp["LAT"].toDouble();
        double lon = wp["LON"].toDouble();
        double alt = wp["ALT"].toDouble();
        
        qDebug() << "  Waypoint: Lat=" << lat << "Lon=" << lon << "Alt=" << alt;
    }
    
    // TODO: Implement actual route visualization on the map canvas
    // This would typically involve:
    // 1. Creating waypoint markers
    // 2. Drawing connecting lines between waypoints
    // 3. Adding route labels and information
    // 4. Storing the route for future reference/editing
}
