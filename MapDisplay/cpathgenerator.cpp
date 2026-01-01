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
    double maxOffset = spreadRadiusDeg * params.randomnessLevel * 0.4;
    
    // Wobble parameters with randomization
    double wobbleFreq = 2.0 + randomDouble(0, 4.0) * params.wobbleIntensity;
    double wobblePhase = randomDouble(0, 2.0 * M_PI);
    double wobbleAmp = maxOffset * params.wobbleIntensity;
    
    // Asymmetry factor - makes one side more curved than other
    double asymmetryFactor = randomDouble(-1.0, 1.0) * params.pathAsymmetry;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        double baseX = start.x() + t * dx;
        double baseY = start.y() + t * dy;
        
        // Smooth edge fade with asymmetric option
        double edgeFade = std::sin(M_PI * t);
        double asymmetricFade = edgeFade * (1.0 + asymmetryFactor * (t - 0.5));
        
        // Combine wobble with random offset
        double wobble = wobbleAmp * std::sin(t * wobbleFreq * M_PI + wobblePhase) * asymmetricFade;
        double noise = randomDouble(-maxOffset * params.noiseScale, maxOffset * params.noiseScale) * asymmetricFade;
        double randomOffset = wobble + noise;
        
        // Forward/backward variation along the path
        double forwardNoise = randomDouble(-0.02, 0.02) * params.randomnessLevel * distance;
        
        double x = baseX + randomOffset * perpX + forwardNoise * (dx / distance);
        double y = baseY + randomOffset * perpY + forwardNoise * (dy / distance);
        
        // Altitude variation with parameters
        double altBase = params.defaultAltitude;
        double altWobble = params.altitudeVariation * 0.3 * std::sin(M_PI * t);
        double altNoise = randomDouble(-params.altitudeVariation * 0.2, params.altitudeVariation * 0.2) * params.randomnessLevel * asymmetricFade;
        double alt = altBase + altWobble + altNoise;
        
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
    
    // Random variation parameters for this path - significantly more variation
    double randomPhaseShift = randomDouble(0, 2.0 * M_PI) * params.randomnessLevel;
    double randomAmplitude = randomDouble(0.5, 1.5) * (0.7 + params.randomnessLevel * 0.6);
    double randomFrequency = randomDouble(0.6, 1.5);
    
    // Asymmetry: make one half of the S larger than the other
    double asymmetry = randomDouble(-0.4, 0.4) * params.pathAsymmetry;
    
    // Sigmoid steepness variation
    double sigmoidSteepness = 8.0 + randomDouble(-3.0, 5.0) * params.randomnessLevel;
    
    // Wobble parameters
    double wobbleFreq = 4.0 + randomDouble(0, 5.0) * params.wobbleIntensity;
    double wobblePhase = randomDouble(0, 2.0 * M_PI);
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Sigmoid-based S-curve offset with random variations
        double sigmoidInput = (t - 0.5) * sigmoidSteepness * randomFrequency;
        double sigmoidT = sigmoid(sigmoidInput);
        
        // Apply asymmetry - different amplitude for each half
        double amplitudeMod = 1.0 + asymmetry * (t < 0.5 ? 1.0 : -1.0);
        double baseOffset = maxOffset * randomAmplitude * amplitudeMod * (2.0 * sigmoidT - 1.0);
        
        // Multiple wobble frequencies for more organic feel
        double wobbleAmplitude = maxOffset * params.wobbleIntensity * 0.5;
        double wobble1 = wobbleAmplitude * std::sin(t * wobbleFreq * M_PI + wobblePhase);
        double wobble2 = wobbleAmplitude * 0.5 * std::sin(t * wobbleFreq * 2.3 * M_PI + randomPhaseShift);
        double wobble = wobble1 + wobble2;
        
        // Add noise
        double noise = randomDouble(-wobbleAmplitude * 0.6, wobbleAmplitude * 0.6) * params.noiseScale;
        
        double offset = baseOffset + wobble + noise;
        
        // Forward/back variation with more intensity
        double forwardOffset = randomDouble(-length * 0.03, length * 0.03) * params.randomnessLevel;
        
        double x = start.x() + t * dx + offset * perpX + forwardOffset * (dx / length);
        double y = start.y() + t * dy + offset * perpY + forwardOffset * (dy / length);
        
        // Altitude variation with parameters
        double altBase = params.defaultAltitude;
        double altCurve = params.altitudeVariation * 0.6 * std::sin(M_PI * t);
        double altNoise = randomDouble(-params.altitudeVariation * 0.3, params.altitudeVariation * 0.3) * params.randomnessLevel;
        double alt = altBase + altCurve + altNoise;
        
        path.append(QgsPointXYZ(x, y, alt));
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
    
    // Perpendicular direction
    double perpX = -dy / distance;
    double perpY = dx / distance;
    
    // Randomize corner position for more variety - wider range
    double cornerRatio = 0.2 + randomDouble(0.0, 0.6) * (0.5 + params.pathAsymmetry * 0.5);
    
    // Random perpendicular offset for the corner point - larger range
    double cornerOffset = randomDouble(-spreadRadiusDeg * 0.8, spreadRadiusDeg * 0.8) * params.randomnessLevel;
    
    // L-curve can be "inverted" randomly
    bool invertL = randomDouble(0, 1) < params.randomnessLevel * 0.4;
    if (invertL) {
        cornerRatio = 1.0 - cornerRatio;
    }
    
    // Create corner with randomization
    double cornerX = start.x() + cornerRatio * dx + cornerOffset * perpX;
    double cornerY = start.y() + (invertL ? cornerRatio : (1.0 - cornerRatio)) * dy + cornerOffset * perpY;
    
    // Add extra intermediate point(s) for more complex L-shape
    bool addIntermediatePoint = randomDouble(0, 1) > (0.5 - params.clusterTendency * 0.3);
    
    QgsPointXY altCorner(cornerX, cornerY);
    
    int halfPoints = params.numWaypoints / 2;
    double wobbleAmp = spreadRadiusDeg * params.wobbleIntensity * 0.35;
    
    // Wobble parameters
    double wobbleFreq1 = 2.0 + randomDouble(0, 3.0) * params.randomnessLevel;
    double wobblePhase1 = randomDouble(0, 2.0 * M_PI);
    double wobbleFreq2 = 3.0 + randomDouble(0, 4.0);
    double wobblePhase2 = randomDouble(0, 2.0 * M_PI);
    
    // First segment: start to corner approach with random wobble
    for (int i = 0; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        // Multi-frequency wobble for organic feel
        double wobble1 = wobbleAmp * std::sin(t * wobbleFreq1 * M_PI + wobblePhase1);
        double wobble2 = wobbleAmp * 0.5 * std::sin(t * wobbleFreq2 * M_PI + wobblePhase2);
        double noise = randomDouble(-wobbleAmp * 0.4, wobbleAmp * 0.4) * params.noiseScale;
        double wobble = (wobble1 + wobble2 + noise) * std::sin(M_PI * t);  // Edge fade
        
        double x = start.x() + smoothT * (altCorner.x() - start.x()) + wobble * perpX;
        double y = start.y() + smoothT * (altCorner.y() - start.y()) + wobble * perpY;
        
        // Altitude with variation
        double altBase = params.defaultAltitude;
        double altCurve = params.altitudeVariation * 0.4 * std::sin(M_PI * t);
        double altNoise = randomDouble(-params.altitudeVariation * 0.25, params.altitudeVariation * 0.25) * params.randomnessLevel;
        double alt = altBase + altCurve + altNoise;
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    // Second segment: corner to end with random wobble
    for (int i = 1; i <= halfPoints; i++) {
        double t = static_cast<double>(i) / halfPoints;
        double smoothT = smoothStep(t);
        
        // Different wobble for second segment
        double wobble1 = wobbleAmp * std::sin(t * wobbleFreq1 * 1.3 * M_PI + wobblePhase1 + M_PI);
        double wobble2 = wobbleAmp * 0.5 * std::sin(t * wobbleFreq2 * 1.2 * M_PI + wobblePhase2);
        double noise = randomDouble(-wobbleAmp * 0.4, wobbleAmp * 0.4) * params.noiseScale;
        double wobble = (wobble1 + wobble2 + noise) * std::sin(M_PI * t);
        
        double x = altCorner.x() + smoothT * (end.x() - altCorner.x()) + wobble * perpX;
        double y = altCorner.y() + smoothT * (end.y() - altCorner.y()) + wobble * perpY;
        
        double altBase = params.defaultAltitude;
        double altCurve = params.altitudeVariation * 0.4 * std::sin(M_PI * t);
        double altNoise = randomDouble(-params.altitudeVariation * 0.25, params.altitudeVariation * 0.25) * params.randomnessLevel;
        double alt = altBase + altCurve + altNoise;
        
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
    
    // Random center offset for variety - larger range
    double centerOffsetX = randomDouble(-spreadRadiusDeg * 0.5, spreadRadiusDeg * 0.5) * params.randomnessLevel;
    double centerOffsetY = randomDouble(-spreadRadiusDeg * 0.5, spreadRadiusDeg * 0.5) * params.randomnessLevel;
    double centerX = (start.x() + end.x()) / 2.0 + centerOffsetX;
    double centerY = (start.y() + end.y()) / 2.0 + centerOffsetY;
    
    double angle = std::atan2(dy, dx);
    // Randomize angle more significantly
    angle += randomDouble(-0.5, 0.5) * params.randomnessLevel;
    
    // Radius of each loop - significantly larger
    double loopRadius = std::max(distance * params.curveFactor * 0.8, spreadRadiusDeg * 0.7);
    
    // Randomize loop size asymmetry (one loop bigger than other) - wider range
    double loopAsymmetry = randomDouble(0.5, 1.5) * (0.7 + params.pathAsymmetry * 0.6);
    
    // Number of complete figure-8 loops with variation - based on parameters
    int baseLoops = 1 + static_cast<int>(params.spiralTurns / 2.0);
    int numLoops = baseLoops + (randomDouble(0, 1) > (0.7 - params.randomnessLevel * 0.3) ? 1 : 0);
    numLoops = std::max(1, std::min(numLoops, 5));  // Clamp to 1-5 loops
    int pointsPerLoop = params.numWaypoints / numLoops;
    
    // Random eccentricity for deformed figure-8 - wider range
    double eccentricityX = randomDouble(0.5, 1.5) * (0.8 + params.pathAsymmetry * 0.4);
    double eccentricityY = randomDouble(0.5, 1.5) * (0.8 + params.pathAsymmetry * 0.4);
    
    // Wobble parameters for organic feel
    double wobbleFreq = 3.0 + randomDouble(0, 4.0) * params.wobbleIntensity;
    double wobblePhase = randomDouble(0, 2.0 * M_PI);
    double wobbleAmp = spreadRadiusDeg * params.wobbleIntensity * 0.25;
    
    // Figure-8 rotation speed variation
    double rotationSpeed = 1.0 + randomDouble(-0.2, 0.2) * params.randomnessLevel;
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    // Generate figure-8 pattern with randomness
    for (int loop = 0; loop < numLoops; loop++) {
        // Vary loop radius over the course of the pattern
        double loopProgress = static_cast<double>(loop) / numLoops;
        double currentLoopRadius = loopRadius * (loop % 2 == 0 ? loopAsymmetry : 1.0 / loopAsymmetry);
        currentLoopRadius *= (0.8 + 0.4 * std::sin(loopProgress * M_PI));  // Size variation
        
        for (int i = 0; i < pointsPerLoop; i++) {
            double t = static_cast<double>(i) / pointsPerLoop + loop;
            double theta = t * 2.0 * M_PI * rotationSpeed;
            
            // Random frequency variation per point
            double freqVariation = 1.0 + randomDouble(-0.15, 0.15) * params.randomnessLevel;
            
            // Parametric equations for figure-8 with eccentricity
            double scale = currentLoopRadius / (1.0 + std::pow(std::cos(theta * freqVariation), 2) * 0.5);
            double localX = scale * eccentricityX * std::cos(theta);
            double localY = scale * eccentricityY * std::sin(2.0 * theta) / 2.0;
            
            // Multi-frequency wobble
            double wobble1 = wobbleAmp * std::sin(theta * wobbleFreq + wobblePhase);
            double wobble2 = wobbleAmp * 0.5 * std::sin(theta * wobbleFreq * 1.7 + wobblePhase + M_PI/3);
            double noise = randomDouble(-wobbleAmp * 0.5, wobbleAmp * 0.5) * params.noiseScale;
            
            localX += wobble1 + noise;
            localY += wobble2 + randomDouble(-wobbleAmp * 0.3, wobbleAmp * 0.3) * params.noiseScale;
            
            // Rotate and translate to align with start-end axis
            double x = centerX + localX * std::cos(angle) - localY * std::sin(angle);
            double y = centerY + localX * std::sin(angle) + localY * std::cos(angle);
            
            // Altitude variation with parameters
            double altBase = params.defaultAltitude;
            double altCurve = params.altitudeVariation * 0.7 * std::sin(theta);
            double altNoise = randomDouble(-params.altitudeVariation * 0.35, params.altitudeVariation * 0.35) * params.randomnessLevel;
            double alt = altBase + altCurve + altNoise;
            
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
    double mainAngle = std::atan2(dy, dx);
    
    // Convert spread radius from km to degrees (approx 111km per degree)
    double spreadRadiusDeg = params.spreadRadiusKm / 111.0;
    
    // Spiral parameters with randomization
    double baseTurns = params.spiralTurns + randomDouble(-0.5, 1.0) * params.randomnessLevel;
    baseTurns = std::max(1.5, baseTurns);  // Ensure at least 1.5 turns for visible spiral
    
    double expansionRate = params.spiralExpansionRate + randomDouble(-0.2, 0.3) * params.randomnessLevel;
    expansionRate = std::max(0.3, std::min(2.0, expansionRate));
    
    // Maximum spiral radius - based on distance and spread
    double maxRadius = std::max(distance * 0.4, spreadRadiusDeg * 0.6);
    maxRadius *= (0.7 + params.curveFactor * 0.6);  // Scale by curve factor
    
    // Spiral direction with randomization
    bool clockwise = params.spiralClockwise;
    if (randomDouble(0, 1) < params.randomnessLevel * 0.3) {
        clockwise = !clockwise;  // Random chance to flip direction
    }
    double directionMultiplier = clockwise ? 1.0 : -1.0;
    
    // Tightness affects how quickly radius grows relative to angle
    double tightness = params.spiralTightness + randomDouble(-0.15, 0.15) * params.randomnessLevel;
    tightness = std::max(0.1, std::min(1.0, tightness));
    
    // Elliptical deformation for more organic spirals
    double eccentricityX = 1.0 + randomDouble(-0.25, 0.25) * params.randomnessLevel;
    double eccentricityY = 1.0 + randomDouble(-0.25, 0.25) * params.randomnessLevel;
    
    // Starting phase offset for variety
    double phaseOffset = randomDouble(0, 2.0 * M_PI) * params.randomnessLevel;
    
    // Wobble parameters
    double wobbleFreq = 3.0 + randomDouble(-1.0, 2.0) * params.randomnessLevel;
    double wobbleAmp = maxRadius * params.wobbleIntensity * 0.15;
    
    // Calculate center point - spiral emanates from start towards end
    double centerX = start.x();
    double centerY = start.y();
    
    // Total angle to traverse for spiral
    double totalAngle = baseTurns * 2.0 * M_PI;
    
    // Generate true Archimedean spiral: r = a + b*θ
    // Start radius (a) and growth rate (b)
    double startRadius = maxRadius * 0.05 * (1.0 - tightness * 0.8);  // Small initial radius
    double growthRate = (maxRadius - startRadius) / totalAngle * expansionRate;
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Current angle in the spiral (0 to totalAngle)
        double theta = t * totalAngle + phaseOffset;
        
        // Archimedean spiral radius: r = a + b*θ
        double baseRadius = startRadius + growthRate * theta;
        
        // Add radius variation for organic feel
        double radiusVariation = 1.0 + randomDouble(-0.12, 0.12) * params.noiseScale;
        // Sinusoidal variation on top of random
        radiusVariation += 0.08 * std::sin(theta * 2.5) * params.randomnessLevel;
        double radius = baseRadius * radiusVariation;
        
        // Add wobble
        double wobble = wobbleAmp * std::sin(theta * wobbleFreq);
        wobble += randomDouble(-wobbleAmp * 0.4, wobbleAmp * 0.4) * params.randomnessLevel;
        radius += wobble;
        
        // Spiral coordinates with elliptical deformation
        double spiralX = radius * eccentricityX * std::cos(theta * directionMultiplier);
        double spiralY = radius * eccentricityY * std::sin(theta * directionMultiplier);
        
        // Rotate spiral to align with start-to-end direction
        double rotatedX = spiralX * std::cos(mainAngle) - spiralY * std::sin(mainAngle);
        double rotatedY = spiralX * std::sin(mainAngle) + spiralY * std::cos(mainAngle);
        
        // Progress toward end point (spiral drifts toward destination)
        double driftFactor = std::pow(t, 1.5);  // Non-linear drift toward end
        double driftX = driftFactor * dx * 0.6;
        double driftY = driftFactor * dy * 0.6;
        
        // Final position
        double x = centerX + rotatedX + driftX;
        double y = centerY + rotatedY + driftY;
        
        // Random noise for organic path
        double noise = spreadRadiusDeg * params.noiseScale * 0.1 * params.randomnessLevel;
        x += randomDouble(-noise, noise);
        y += randomDouble(-noise, noise);
        
        // Altitude varies with spiral phase and adds randomness
        double altBase = params.defaultAltitude;
        double altSpiral = params.altitudeVariation * 0.5 * std::sin(theta);
        double altRandom = randomDouble(-params.altitudeVariation * 0.3, params.altitudeVariation * 0.3) * params.randomnessLevel;
        double alt = altBase + altSpiral + altRandom;
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    // Ensure last point connects reasonably to end
    if (!path.isEmpty()) {
        // Add a final approach to end point
        QgsPointXYZ lastPoint = path.last();
        double finalDx = end.x() - lastPoint.x();
        double finalDy = end.y() - lastPoint.y();
        
        // Add a few points to smoothly approach the end
        for (int i = 1; i <= 3; i++) {
            double t = static_cast<double>(i) / 4.0;
            double x = lastPoint.x() + t * finalDx + randomDouble(-spreadRadiusDeg * 0.02, spreadRadiusDeg * 0.02);
            double y = lastPoint.y() + t * finalDy + randomDouble(-spreadRadiusDeg * 0.02, spreadRadiusDeg * 0.02);
            double alt = params.defaultAltitude + randomDouble(-30, 30);
            path.append(QgsPointXYZ(x, y, alt));
        }
    }
    
    // Final end point
    path.append(QgsPointXYZ(end.x(), end.y(), params.defaultAltitude));
    
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
    
    // Randomize frequency and amplitude - wider range
    double frequency = params.zigzagFrequency + randomDouble(-3, 4) * params.randomnessLevel;
    frequency = std::max(2.0, frequency);  // At least 2 oscillations
    double amplitudeVariation = randomDouble(0.6, 1.5) * (0.8 + params.pathAsymmetry * 0.4);
    double amplitude = baseAmplitude * amplitudeVariation;
    
    // Random phase offset
    double phaseOffset = randomDouble(0, 2.0 * M_PI);
    
    // Choose zigzag style - more variety
    int zigzagStyle = static_cast<int>(randomDouble(0, 3));  // 0: sine, 1: triangle, 2: mixed
    
    // Frequency modulation for organic zigzag
    double freqMod = randomDouble(0.5, 1.5) * params.wobbleIntensity;
    
    // Asymmetry - one direction of zigzag larger than other
    double asymmetry = randomDouble(-0.4, 0.4) * params.pathAsymmetry;
    
    // Wobble parameters
    double wobbleFreq = 5.0 + randomDouble(0, 6.0) * params.wobbleIntensity;
    double wobblePhase = randomDouble(0, 2.0 * M_PI);
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Variable amplitude per zigzag peak with clustering
        double clusterMod = 1.0;
        if (params.clusterTendency > 0.3) {
            // Create amplitude clusters
            clusterMod = 0.7 + 0.6 * std::sin(t * 3.0 * M_PI);
        }
        double localAmplitude = amplitude * (1.0 + randomDouble(-0.5, 0.5) * params.randomnessLevel) * clusterMod;
        
        // Frequency modulation over path
        double localFreq = frequency * (1.0 + freqMod * std::sin(t * 2.0 * M_PI));
        
        // Zigzag offset with style variation
        double phase = t * localFreq + phaseOffset;
        double waveValue;
        
        switch (zigzagStyle) {
            case 0:  // Smoother sine-based zigzag
                waveValue = std::sin(phase * 2.0 * M_PI);
                break;
            case 1:  // Sharp triangle wave zigzag
            {
                double triangleWave = 2.0 * std::abs(phase - std::floor(phase + 0.5));
                waveValue = triangleWave * 2.0 - 1.0;
            }
                break;
            case 2:  // Mixed - sine with sharp peaks
            default:
            {
                double sineVal = std::sin(phase * 2.0 * M_PI);
                waveValue = sineVal * (1.0 + 0.3 * std::abs(sineVal));  // Sharper peaks
            }
                break;
        }
        
        // Apply asymmetry
        if (waveValue > 0) {
            waveValue *= (1.0 + asymmetry);
        } else {
            waveValue *= (1.0 - asymmetry);
        }
        
        double offset = localAmplitude * waveValue;
        
        // Softer edge fade for wider coverage
        double edgeFade = std::pow(4.0 * t * (1.0 - t), 0.4);
        offset *= std::max(edgeFade, 0.35);
        
        // Multi-frequency wobble
        double wobble1 = spreadRadiusDeg * params.wobbleIntensity * 0.2 * std::sin(t * wobbleFreq * M_PI + wobblePhase);
        double wobble2 = spreadRadiusDeg * params.wobbleIntensity * 0.1 * std::sin(t * wobbleFreq * 2.3 * M_PI);
        double noise = randomDouble(-spreadRadiusDeg * 0.15, spreadRadiusDeg * 0.15) * params.noiseScale;
        double wobble = wobble1 + wobble2 + noise;
        
        // Forward-backward random variation - larger
        double forwardOffset = randomDouble(-distance * 0.035, distance * 0.035) * params.randomnessLevel;
        
        double x = start.x() + t * dx + offset * perpX + wobble * perpX + forwardOffset * (dx / distance);
        double y = start.y() + t * dy + offset * perpY + wobble * perpY + forwardOffset * (dy / distance);
        
        // Altitude variation with parameters
        double altBase = params.defaultAltitude;
        double altWave = params.altitudeVariation * 0.5 * waveValue;
        double altNoise = randomDouble(-params.altitudeVariation * 0.3, params.altitudeVariation * 0.3) * params.randomnessLevel;
        double alt = altBase + altWave + altNoise;
        
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
    
    // Randomize control point positions for variety - wider range
    double cp1Ratio = 0.2 + randomDouble(-0.15, 0.25) * params.randomnessLevel;
    double cp2Ratio = 0.8 + randomDouble(-0.25, 0.15) * params.randomnessLevel;
    
    // Asymmetry - control points can be on same or opposite sides
    bool sameDirection = randomDouble(0, 1) > params.pathAsymmetry;
    double directionMult = sameDirection ? 1.0 : -1.0;
    
    // Random perpendicular offset for each control point - wider range
    double cp1Offset = cpOffset * randomDouble(0.4, 1.8) * (randomDouble(0, 1) > 0.5 ? 1.0 : -1.0);
    double cp2Offset = cpOffset * randomDouble(0.4, 1.8) * directionMult * (cp1Offset > 0 ? 1.0 : -1.0);
    
    // Add some forward/backward randomness to control points - larger range
    double cp1Forward = randomDouble(-0.15, 0.15) * distance * params.randomnessLevel;
    double cp2Forward = randomDouble(-0.15, 0.15) * distance * params.randomnessLevel;
    
    // Control points for cubic Bezier with randomization
    QgsPointXY p0 = start;
    QgsPointXY p1(start.x() + cp1Ratio * dx + cp1Offset * perpX + cp1Forward * (dx / distance), 
                  start.y() + cp1Ratio * dy + cp1Offset * perpY + cp1Forward * (dy / distance));
    QgsPointXY p2(start.x() + cp2Ratio * dx + cp2Offset * perpX + cp2Forward * (dx / distance),
                  start.y() + cp2Ratio * dy + cp2Offset * perpY + cp2Forward * (dy / distance));
    QgsPointXY p3 = end;
    
    // Multi-frequency wobble for organic feel
    double wobbleAmp = spreadRadiusDeg * params.wobbleIntensity * 0.3;
    double wobbleFreq1 = randomDouble(2.0, 5.0);
    double wobbleFreq2 = randomDouble(4.0, 8.0);
    double wobblePhase1 = randomDouble(0, 2.0 * M_PI);
    double wobblePhase2 = randomDouble(0, 2.0 * M_PI);
    
    for (int i = 0; i <= params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        QgsPointXY pt = bezierPoint(p0, p1, p2, p3, t);
        
        // Edge fade for smoother endpoints
        double edgeFade = std::sin(M_PI * t);
        
        // Multi-frequency sinusoidal wobble along the path
        double wobble1 = wobbleAmp * std::sin(t * wobbleFreq1 * M_PI + wobblePhase1);
        double wobble2 = wobbleAmp * 0.5 * std::sin(t * wobbleFreq2 * M_PI + wobblePhase2);
        double noise = randomDouble(-wobbleAmp * 0.5, wobbleAmp * 0.5) * params.noiseScale;
        double wobble = (wobble1 + wobble2 + noise) * edgeFade;
        
        // Apply wobble perpendicular to path direction
        double x = pt.x() + wobble * perpX;
        double y = pt.y() + wobble * perpY;
        
        // Altitude variation with parameters
        double altBase = params.defaultAltitude;
        double altCurve = params.altitudeVariation * 0.7 * std::sin(M_PI * t);
        double altNoise = randomDouble(-params.altitudeVariation * 0.3, params.altitudeVariation * 0.3) * params.randomnessLevel * edgeFade;
        double alt = altBase + altCurve + altNoise;
        
        path.append(QgsPointXYZ(x, y, alt));
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
    double maxOffset = std::max(distance * params.randomVariance * 2.5, spreadRadiusDeg * 1.2);
    
    // Determine randomness style based on parameters - more variety
    int randomStyle;
    double styleSelector = randomDouble(0, 1);
    if (styleSelector < 0.25) {
        randomStyle = 0;  // Pure random
    } else if (styleSelector < 0.25 + params.clusterTendency * 0.5) {
        randomStyle = 2;  // Clustered random - more likely with higher cluster tendency
    } else if (styleSelector < 0.6) {
        randomStyle = 1;  // Random walk
    } else {
        randomStyle = 3;  // New: Levy flight style (occasional large jumps)
    }
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.defaultAltitude));
    
    double prevOffsetX = 0, prevOffsetY = 0;  // For random walk style
    double clusterCenterX = 0, clusterCenterY = 0;  // For clustered random
    int clusterSize = 3 + static_cast<int>(params.clusterTendency * 5);  // Cluster size based on parameter
    
    // Random intermediate waypoints
    for (int i = 1; i < params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Base position along the line with random forward/back variation - larger
        double forwardVariation = randomDouble(-0.15, 0.15) * params.randomnessLevel * distance;
        double baseX = start.x() + t * dx + forwardVariation * (dx / distance);
        double baseY = start.y() + t * dy + forwardVariation * (dy / distance);
        
        double randomOffsetX, randomOffsetY;
        
        switch (randomStyle) {
            case 0:  // Pure random - full area coverage with asymmetry
            {
                double asymmetryX = 1.0 + params.pathAsymmetry * randomDouble(-0.5, 0.5);
                double asymmetryY = 1.0 + params.pathAsymmetry * randomDouble(-0.5, 0.5);
                randomOffsetX = randomDouble(-maxOffset, maxOffset) * asymmetryX;
                randomOffsetY = randomDouble(-maxOffset, maxOffset) * asymmetryY;
            }
                break;
                
            case 1:  // Random walk - more connected movements
            {
                double walkStep = maxOffset * (0.3 + params.wobbleIntensity * 0.3);
                prevOffsetX += randomDouble(-walkStep, walkStep);
                prevOffsetY += randomDouble(-walkStep, walkStep);
                // Clamp to max offset with soft boundary
                double clampFactor = 0.9;
                if (std::abs(prevOffsetX) > maxOffset * clampFactor) {
                    prevOffsetX *= 0.8;  // Soft bounce back
                }
                if (std::abs(prevOffsetY) > maxOffset * clampFactor) {
                    prevOffsetY *= 0.8;
                }
                randomOffsetX = prevOffsetX;
                randomOffsetY = prevOffsetY;
            }
                break;
                
            case 2:  // Clustered random - random but with occasional clusters
            {
                // Decide if this point starts a new cluster
                bool newCluster = (i % clusterSize == 1) || randomDouble(0, 1) > (0.8 - params.randomnessLevel * 0.3);
                if (newCluster) {
                    clusterCenterX = randomDouble(-maxOffset, maxOffset);
                    clusterCenterY = randomDouble(-maxOffset, maxOffset);
                }
                // Add variation around cluster center based on cluster tendency
                double clusterVariation = maxOffset * (0.2 + (1.0 - params.clusterTendency) * 0.4);
                randomOffsetX = clusterCenterX + randomDouble(-clusterVariation, clusterVariation);
                randomOffsetY = clusterCenterY + randomDouble(-clusterVariation, clusterVariation);
            }
                break;
                
            case 3:  // Levy flight style - occasional large jumps
            default:
            {
                // Most steps are small, occasional large jumps
                bool largeJump = randomDouble(0, 1) < (0.1 + params.randomnessLevel * 0.15);
                double stepScale = largeJump ? randomDouble(0.6, 1.0) : randomDouble(0.1, 0.4);
                double angle = randomDouble(0, 2.0 * M_PI);
                double stepSize = maxOffset * stepScale;
                
                prevOffsetX += stepSize * std::cos(angle);
                prevOffsetY += stepSize * std::sin(angle);
                
                // Soft clamp
                double dist = std::sqrt(prevOffsetX * prevOffsetX + prevOffsetY * prevOffsetY);
                if (dist > maxOffset) {
                    prevOffsetX *= maxOffset / dist * 0.9;
                    prevOffsetY *= maxOffset / dist * 0.9;
                }
                randomOffsetX = prevOffsetX;
                randomOffsetY = prevOffsetY;
            }
                break;
        }
        
        // Softer edge fade - maintain more randomness near endpoints
        double edgeFade = std::pow(4.0 * t * (1.0 - t), 0.35);
        edgeFade = std::max(edgeFade, 0.25);  // Minimum 25% even at edges
        
        randomOffsetX *= edgeFade;
        randomOffsetY *= edgeFade;
        
        // Add noise on top
        double noise = spreadRadiusDeg * params.noiseScale * 0.2;
        randomOffsetX += randomDouble(-noise, noise);
        randomOffsetY += randomDouble(-noise, noise);
        
        double x = baseX + randomOffsetX;
        double y = baseY + randomOffsetY;
        
        // Wide altitude variation with parameters
        double altBase = params.defaultAltitude;
        double altRange = params.altitudeVariation * (0.8 + params.randomnessLevel * 0.5);
        double altVariation = altBase + randomDouble(-altRange, altRange) * edgeFade;
        
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
