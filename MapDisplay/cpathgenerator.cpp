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
    
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Perpendicular direction for random offset
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    double maxOffset = spreadRadiusDeg * params.randomnessLevel * 0.3;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        double baseX = start.x() + t * dx;
        double baseY = start.y() + t * dy;
        
        // Add random wobble even to "straight" path - slight variations for realism
        double edgeFade = std::sin(M_PI * t);  // Smooth fade at edges
        double randomOffset = randomDouble(-maxOffset, maxOffset) * edgeFade;
        
        double x = baseX + randomOffset * perpX;
        double y = baseY + randomOffset * perpY;
        double alt = params.defaultAltitude + randomDouble(-30, 30) * edgeFade;
        
        path.append(QgsPointXYZ(x, y, alt));
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
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Maximum lateral offset for the S-curve - increased spread
    double maxOffset = std::max(length * params.curveFactor * 1.5, spreadRadiusDeg * 0.8);
    
    // Random variation parameters for this path
    double randomPhaseShift = randomDouble(0, M_PI);  // Random starting phase
    double randomAmplitude = randomDouble(0.6, 1.4);  // Random amplitude multiplier
    double randomFrequency = randomDouble(0.8, 1.3);  // Random frequency variation
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Sigmoid-based S-curve offset with random variations
        double sigmoidT = sigmoid((t - 0.5) * 10.0 * randomFrequency);
        double baseOffset = maxOffset * randomAmplitude * (2.0 * sigmoidT - 1.0);
        
        // Add random wobble to the curve
        double wobbleAmplitude = maxOffset * params.randomnessLevel * 0.4;
        double wobble = wobbleAmplitude * std::sin(t * 6.0 * M_PI + randomPhaseShift);
        wobble += randomDouble(-wobbleAmplitude * 0.5, wobbleAmplitude * 0.5);
        
        double offset = baseOffset + wobble;
        
        // Linear interpolation along the main axis with random forward/back variation
        double forwardOffset = randomDouble(-length * 0.02, length * 0.02) * params.randomnessLevel;
        double x = start.x() + t * dx + offset * perpX + forwardOffset * (dx / length);
        double y = start.y() + t * dy + offset * perpY + forwardOffset * (dy / length);
        
        // Add altitude variation with randomness
        double altVariation = params.defaultAltitude + 100.0 * std::sin(M_PI * t) + randomDouble(-50, 50);
        
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
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Convert spread radius from km to degrees
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Randomize corner position for more variety
    double cornerRatio = 0.3 + randomDouble(0.0, 0.5) * params.curveFactor;
    
    // Random perpendicular offset for the corner point
    double perpX = -dy / distance;
    double perpY = dx / distance;
    double cornerOffset = randomDouble(-spreadRadiusDeg * 0.6, spreadRadiusDeg * 0.6) * params.randomnessLevel;
    
    // Create multiple corner variations
    double cornerX = start.x() + cornerRatio * dx + cornerOffset * perpX;
    double cornerY = start.y() + (1.0 - cornerRatio) * dy + cornerOffset * perpY;
    
    // Optionally add an intermediate waypoint for more complex L-shape
    bool addIntermediatePoint = randomDouble(0, 1) > 0.4;
    double midRatio = randomDouble(0.3, 0.7);
    double midOffset = randomDouble(-spreadRadiusDeg * 0.4, spreadRadiusDeg * 0.4);
    
    QgsPointXY altCorner(cornerX, cornerY);
    
    int halfPoints = params.numWaypoints / 2;
    double wobbleAmp = spreadRadiusDeg * params.randomnessLevel * 0.25;
    
    // First segment: start to corner approach with random wobble
    for (int i = 0; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        // Add random wobble
        double wobble = randomDouble(-wobbleAmp, wobbleAmp) * std::sin(M_PI * t);
        
        double x = start.x() + smoothT * (altCorner.x() - start.x()) + wobble * perpX;
        double y = start.y() + smoothT * (altCorner.y() - start.y()) + wobble * perpY;
        
        double alt = params.defaultAltitude + randomDouble(-40, 40) * std::sin(M_PI * t);
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    // Second segment: corner to end with random wobble
    for (int i = 1; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        // Add random wobble
        double wobble = randomDouble(-wobbleAmp, wobbleAmp) * std::sin(M_PI * t);
        
        double x = altCorner.x() + smoothT * (end.x() - altCorner.x()) + wobble * perpX;
        double y = altCorner.y() + smoothT * (end.y() - altCorner.y()) + wobble * perpY;
        
        double alt = params.defaultAltitude + randomDouble(-40, 40) * std::sin(M_PI * t);
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    return path;
}

QList<QgsPointXYZ> CPathGenerator::generateFigure8(const QgsPointXY &start, 
                                                    const QgsPointXY &end, 
                                                    const PathParameters &params)
{
    QList<QgsPointXYZ> path;
    
    // Center of the figure-8 - add random offset
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Random center offset for variety
    double centerOffsetX = randomDouble(-spreadRadiusDeg * 0.3, spreadRadiusDeg * 0.3) * params.randomnessLevel;
    double centerOffsetY = randomDouble(-spreadRadiusDeg * 0.3, spreadRadiusDeg * 0.3) * params.randomnessLevel;
    double centerX = (start.x() + end.x()) / 2.0 + centerOffsetX;
    double centerY = (start.y() + end.y()) / 2.0 + centerOffsetY;
    
    double angle = std::atan2(dy, dx);
    // Randomize angle slightly
    angle += randomDouble(-0.3, 0.3) * params.randomnessLevel;
    
    // Radius of each loop - significantly larger
    double loopRadius = std::max(distance * params.curveFactor * 0.8, spreadRadiusDeg * 0.7);
    
    // Randomize loop size asymmetry (one loop bigger than other)
    double loopAsymmetry = randomDouble(0.7, 1.3);
    
    // Number of complete figure-8 loops with variation
    int numLoops = 2 + (randomDouble(0, 1) > 0.6 ? 1 : 0);  // Sometimes 3 loops
    int pointsPerLoop = params.numWaypoints / numLoops;
    
    // Random eccentricity for deformed figure-8
    double eccentricityX = randomDouble(0.6, 1.4);
    double eccentricityY = randomDouble(0.6, 1.4);
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    // Generate figure-8 pattern with randomness
    for (int loop = 0; loop < numLoops; loop++) {
        double currentLoopRadius = loopRadius * (loop % 2 == 0 ? loopAsymmetry : 1.0 / loopAsymmetry);
        
        for (int i = 0; i < pointsPerLoop; i++) {
            double t = static_cast<double>(i) / pointsPerLoop + loop;
            double theta = t * 2.0 * M_PI;
            
            // Random frequency variation
            double freqVariation = 1.0 + randomDouble(-0.1, 0.1) * params.randomnessLevel;
            
            // Parametric equations for figure-8 with eccentricity
            double scale = currentLoopRadius / (1.0 + std::pow(std::cos(theta * freqVariation), 2) * 0.5);
            double localX = scale * eccentricityX * std::cos(theta);
            double localY = scale * eccentricityY * std::sin(2.0 * theta) / 2.0;
            
            // Add random wobble
            double wobble = spreadRadiusDeg * params.randomnessLevel * 0.2;
            localX += randomDouble(-wobble, wobble);
            localY += randomDouble(-wobble, wobble);
            
            // Rotate and translate to align with start-end axis
            double x = centerX + localX * std::cos(angle) - localY * std::sin(angle);
            double y = centerY + localX * std::sin(angle) + localY * std::cos(angle);
            
            // Altitude variation with randomness
            double alt = params.defaultAltitude + 150.0 * std::sin(theta) + randomDouble(-60, 60);
            
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
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Maximum spiral radius - significantly increased
    double maxRadius = std::max(distance * params.curveFactor * 0.7, spreadRadiusDeg * 0.8);
    
    // Randomize spiral parameters
    double turns = params.spiralTurns + randomDouble(-1.0, 1.5) * params.randomnessLevel;
    double radiusGrowthRate = randomDouble(0.3, 0.9);  // How quickly radius changes
    bool expandingSpiral = randomDouble(0, 1) > 0.5;   // Expanding vs contracting
    
    // Random eccentricity makes spiral elliptical
    double eccentricityX = randomDouble(0.7, 1.3);
    double eccentricityY = randomDouble(0.7, 1.3);
    
    // Random phase offset
    double phaseOffset = randomDouble(0, 2.0 * M_PI);
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Progress along the main axis with random variation
        double progressVariation = randomDouble(-0.03, 0.03) * params.randomnessLevel;
        double mainX = start.x() + (t + progressVariation) * dx;
        double mainY = start.y() + (t + progressVariation) * dy;
        
        // Spiral offset with random radius variation
        double spiralProgress = turns * 2.0 * M_PI * t + phaseOffset;
        double baseRadius = expandingSpiral ? 
            maxRadius * t * radiusGrowthRate + maxRadius * 0.3 :
            maxRadius * (1.0 - t * radiusGrowthRate);
        
        // Add random radius wobble
        double radiusWobble = randomDouble(-maxRadius * 0.3, maxRadius * 0.3) * params.randomnessLevel;
        double radius = baseRadius + radiusWobble;
        
        // Elliptical spiral coordinates
        double offsetX = radius * eccentricityX * std::cos(spiralProgress);
        double offsetY = radius * eccentricityY * std::sin(spiralProgress);
        
        // Add noise for organic feel
        double noise = spreadRadiusDeg * params.randomnessLevel * 0.15;
        offsetX += randomDouble(-noise, noise);
        offsetY += randomDouble(-noise, noise);
        
        // Rotate offset to align with main direction
        double rotatedOffsetX = offsetX * std::cos(angle + M_PI / 2) - offsetY * std::sin(angle + M_PI / 2);
        double rotatedOffsetY = offsetX * std::sin(angle + M_PI / 2) + offsetY * std::cos(angle + M_PI / 2);
        
        double x = mainX + rotatedOffsetX;
        double y = mainY + rotatedOffsetY;
        
        // Altitude varies with spiral plus randomness
        double alt = params.defaultAltitude + 100.0 * std::sin(spiralProgress) + randomDouble(-50, 50);
        
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
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Zigzag amplitude - significantly increased
    double baseAmplitude = std::max(distance * params.zigzagAmplitude * 1.5, spreadRadiusDeg * 0.7);
    
    // Randomize frequency and amplitude
    double frequency = params.zigzagFrequency + randomDouble(-2, 3) * params.randomnessLevel;
    double amplitudeVariation = randomDouble(0.7, 1.4);
    double amplitude = baseAmplitude * amplitudeVariation;
    
    // Random phase offset
    double phaseOffset = randomDouble(0, 1.0);
    
    // Choose zigzag style randomly
    bool smoothZigzag = randomDouble(0, 1) > 0.5;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Variable amplitude per zigzag peak
        double localAmplitude = amplitude * (1.0 + randomDouble(-0.4, 0.4) * params.randomnessLevel);
        
        // Zigzag offset with style variation
        double phase = t * frequency + phaseOffset;
        double waveValue;
        
        if (smoothZigzag) {
            // Smoother sine-based zigzag
            waveValue = std::sin(phase * 2.0 * M_PI);
        } else {
            // Sharp triangle wave zigzag
            double triangleWave = 2.0 * std::abs(phase - std::floor(phase + 0.5));
            waveValue = triangleWave * 2.0 - 1.0;
        }
        
        double offset = localAmplitude * waveValue;
        
        // Reduce edge fade for wider coverage (less tapering at edges)
        double edgeFade = std::pow(4.0 * t * (1.0 - t), 0.5);  // Softer parabola - more spread at edges
        offset *= std::max(edgeFade, 0.4);  // Minimum 40% amplitude even at edges
        
        // Add random wobble perpendicular to zigzag
        double wobble = spreadRadiusDeg * params.randomnessLevel * 0.2 * randomDouble(-1, 1);
        
        // Forward-backward random variation
        double forwardOffset = randomDouble(-distance * 0.02, distance * 0.02) * params.randomnessLevel;
        
        double x = start.x() + t * dx + offset * perpX + wobble * (dx / distance) + forwardOffset * (dx / distance);
        double y = start.y() + t * dy + offset * perpY + wobble * (dy / distance) + forwardOffset * (dy / distance);
        
        // Random altitude variation
        double alt = params.defaultAltitude + 80.0 * waveValue + randomDouble(-40, 40);
        
        path.append(QgsPointXYZ(x, y, alt));
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
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Control point offset - significantly increased
    double cpOffset = std::max(distance * params.curveFactor * 1.5, spreadRadiusDeg * 0.8);
    
    // Perpendicular direction
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Randomize control point positions for variety
    double cp1Ratio = 0.25 + randomDouble(-0.15, 0.15) * params.randomnessLevel;
    double cp2Ratio = 0.75 + randomDouble(-0.15, 0.15) * params.randomnessLevel;
    
    // Random perpendicular offset for each control point
    double cp1Offset = cpOffset * randomDouble(0.5, 1.5) * (randomDouble(0, 1) > 0.5 ? 1.0 : -1.0);
    double cp2Offset = cpOffset * randomDouble(0.5, 1.5) * (randomDouble(0, 1) > 0.5 ? 1.0 : -1.0);
    
    // Add some forward/backward randomness to control points
    double cp1Forward = randomDouble(-0.1, 0.1) * distance * params.randomnessLevel;
    double cp2Forward = randomDouble(-0.1, 0.1) * distance * params.randomnessLevel;
    
    // Control points for cubic Bezier with randomization
    QgsPointXY p0 = start;
    QgsPointXY p1(start.x() + cp1Ratio * dx + cp1Offset * perpX + cp1Forward * (dx / distance), 
                  start.y() + cp1Ratio * dy + cp1Offset * perpY + cp1Forward * (dy / distance));
    QgsPointXY p2(start.x() + cp2Ratio * dx + cp2Offset * perpX + cp2Forward * (dx / distance),
                  start.y() + cp2Ratio * dy + cp2Offset * perpY + cp2Forward * (dy / distance));
    QgsPointXY p3 = end;
    
    // Random wobble amplitude for path variation
    double wobbleAmp = spreadRadiusDeg * params.randomnessLevel * 0.2;
    double wobbleFreq = randomDouble(2.0, 5.0);
    double wobblePhase = randomDouble(0, 2.0 * M_PI);
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        QgsPointXY pt = bezierPoint(p0, p1, p2, p3, t);
        
        // Add sinusoidal wobble along the path
        double wobble = wobbleAmp * std::sin(t * wobbleFreq * M_PI + wobblePhase);
        wobble += randomDouble(-wobbleAmp * 0.5, wobbleAmp * 0.5);
        
        // Apply wobble perpendicular to path direction
        double x = pt.x() + wobble * perpX;
        double y = pt.y() + wobble * perpY;
        
        // Altitude variation with randomness
        double altVariation = params.defaultAltitude + 120.0 * std::sin(M_PI * t) + randomDouble(-50, 50);
        
        path.append(QgsPointXYZ(x, y, altVariation));
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
    
    // Perpendicular direction
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Maximum random offset - significantly increased for wide area coverage
    double maxOffset = std::max(distance * params.randomVariance * 2.0, spreadRadiusDeg);
    
    // Determine randomness style
    int randomStyle = static_cast<int>(randomDouble(0, 3));  // 0: pure random, 1: random walk, 2: clustered random
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    double prevOffsetX = 0, prevOffsetY = 0;  // For random walk style
    
    // Random intermediate waypoints
    for (int i = 1; i < params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Base position along the line with random forward/back variation
        double forwardVariation = randomDouble(-0.1, 0.1) * params.randomnessLevel * distance;
        double baseX = start.x() + t * dx + forwardVariation * (dx / distance);
        double baseY = start.y() + t * dy + forwardVariation * (dy / distance);
        
        double randomOffsetX, randomOffsetY;
        
        switch (randomStyle) {
            case 0:  // Pure random - full area coverage
                randomOffsetX = randomDouble(-maxOffset, maxOffset);
                randomOffsetY = randomDouble(-maxOffset, maxOffset);
                break;
                
            case 1:  // Random walk - more connected movements
                {
                    double walkStep = maxOffset * 0.4;
                    prevOffsetX += randomDouble(-walkStep, walkStep);
                    prevOffsetY += randomDouble(-walkStep, walkStep);
                    // Clamp to max offset
                    prevOffsetX = std::max(-maxOffset, std::min(maxOffset, prevOffsetX));
                    prevOffsetY = std::max(-maxOffset, std::min(maxOffset, prevOffsetY));
                    randomOffsetX = prevOffsetX;
                    randomOffsetY = prevOffsetY;
                }
                break;
                
            case 2:  // Clustered random - random but with occasional clusters
            default:
                {
                    // Decide if this point starts a new cluster
                    bool newCluster = (i % 5 == 1) || randomDouble(0, 1) > 0.7;
                    if (newCluster) {
                        prevOffsetX = randomDouble(-maxOffset, maxOffset);
                        prevOffsetY = randomDouble(-maxOffset, maxOffset);
                    }
                    // Add small variation around cluster center
                    double clusterVariation = maxOffset * 0.3;
                    randomOffsetX = prevOffsetX + randomDouble(-clusterVariation, clusterVariation);
                    randomOffsetY = prevOffsetY + randomDouble(-clusterVariation, clusterVariation);
                }
                break;
        }
        
        // Softer edge fade - maintain more randomness near endpoints
        double edgeFade = std::pow(4.0 * t * (1.0 - t), 0.4);  // Gentler fade
        edgeFade = std::max(edgeFade, 0.3);  // Minimum 30% even at edges
        
        randomOffsetX *= edgeFade;
        randomOffsetY *= edgeFade;
        
        double x = baseX + randomOffsetX;
        double y = baseY + randomOffsetY;
        
        // Wide altitude variation
        double altVariation = params.defaultAltitude + randomDouble(-200, 200) * edgeFade;
        
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
