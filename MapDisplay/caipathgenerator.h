#ifndef CAIPATHGENERATOR_H
#define CAIPATHGENERATOR_H

#include <QObject>
#include <QList>
#include <QString>
#include <QPointF>
#include <QtMath>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "qgspointxyz.h"
#include "../globalConstants.h"

/**
 * @brief The CAIPathGenerator class generates AI-powered intelligent flight paths
 * 
 * This class implements AI-based path generation that considers:
 * - Mission objectives (patrol, strike, recon, evasion, etc.)
 * - Terrain awareness simulation
 * - Threat avoidance patterns
 * - Fuel/range optimization
 * - Time constraints
 * 
 * The AI generation can use either:
 * 1. Built-in heuristic AI (offline) - Uses intelligent algorithms
 * 2. External LLM API (online) - Can integrate with OpenAI/Claude APIs
 */
class CAIPathGenerator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief AI-specific path generation parameters
     */
    struct AIPathParameters {
        // Mission parameters
        eVISTAR_AI_MISSION_TYPE missionType = AI_MISSION_PATROL;
        QString missionDescription = "";      // Custom mission description
        
        // Path constraints
        double minAltitude = 500.0;           // Minimum altitude (m)
        double maxAltitude = 15000.0;         // Maximum altitude (m)
        double preferredAltitude = 5000.0;    // Preferred cruise altitude (m)
        double maxSpeed = 500.0;              // Max speed (km/h) - affects path smoothness
        double minTurnRadius = 1.0;           // Minimum turn radius (km)
        
        // Threat parameters
        bool avoidDetection = false;          // Enable detection avoidance
        double threatRadius = 50.0;           // Assumed threat detection radius (km)
        QList<QPointF> threatLocations;       // Known threat positions (lat, lon)
        
        // Terrain parameters (simulated)
        bool terrainFollowing = false;        // Enable terrain-following mode
        double terrainClearance = 100.0;      // Minimum terrain clearance (m)
        
        // Time constraints
        double maxFlightTime = 0.0;           // Maximum flight time (hours), 0 = unlimited
        
        // Generation quality
        int numWaypoints = 30;                // Target number of waypoints
        double pathRandomness = 0.3;          // Randomness factor (0.0-1.0)
        bool optimizeForFuel = false;         // Optimize path for fuel efficiency
        
        // AI API settings (for external AI)
        bool useExternalAI = false;           // Use external LLM API
        QString apiEndpoint = "";             // API endpoint URL
        QString apiKey = "";                  // API key (stored securely)
        
        AIPathParameters() = default;
    };

    explicit CAIPathGenerator(QObject *parent = nullptr);
    ~CAIPathGenerator() = default;

    /**
     * @brief Generate an AI-powered path between two points
     * @param startPoint Starting waypoint (lon, lat)
     * @param endPoint Ending waypoint (lon, lat)
     * @param params AI-specific parameters for path generation
     * @return List of waypoints forming the intelligent path
     */
    QList<QgsPointXYZ> generateAIPath(const QgsPointXY &startPoint,
                                       const QgsPointXY &endPoint,
                                       const AIPathParameters &params = AIPathParameters());

    /**
     * @brief Generate path asynchronously using external AI (for API-based generation)
     * Emits signalAIPathGenerated when complete
     */
    void generateAIPathAsync(const QgsPointXY &startPoint,
                             const QgsPointXY &endPoint,
                             const AIPathParameters &params = AIPathParameters());

    /**
     * @brief Get human-readable name for a mission type
     */
    static QString getMissionTypeName(eVISTAR_AI_MISSION_TYPE missionType);

    /**
     * @brief Get description for a mission type
     */
    static QString getMissionTypeDescription(eVISTAR_AI_MISSION_TYPE missionType);

    /**
     * @brief Set default AI parameters
     */
    void setDefaultParameters(const AIPathParameters &params);

    /**
     * @brief Get current default AI parameters
     */
    AIPathParameters getDefaultParameters() const;

    /**
     * @brief Check if external AI API is configured and available
     */
    bool isExternalAIAvailable() const;

    /**
     * @brief Configure external AI API
     */
    void configureExternalAI(const QString &endpoint, const QString &apiKey);

signals:
    /**
     * @brief Emitted when async AI path generation is complete
     */
    void signalAIPathGenerated(QList<QgsPointXYZ> path, bool success, QString errorMessage);

    /**
     * @brief Emitted during generation to report progress
     */
    void signalGenerationProgress(int percentComplete, QString statusMessage);

private:
    AIPathParameters _m_defaultParams;
    QNetworkAccessManager *_m_networkManager;
    QString _m_apiEndpoint;
    QString _m_apiKey;

    // AI Path generation algorithms (built-in heuristic AI)
    QList<QgsPointXYZ> generatePatrolPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateStrikePath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateReconPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateEvasionPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateInterceptPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateEscortPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> generateCustomPath(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);

    // Helper functions for AI path generation
    double calculateDistance(const QgsPointXY &p1, const QgsPointXY &p2) const;
    double calculateBearing(const QgsPointXY &from, const QgsPointXY &to) const;
    QgsPointXY movePoint(const QgsPointXY &point, double bearing, double distanceKm) const;
    QgsPointXY offsetPoint(const QgsPointXY &point, double dx, double dy) const;
    double randomDouble(double min, double max) const;
    bool isNearThreat(const QgsPointXY &point, const QList<QPointF> &threats, double radius) const;
    QgsPointXY avoidThreat(const QgsPointXY &current, const QgsPointXY &desired, 
                           const QList<QPointF> &threats, double radius) const;
    double smoothAltitude(double currentAlt, double targetAlt, double smoothFactor) const;
    
    // Bezier curve helpers for smooth paths
    QgsPointXY bezierPoint(const QgsPointXY &p0, const QgsPointXY &p1, 
                           const QgsPointXY &p2, const QgsPointXY &p3, double t) const;
    QList<QgsPointXYZ> smoothPath(const QList<QgsPointXYZ> &roughPath, int segments) const;

    // External AI integration
    void callExternalAI(const QgsPointXY &start, const QgsPointXY &end, const AIPathParameters &params);
    QList<QgsPointXYZ> parseAIResponse(const QJsonDocument &response) const;

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);
};

#endif // CAIPATHGENERATOR_H
