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
        // General parameters
        double defaultAltitude = 1000.0;      // Default altitude in meters
        int numWaypoints = 50;                // Number of intermediate waypoints (increased for smoother paths)
        double curveFactor = 0.6;             // Controls curve intensity (0.0-1.0)
        double spreadRadiusKm = 15.0;         // Maximum trajectory spread radius in kilometers
        
        // Spiral-specific parameters
        double spiralTurns = 4.0;             // Number of spiral rotations
        double spiralExpansionRate = 0.8;     // How fast the spiral expands (0.1-2.0)
        double spiralTightness = 0.5;         // How tight the spiral is (0.0=loose, 1.0=tight)
        bool spiralClockwise = true;          // Direction of spiral rotation
        
        // Zigzag parameters
        double zigzagAmplitude = 0.45;        // Amplitude of zigzag (fraction of distance)
        int zigzagFrequency = 8;              // Number of zigzag oscillations
        
        // Turn parameters
        double maxTurnRadius = 0.1;           // Maximum turn radius in degrees
        
        // Randomness parameters
        double randomVariance = 0.45;         // Random path variance factor
        double randomnessLevel = 0.5;         // Global randomness multiplier (0.0-1.0) for all paths
        double noiseScale = 0.3;              // Scale of Perlin-like noise added to paths (0.0-1.0)
        double altitudeVariation = 150.0;     // Max altitude variation in meters
        double wobbleIntensity = 0.4;         // Intensity of path wobble (0.0-1.0)
        
        // Advanced randomness
        double pathAsymmetry = 0.3;           // How asymmetric paths can be (0.0-1.0)
        double clusterTendency = 0.5;         // Tendency to cluster waypoints (0.0-1.0)
        int randomSeed = -1;                  // -1 for random seed, otherwise fixed seed
        
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
