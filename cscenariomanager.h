#ifndef CSCENARIOMANAGER_H
#define CSCENARIOMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>
#include <QMap>

// Structure to hold object data
struct ScenarioObject {
    QString id;
    QString type;
    double latitude;
    double longitude;
    double altitude;
    QJsonObject additionalData;
};

// Structure to hold route data
struct ScenarioRoute {
    QString id;
    QString name;
    QList<QPointF> waypoints; // lat, lon pairs
    QList<double> altitudes;
    QJsonObject additionalData;
};

// Structure to hold complete scenario
struct Scenario {
    QString name;
    QString description;
    QString createdDate;
    QList<ScenarioObject> objects;
    QList<ScenarioRoute> routes;
    QJsonObject metadata;
};

class CScenarioManager : public QObject
{
    Q_OBJECT

public:
    explicit CScenarioManager(QObject *parent = nullptr);
    ~CScenarioManager();

    // Save/Load scenarios
    bool saveScenario(const Scenario &scenario, const QString &filePath);
    bool loadScenario(const QString &filePath, Scenario &scenario);
    
    // Get list of available scenarios from directory
    QStringList getAvailableScenarios(const QString &directory = "");
    
    // Convert between Scenario struct and JSON
    QJsonObject scenarioToJson(const Scenario &scenario);
    Scenario jsonToScenario(const QJsonObject &json);
    
    // Create sample scenarios
    static Scenario createSampleScenario1(); // Basic defense scenario
    static Scenario createSampleScenario2(); // Air patrol scenario
    static Scenario createSampleScenario3(); // Multi-target scenario
    
    // Get default scenarios directory
    QString getDefaultScenariosDirectory() const;
    
    // Create default scenarios directory if it doesn't exist
    bool ensureScenariosDirectoryExists();

signals:
    void scenarioLoaded(const Scenario &scenario);
    void scenarioSaved(const QString &filePath);
    void error(const QString &message);

private:
    QString _m_scenariosDirectory;
    
    // Helper functions
    QJsonObject objectToJson(const ScenarioObject &obj);
    ScenarioObject jsonToObject(const QJsonObject &json);
    QJsonObject routeToJson(const ScenarioRoute &route);
    ScenarioRoute jsonToRoute(const QJsonObject &json);
};

#endif // CSCENARIOMANAGER_H
