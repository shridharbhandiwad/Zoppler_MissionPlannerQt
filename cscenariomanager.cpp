#include "cscenariomanager.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

CScenarioManager::CScenarioManager(QObject *parent)
    : QObject(parent)
{
    _m_scenariosDirectory = getDefaultScenariosDirectory();
    ensureScenariosDirectoryExists();
}

CScenarioManager::~CScenarioManager()
{
}

QString CScenarioManager::getDefaultScenariosDirectory() const
{
    // Use application data directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appDataPath + "/scenarios";
}

bool CScenarioManager::ensureScenariosDirectoryExists()
{
    QDir dir;
    if (!dir.exists(_m_scenariosDirectory)) {
        if (dir.mkpath(_m_scenariosDirectory)) {
            qDebug() << "Created scenarios directory:" << _m_scenariosDirectory;
            return true;
        } else {
            qWarning() << "Failed to create scenarios directory:" << _m_scenariosDirectory;
            return false;
        }
    }
    return true;
}

bool CScenarioManager::saveScenario(const Scenario &scenario, const QString &filePath)
{
    QJsonObject jsonScenario = scenarioToJson(scenario);
    QJsonDocument doc(jsonScenario);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error("Failed to open file for writing: " + filePath);
        return false;
    }
    
    qint64 bytesWritten = file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    if (bytesWritten == -1) {
        emit error("Failed to write scenario to file: " + filePath);
        return false;
    }
    
    emit scenarioSaved(filePath);
    qDebug() << "Scenario saved successfully:" << filePath;
    return true;
}

bool CScenarioManager::loadScenario(const QString &filePath, Scenario &scenario)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error("Failed to open file for reading: " + filePath);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit error("JSON parse error: " + parseError.errorString());
        return false;
    }
    
    if (!doc.isObject()) {
        emit error("Invalid scenario format: root must be an object");
        return false;
    }
    
    scenario = jsonToScenario(doc.object());
    emit scenarioLoaded(scenario);
    qDebug() << "Scenario loaded successfully:" << filePath;
    return true;
}

QStringList CScenarioManager::getAvailableScenarios(const QString &directory)
{
    QString searchDir = directory.isEmpty() ? _m_scenariosDirectory : directory;
    QDir dir(searchDir);
    
    QStringList filters;
    filters << "*.json" << "*.scenario";
    
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);
    return files;
}

QJsonObject CScenarioManager::scenarioToJson(const Scenario &scenario)
{
    QJsonObject json;
    
    json["name"] = scenario.name;
    json["description"] = scenario.description;
    json["createdDate"] = scenario.createdDate;
    
    // Convert objects
    QJsonArray objectsArray;
    for (const ScenarioObject &obj : scenario.objects) {
        objectsArray.append(objectToJson(obj));
    }
    json["objects"] = objectsArray;
    
    // Convert routes
    QJsonArray routesArray;
    for (const ScenarioRoute &route : scenario.routes) {
        routesArray.append(routeToJson(route));
    }
    json["routes"] = routesArray;
    
    // Add metadata
    json["metadata"] = scenario.metadata;
    
    return json;
}

Scenario CScenarioManager::jsonToScenario(const QJsonObject &json)
{
    Scenario scenario;
    
    scenario.name = json["name"].toString();
    scenario.description = json["description"].toString();
    scenario.createdDate = json["createdDate"].toString();
    
    // Parse objects
    QJsonArray objectsArray = json["objects"].toArray();
    for (const QJsonValue &val : objectsArray) {
        scenario.objects.append(jsonToObject(val.toObject()));
    }
    
    // Parse routes
    QJsonArray routesArray = json["routes"].toArray();
    for (const QJsonValue &val : routesArray) {
        scenario.routes.append(jsonToRoute(val.toObject()));
    }
    
    // Parse metadata
    scenario.metadata = json["metadata"].toObject();
    
    return scenario;
}

QJsonObject CScenarioManager::objectToJson(const ScenarioObject &obj)
{
    QJsonObject json;
    json["id"] = obj.id;
    json["type"] = obj.type;
    json["latitude"] = obj.latitude;
    json["longitude"] = obj.longitude;
    json["altitude"] = obj.altitude;
    json["additionalData"] = obj.additionalData;
    return json;
}

ScenarioObject CScenarioManager::jsonToObject(const QJsonObject &json)
{
    ScenarioObject obj;
    obj.id = json["id"].toString();
    obj.type = json["type"].toString();
    obj.latitude = json["latitude"].toDouble();
    obj.longitude = json["longitude"].toDouble();
    obj.altitude = json["altitude"].toDouble();
    obj.additionalData = json["additionalData"].toObject();
    return obj;
}

QJsonObject CScenarioManager::routeToJson(const ScenarioRoute &route)
{
    QJsonObject json;
    json["id"] = route.id;
    json["name"] = route.name;
    
    QJsonArray waypointsArray;
    for (int i = 0; i < route.waypoints.size(); ++i) {
        QJsonObject waypointObj;
        waypointObj["latitude"] = route.waypoints[i].x();
        waypointObj["longitude"] = route.waypoints[i].y();
        if (i < route.altitudes.size()) {
            waypointObj["altitude"] = route.altitudes[i];
        }
        waypointsArray.append(waypointObj);
    }
    json["waypoints"] = waypointsArray;
    json["additionalData"] = route.additionalData;
    
    return json;
}

ScenarioRoute CScenarioManager::jsonToRoute(const QJsonObject &json)
{
    ScenarioRoute route;
    route.id = json["id"].toString();
    route.name = json["name"].toString();
    
    QJsonArray waypointsArray = json["waypoints"].toArray();
    for (const QJsonValue &val : waypointsArray) {
        QJsonObject wpObj = val.toObject();
        route.waypoints.append(QPointF(wpObj["latitude"].toDouble(), wpObj["longitude"].toDouble()));
        route.altitudes.append(wpObj["altitude"].toDouble(0.0));
    }
    
    route.additionalData = json["additionalData"].toObject();
    return route;
}

// Sample Scenario 1: Basic Defense Setup
Scenario CScenarioManager::createSampleScenario1()
{
    Scenario scenario;
    scenario.name = "Basic Defense Setup";
    scenario.description = "A basic air defense scenario with radar, launcher, and patrol route";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // Add Radar
    ScenarioObject radar;
    radar.id = "RADAR_001";
    radar.type = "RADAR";
    radar.latitude = 28.5000;
    radar.longitude = 77.2000;
    radar.altitude = 100.0;
    radar.additionalData["range"] = 50000;
    radar.additionalData["status"] = "active";
    scenario.objects.append(radar);
    
    // Add Launcher
    ScenarioObject launcher;
    launcher.id = "LAUNCHER_001";
    launcher.type = "LAUNCHER";
    launcher.latitude = 28.5100;
    launcher.longitude = 77.2100;
    launcher.altitude = 50.0;
    launcher.additionalData["missileCount"] = 8;
    launcher.additionalData["status"] = "ready";
    scenario.objects.append(launcher);
    
    // Add Fighter
    ScenarioObject fighter;
    fighter.id = "FIGHTER_001";
    fighter.type = "FIGHTER";
    fighter.latitude = 28.4800;
    fighter.longitude = 77.1800;
    fighter.altitude = 5000.0;
    fighter.additionalData["callsign"] = "ALPHA-1";
    fighter.additionalData["fuel"] = 75;
    scenario.objects.append(fighter);
    
    // Add patrol route
    ScenarioRoute patrolRoute;
    patrolRoute.id = "ROUTE_001";
    patrolRoute.name = "CAP Route Alpha";
    patrolRoute.waypoints.append(QPointF(28.4500, 77.1500));
    patrolRoute.altitudes.append(5000.0);
    patrolRoute.waypoints.append(QPointF(28.5500, 77.1500));
    patrolRoute.altitudes.append(5000.0);
    patrolRoute.waypoints.append(QPointF(28.5500, 77.2500));
    patrolRoute.altitudes.append(5000.0);
    patrolRoute.waypoints.append(QPointF(28.4500, 77.2500));
    patrolRoute.altitudes.append(5000.0);
    patrolRoute.additionalData["type"] = "patrol";
    scenario.routes.append(patrolRoute);
    
    scenario.metadata["author"] = "System";
    scenario.metadata["version"] = "1.0";
    scenario.metadata["difficulty"] = "easy";
    
    return scenario;
}

// Sample Scenario 2: Air Patrol Mission
Scenario CScenarioManager::createSampleScenario2()
{
    Scenario scenario;
    scenario.name = "Air Patrol Mission";
    scenario.description = "Multiple UAVs on patrol routes with surveillance coverage";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // Add UAV 1
    ScenarioObject uav1;
    uav1.id = "UAV_001";
    uav1.type = "UAV";
    uav1.latitude = 28.6000;
    uav1.longitude = 77.3000;
    uav1.altitude = 3000.0;
    uav1.additionalData["callsign"] = "REAPER-1";
    uav1.additionalData["endurance"] = 12;
    scenario.objects.append(uav1);
    
    // Add UAV 2
    ScenarioObject uav2;
    uav2.id = "UAV_002";
    uav2.type = "UAV";
    uav2.latitude = 28.4000;
    uav2.longitude = 77.3000;
    uav2.altitude = 3000.0;
    uav2.additionalData["callsign"] = "REAPER-2";
    uav2.additionalData["endurance"] = 12;
    scenario.objects.append(uav2);
    
    // Add Drone Swarm
    ScenarioObject swarm;
    swarm.id = "SWARM_001";
    swarm.type = "DRONE_SWARM";
    swarm.latitude = 28.5000;
    swarm.longitude = 77.2500;
    swarm.altitude = 1000.0;
    swarm.additionalData["droneCount"] = 10;
    swarm.additionalData["formation"] = "V";
    scenario.objects.append(swarm);
    
    // Add patrol route 1
    ScenarioRoute route1;
    route1.id = "ROUTE_002";
    route1.name = "North Patrol";
    route1.waypoints.append(QPointF(28.6000, 77.2000));
    route1.altitudes.append(3000.0);
    route1.waypoints.append(QPointF(28.7000, 77.3000));
    route1.altitudes.append(3000.0);
    route1.waypoints.append(QPointF(28.7000, 77.4000));
    route1.altitudes.append(3000.0);
    route1.additionalData["assignedTo"] = "UAV_001";
    scenario.routes.append(route1);
    
    // Add patrol route 2
    ScenarioRoute route2;
    route2.id = "ROUTE_003";
    route2.name = "South Patrol";
    route2.waypoints.append(QPointF(28.4000, 77.2000));
    route2.altitudes.append(3000.0);
    route2.waypoints.append(QPointF(28.3000, 77.3000));
    route2.altitudes.append(3000.0);
    route2.waypoints.append(QPointF(28.3000, 77.4000));
    route2.altitudes.append(3000.0);
    route2.additionalData["assignedTo"] = "UAV_002";
    scenario.routes.append(route2);
    
    scenario.metadata["author"] = "System";
    scenario.metadata["version"] = "1.0";
    scenario.metadata["difficulty"] = "medium";
    scenario.metadata["missionType"] = "surveillance";
    
    return scenario;
}

// Sample Scenario 3: Multi-Target Engagement
Scenario CScenarioManager::createSampleScenario3()
{
    Scenario scenario;
    scenario.name = "Multi-Target Engagement";
    scenario.description = "Complex scenario with multiple threats and defense assets";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // Add multiple radars
    ScenarioObject radar1;
    radar1.id = "RADAR_002";
    radar1.type = "RADAR";
    radar1.latitude = 28.5000;
    radar1.longitude = 77.2000;
    radar1.altitude = 100.0;
    radar1.additionalData["type"] = "long-range";
    scenario.objects.append(radar1);
    
    ScenarioObject radar2;
    radar2.id = "RADAR_003";
    radar2.type = "RADAR";
    radar2.latitude = 28.6000;
    radar2.longitude = 77.3000;
    radar2.altitude = 100.0;
    radar2.additionalData["type"] = "medium-range";
    scenario.objects.append(radar2);
    
    // Add multiple launchers
    ScenarioObject launcher1;
    launcher1.id = "LAUNCHER_002";
    launcher1.type = "LAUNCHER";
    launcher1.latitude = 28.5100;
    launcher1.longitude = 77.2100;
    launcher1.altitude = 50.0;
    launcher1.additionalData["missileCount"] = 12;
    scenario.objects.append(launcher1);
    
    ScenarioObject launcher2;
    launcher2.id = "LAUNCHER_003";
    launcher2.type = "LAUNCHER";
    launcher2.latitude = 28.6100;
    launcher2.longitude = 77.3100;
    launcher2.altitude = 50.0;
    launcher2.additionalData["missileCount"] = 10;
    scenario.objects.append(launcher2);
    
    // Add fighters
    ScenarioObject fighter1;
    fighter1.id = "FIGHTER_002";
    fighter1.type = "FIGHTER";
    fighter1.latitude = 28.4800;
    fighter1.longitude = 77.1800;
    fighter1.altitude = 8000.0;
    fighter1.additionalData["callsign"] = "VIPER-1";
    scenario.objects.append(fighter1);
    
    ScenarioObject fighter2;
    fighter2.id = "FIGHTER_003";
    fighter2.type = "FIGHTER";
    fighter2.latitude = 28.4900;
    fighter2.longitude = 77.1900;
    fighter2.altitude = 8000.0;
    fighter2.additionalData["callsign"] = "VIPER-2";
    scenario.objects.append(fighter2);
    
    // Add interceptor route
    ScenarioRoute interceptRoute;
    interceptRoute.id = "ROUTE_004";
    interceptRoute.name = "Intercept Vector";
    interceptRoute.waypoints.append(QPointF(28.4800, 77.1800));
    interceptRoute.altitudes.append(8000.0);
    interceptRoute.waypoints.append(QPointF(28.5500, 77.2500));
    interceptRoute.altitudes.append(10000.0);
    interceptRoute.waypoints.append(QPointF(28.6500, 77.3500));
    interceptRoute.altitudes.append(12000.0);
    interceptRoute.additionalData["type"] = "intercept";
    interceptRoute.additionalData["speed"] = 800;
    scenario.routes.append(interceptRoute);
    
    // Add CAP route
    ScenarioRoute capRoute;
    capRoute.id = "ROUTE_005";
    capRoute.name = "CAP Station Bravo";
    capRoute.waypoints.append(QPointF(28.5000, 77.2000));
    capRoute.altitudes.append(9000.0);
    capRoute.waypoints.append(QPointF(28.6000, 77.2000));
    capRoute.altitudes.append(9000.0);
    capRoute.waypoints.append(QPointF(28.6000, 77.3000));
    capRoute.altitudes.append(9000.0);
    capRoute.waypoints.append(QPointF(28.5000, 77.3000));
    capRoute.altitudes.append(9000.0);
    capRoute.additionalData["type"] = "CAP";
    scenario.routes.append(capRoute);
    
    scenario.metadata["author"] = "System";
    scenario.metadata["version"] = "1.0";
    scenario.metadata["difficulty"] = "hard";
    scenario.metadata["missionType"] = "defense";
    scenario.metadata["threatLevel"] = "high";
    
    return scenario;
}
