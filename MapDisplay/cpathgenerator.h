#ifndef CPATHGENERATOR_H
#define CPATHGENERATOR_H

#include <QList>
#include <QPointF>
#include <QtMath>
#include "qgspointxyz.h"
#include "../globalConstants.h"

/**
 * @brief The CPathGenerator class generates various trajectory paths between two waypoints.
 * 
 * This class implements physics-based and kinematic path generation algorithms
 * for creating smooth, realistic flight paths for drones, UAVs, and other aircraft.
 */
class CPathGenerator
{
public:
    /**
     * @brief Structure to hold path generation parameters
     */
    struct PathParameters {
        double defaultAltitude = 1000.0;      // Default altitude in meters
        int numWaypoints = 30;                // Number of intermediate waypoints (increased for more detail)
        double curveFactor = 0.6;             // Controls curve intensity (0.0-1.0) - increased for wider curves
        double spiralTurns = 3.5;             // Number of spiral rotations - increased for more coverage
        double zigzagAmplitude = 0.45;        // Amplitude of zigzag (fraction of distance) - increased
        int zigzagFrequency = 8;              // Number of zigzag oscillations - increased
        double maxTurnRadius = 0.1;           // Maximum turn radius in degrees - increased
        double randomVariance = 0.45;         // Random path variance factor - significantly increased
        double spreadRadiusKm = 15.0;         // Maximum trajectory spread radius in kilometers - tripled
        double randomnessLevel = 0.5;         // Global randomness multiplier (0.0-1.0) for all paths
        
        // Path generation method (AI vs Conventional)
        eVISTAR_PATH_METHOD generationMethod = PATH_METHOD_CONVENTIONAL;
        
        // AI-specific parameters (used when generationMethod = PATH_METHOD_AI)
        eVISTAR_AI_MISSION_TYPE aiMissionType = AI_MISSION_PATROL;
        bool aiAvoidDetection = false;        // Enable threat avoidance in AI mode
        bool aiTerrainFollowing = false;      // Enable terrain following in AI mode
        double aiMinAltitude = 500.0;         // Minimum altitude for AI paths
        double aiMaxAltitude = 15000.0;       // Maximum altitude for AI paths
        double aiThreatRadius = 50.0;         // Threat detection radius in km
        bool aiOptimizeForFuel = false;       // Optimize path for fuel efficiency
        
        PathParameters() = default;
    };

    CPathGenerator();
    ~CPathGenerator() = default;

    /**
     * @brief Generate a path between two points based on the specified path type
     * @param startPoint Starting waypoint (lon, lat)
     * @param endPoint Ending waypoint (lon, lat)
     * @param pathType Type of trajectory to generate
     * @param params Optional parameters for path generation
     * @return List of waypoints forming the path
     */
    QList<QgsPointXYZ> generatePath(const QgsPointXY &startPoint,
                                     const QgsPointXY &endPoint,
                                     eVISTAR_PATH_TYPE pathType,
                                     const PathParameters &params = PathParameters());

    /**
     * @brief Get human-readable name for a path type
     */
    static QString getPathTypeName(eVISTAR_PATH_TYPE pathType);

    /**
     * @brief Get description for a path type
     */
    static QString getPathTypeDescription(eVISTAR_PATH_TYPE pathType);

    /**
     * @brief Set default parameters
     */
    void setDefaultParameters(const PathParameters &params);

    /**
     * @brief Get current default parameters
     */
    PathParameters getDefaultParameters() const;

private:
    PathParameters _m_defaultParams;

    // Path generation algorithms
    QList<QgsPointXYZ> generateStraightPath(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateSCurve(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateLCurve(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateFigure8(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateSpiral(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateZigzag(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateBezier(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);
    QList<QgsPointXYZ> generateRandom(const QgsPointXY &start, const QgsPointXY &end, const PathParameters &params);

    // Helper functions
    double sigmoid(double x) const;
    double smoothStep(double x) const;
    QgsPointXY rotatePoint(const QgsPointXY &point, const QgsPointXY &center, double angleDegrees) const;
    QgsPointXY bezierPoint(const QgsPointXY &p0, const QgsPointXY &p1, 
                           const QgsPointXY &p2, const QgsPointXY &p3, double t) const;
    double randomDouble(double min, double max) const;
};

#endif // CPATHGENERATOR_H
