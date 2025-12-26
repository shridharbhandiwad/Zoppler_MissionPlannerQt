#include "cpathgenerator.h"
#include <QRandomGenerator>
#include <cmath>

CPathGenerator::CPathGenerator()
{
    // Default parameters are already set in the struct definition
}

QString CPathGenerator::getPathTypeName(eVISTAR_PATH_TYPE pathType)
{
    switch (pathType) {
        case PATH_TYPE_STRAIGHT: return "Straight";
        case PATH_TYPE_S_CURVE:  return "S-Curve";
        case PATH_TYPE_L_CURVE:  return "L-Curve";
        case PATH_TYPE_FIGURE_8: return "Figure-8";
        case PATH_TYPE_SPIRAL:   return "Spiral";
        case PATH_TYPE_ZIGZAG:   return "Zigzag";
        case PATH_TYPE_BEZIER:   return "Bezier";
        case PATH_TYPE_RANDOM:   return "Random";
        default:                 return "Unknown";
    }
}

QString CPathGenerator::getPathTypeDescription(eVISTAR_PATH_TYPE pathType)
{
    switch (pathType) {
        case PATH_TYPE_STRAIGHT: 
            return "Direct line path - shortest distance between two points";
        case PATH_TYPE_S_CURVE:  
            return "Smooth S-shaped trajectory using sigmoid function - ideal for gradual heading changes";
        case PATH_TYPE_L_CURVE:  
            return "L-shaped path with rounded corner - useful for perpendicular approach maneuvers";
        case PATH_TYPE_FIGURE_8: 
            return "Figure-8 looping trajectory - excellent for surveillance patterns";
        case PATH_TYPE_SPIRAL:   
            return "Spiral path expanding outward - great for area coverage";
        case PATH_TYPE_ZIGZAG:   
            return "Zigzag evasive maneuver pattern - useful for tactical approaches";
        case PATH_TYPE_BEZIER:   
            return "Smooth Bezier curve - elegant curved trajectory";
        case PATH_TYPE_RANDOM:   
            return "Random waypoints - unpredictable path for testing";
        default:                 
            return "No path type selected";
    }
}

void CPathGenerator::setDefaultParameters(const PathParameters &params)
{
    _m_defaultParams = params;
}

CPathGenerator::PathParameters CPathGenerator::getDefaultParameters() const
{
    return _m_defaultParams;
}

QList<QgsPointXYZ> CPathGenerator::generatePath(const QgsPointXY &startPoint,
                                                 const QgsPointXY &endPoint,
                                                 eVISTAR_PATH_TYPE pathType,
                                                 const PathParameters &params)
{
    switch (pathType) {
        case PATH_TYPE_STRAIGHT:
            return generateStraightPath(startPoint, endPoint, params);
        case PATH_TYPE_S_CURVE:
            return generateSCurve(startPoint, endPoint, params);
        case PATH_TYPE_L_CURVE:
            return generateLCurve(startPoint, endPoint, params);
        case PATH_TYPE_FIGURE_8:
            return generateFigure8(startPoint, endPoint, params);
        case PATH_TYPE_SPIRAL:
            return generateSpiral(startPoint, endPoint, params);
        case PATH_TYPE_ZIGZAG:
            return generateZigzag(startPoint, endPoint, params);
        case PATH_TYPE_BEZIER:
            return generateBezier(startPoint, endPoint, params);
        case PATH_TYPE_RANDOM:
            return generateRandom(startPoint, endPoint, params);
        default:
            return generateStraightPath(startPoint, endPoint, params);
    }
}

// ============ Path Generation Algorithms ============

QList<QgsPointXYZ> CPathGenerator::generateStraightPath(const QgsPointXY &start, 
                                                         const QgsPointXY &end, 
                                                         const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        double x = start.x() + t * (end.x() - start.x());
        double y = start.y() + t * (end.y() - start.y());
        path.append(QgsPointXYZ(x, y, params.defaultAltitude));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateSCurve(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    // Calculate the perpendicular offset direction
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double length = std::sqrt(dx * dx + dy * dy);
    
    // Perpendicular unit vector (rotated 90 degrees)
    double perpX = -dy / length;
    double perpY = dx / length;
    
    // Maximum lateral offset for the S-curve
    double maxOffset = length * params.curveFactor;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Sigmoid-based S-curve offset
        // The sigmoid creates a smooth transition from one side to the other
        double sigmoidT = sigmoid((t - 0.5) * 10.0);  // Scaled sigmoid
        double offset = maxOffset * (2.0 * sigmoidT - 1.0);
        
        // Linear interpolation along the main axis
        double x = start.x() + t * dx + offset * perpX;
        double y = start.y() + t * dy + offset * perpY;
        
        // Add slight altitude variation for realism
        double altVariation = params.defaultAltitude + 50.0 * std::sin(M_PI * t);
        
        path.append(QgsPointXYZ(x, y, altVariation));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateLCurve(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    
    // Corner point is where the L bends (at the corner position based on curve factor)
    double cornerRatio = 0.5 + params.curveFactor * 0.3;
    QgsPointXY cornerPoint(start.x() + cornerRatio * dx, start.y());
    
    // Alternative corner for more interesting L-shape
    QgsPointXY altCorner(start.x() + cornerRatio * dx, start.y() + (1.0 - cornerRatio) * dy);
    
    int halfPoints = params.numWaypoints / 2;
    double turnRadius = params.maxTurnRadius;
    
    // First segment: start to corner approach
    for (int i = 0; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        double x = start.x() + smoothT * (altCorner.x() - start.x());
        double y = start.y() + smoothT * (altCorner.y() - start.y());
        
        path.append(QgsPointXYZ(x, y, params.defaultAltitude));
    }
    
    // Second segment: corner to end
    for (int i = 1; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        double x = altCorner.x() + smoothT * (end.x() - altCorner.x());
        double y = altCorner.y() + smoothT * (end.y() - altCorner.y());
        
        path.append(QgsPointXYZ(x, y, params.defaultAltitude));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateFigure8(const QgsPointXY &start, 
                                                    const QgsPointXY &end, 
                                                    const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    // Center of the figure-8 is between start and end
    double centerX = (start.x() + end.x()) / 2.0;
    double centerY = (start.y() + end.y()) / 2.0;
    
    // Distance and angle calculations
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    double angle = std::atan2(dy, dx);
    
    // Radius of each loop (fraction of distance)
    double loopRadius = distance * params.curveFactor * 0.4;
    
    // Number of complete figure-8 loops
    int numLoops = 2;
    int pointsPerLoop = params.numWaypoints / numLoops;
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    // Generate figure-8 pattern
    for (int loop = 0; loop < numLoops; loop++) {
        for (int i = 0; i < pointsPerLoop; i++) {
            double t = static_cast<double>(i) / pointsPerLoop + loop;
            double theta = t * 2.0 * M_PI;
            
            // Parametric equations for figure-8 (lemniscate of Gerono)
            double scale = loopRadius / (1.0 + std::pow(std::cos(theta), 2) * 0.5);
            double localX = scale * std::cos(theta);
            double localY = scale * std::sin(2.0 * theta) / 2.0;
            
            // Rotate and translate to align with start-end axis
            double x = centerX + localX * std::cos(angle) - localY * std::sin(angle);
            double y = centerY + localX * std::sin(angle) + localY * std::cos(angle);
            
            // Altitude variation for dynamic effect
            double alt = params.defaultAltitude + 100.0 * std::sin(theta);
            
            path.append(QgsPointXYZ(x, y, alt));
        }
    }
    
    // End point
    path.append(QgsPointXYZ(end.x(), end.y(), params.defaultAltitude));
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateSpiral(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    double angle = std::atan2(dy, dx);
    
    // Maximum spiral radius
    double maxRadius = distance * params.curveFactor * 0.3;
    
    // Number of spiral turns
    double turns = params.spiralTurns;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Progress along the main axis
        double mainX = start.x() + t * dx;
        double mainY = start.y() + t * dy;
        
        // Spiral offset (decreasing radius towards the end)
        double spiralProgress = turns * 2.0 * M_PI * t;
        double radius = maxRadius * (1.0 - t * 0.7);  // Radius decreases along path
        
        // Spiral coordinates
        double offsetX = radius * std::cos(spiralProgress);
        double offsetY = radius * std::sin(spiralProgress);
        
        // Rotate offset to align with main direction
        double rotatedOffsetX = offsetX * std::cos(angle + M_PI / 2) - offsetY * std::sin(angle + M_PI / 2);
        double rotatedOffsetY = offsetX * std::sin(angle + M_PI / 2) + offsetY * std::cos(angle + M_PI / 2);
        
        double x = mainX + rotatedOffsetX;
        double y = mainY + rotatedOffsetY;
        
        // Altitude varies with spiral
        double alt = params.defaultAltitude + 50.0 * std::sin(spiralProgress);
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateZigzag(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Perpendicular direction
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Zigzag amplitude
    double amplitude = distance * params.zigzagAmplitude;
    int frequency = params.zigzagFrequency;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Zigzag offset (triangle wave for sharp zigzag)
        double phase = t * frequency;
        double triangleWave = 2.0 * std::abs(phase - std::floor(phase + 0.5));
        double offset = amplitude * (triangleWave * 2.0 - 1.0);
        
        // Smooth the zigzag slightly at the edges
        double edgeFade = 4.0 * t * (1.0 - t);  // Parabola: 0 at edges, 1 at center
        offset *= edgeFade;
        
        double x = start.x() + t * dx + offset * perpX;
        double y = start.y() + t * dy + offset * perpY;
        
        path.append(QgsPointXYZ(x, y, params.defaultAltitude));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateBezier(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Control point offset
    double cpOffset = distance * params.curveFactor;
    
    // Perpendicular direction
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Control points for cubic Bezier
    QgsPointXY p0 = start;
    QgsPointXY p1(start.x() + 0.33 * dx + cpOffset * perpX, 
                  start.y() + 0.33 * dy + cpOffset * perpY);
    QgsPointXY p2(start.x() + 0.67 * dx - cpOffset * perpX,
                  start.y() + 0.67 * dy - cpOffset * perpY);
    QgsPointXY p3 = end;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        QgsPointXY pt = bezierPoint(p0, p1, p2, p3, t);
        
        // Altitude variation based on curve position
        double altVariation = params.defaultAltitude + 80.0 * std::sin(M_PI * t);
        
        path.append(QgsPointXYZ(pt.x(), pt.y(), altVariation));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateRandom(const QgsPointXY &start, 
                                                   const QgsPointXY &end, 
                                                   const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Maximum random offset
    double maxOffset = distance * params.randomVariance;
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    // Random intermediate waypoints
    for (int i = 1; i < params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Base position along the line
        double baseX = start.x() + t * dx;
        double baseY = start.y() + t * dy;
        
        // Random offset (perpendicular to the main direction)
        double randomOffsetX = randomDouble(-maxOffset, maxOffset);
        double randomOffsetY = randomDouble(-maxOffset, maxOffset);
        
        // Reduce offset near start and end for smoother transition
        double edgeFade = 4.0 * t * (1.0 - t);
        randomOffsetX *= edgeFade;
        randomOffsetY *= edgeFade;
        
        double x = baseX + randomOffsetX;
        double y = baseY + randomOffsetY;
        
        // Random altitude variation
        double altVariation = params.defaultAltitude + randomDouble(-100, 100) * edgeFade;
        
        path.append(QgsPointXYZ(x, y, altVariation));
    }
    
    // End point
    path.append(QgsPointXYZ(end.x(), end.y(), params.defaultAltitude));
    
    return path;
}

// ============ Helper Functions ============

double CPathGenerator::sigmoid(double x) const
{
    return 1.0 / (1.0 + std::exp(-x));
}

double CPathGenerator::smoothStep(double x) const
{
    // Hermite interpolation for smooth acceleration/deceleration
    x = std::max(0.0, std::min(1.0, x));
    return x * x * (3.0 - 2.0 * x);
}

QgsPointXY CPathGenerator::rotatePoint(const QgsPointXY &point, 
                                        const QgsPointXY &center, 
                                        double angleDegrees) const
{
    double angleRad = angleDegrees * M_PI / 180.0;
    double cosA = std::cos(angleRad);
    double sinA = std::sin(angleRad);
    
    double dx = point.x() - center.x();
    double dy = point.y() - center.y();
    
    double newX = center.x() + dx * cosA - dy * sinA;
    double newY = center.y() + dx * sinA + dy * cosA;
    
    return QgsPointXY(newX, newY);
}

QgsPointXY CPathGenerator::bezierPoint(const QgsPointXY &p0, const QgsPointXY &p1,
                                        const QgsPointXY &p2, const QgsPointXY &p3, 
                                        double t) const
{
    // Cubic Bezier curve formula
    double u = 1.0 - t;
    double tt = t * t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * t;
    
    double x = uuu * p0.x() + 3 * uu * t * p1.x() + 3 * u * tt * p2.x() + ttt * p3.x();
    double y = uuu * p0.y() + 3 * uu * t * p1.y() + 3 * u * tt * p2.y() + ttt * p3.y();
    
    return QgsPointXY(x, y);
}

double CPathGenerator::randomDouble(double min, double max) const
{
    return QRandomGenerator::global()->bounded(max - min) + min;
}
