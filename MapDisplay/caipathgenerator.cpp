#include "caipathgenerator.h"
#include <QRandomGenerator>
#include <QUrl>
#include <QNetworkRequest>
#include <cmath>

CAIPathGenerator::CAIPathGenerator(QObject *parent)
    : QObject(parent)
{
    _m_networkManager = new QNetworkAccessManager(this);
    connect(_m_networkManager, &QNetworkAccessManager::finished,
            this, &CAIPathGenerator::onNetworkReplyFinished);
}

QString CAIPathGenerator::getMissionTypeName(eVISTAR_AI_MISSION_TYPE missionType)
{
    switch (missionType) {
        case AI_MISSION_PATROL:    return "Patrol";
        case AI_MISSION_STRIKE:    return "Strike";
        case AI_MISSION_RECON:     return "Reconnaissance";
        case AI_MISSION_EVASION:   return "Evasion";
        case AI_MISSION_INTERCEPT: return "Intercept";
        case AI_MISSION_ESCORT:    return "Escort";
        case AI_MISSION_CUSTOM:    return "Custom";
        default:                   return "Unknown";
    }
}

QString CAIPathGenerator::getMissionTypeDescription(eVISTAR_AI_MISSION_TYPE missionType)
{
    switch (missionType) {
        case AI_MISSION_PATROL:
            return "Area patrol with coverage optimization - suitable for surveillance and monitoring";
        case AI_MISSION_STRIKE:
            return "Direct approach with final attack run - optimized for strike missions";
        case AI_MISSION_RECON:
            return "Intelligence gathering path with multiple observation points";
        case AI_MISSION_EVASION:
            return "Unpredictable path to avoid detection and interception";
        case AI_MISSION_INTERCEPT:
            return "Optimal intercept trajectory to reach target quickly";
        case AI_MISSION_ESCORT:
            return "Protective pattern maintaining proximity to defended asset";
        case AI_MISSION_CUSTOM:
            return "Custom mission path based on specific parameters";
        default:
            return "No mission type selected";
    }
}

void CAIPathGenerator::setDefaultParameters(const AIPathParameters &params)
{
    _m_defaultParams = params;
}

CAIPathGenerator::AIPathParameters CAIPathGenerator::getDefaultParameters() const
{
    return _m_defaultParams;
}

bool CAIPathGenerator::isExternalAIAvailable() const
{
    return !_m_apiEndpoint.isEmpty() && !_m_apiKey.isEmpty();
}

void CAIPathGenerator::configureExternalAI(const QString &endpoint, const QString &apiKey)
{
    _m_apiEndpoint = endpoint;
    _m_apiKey = apiKey;
}

QList<QgsPointXYZ> CAIPathGenerator::generateAIPath(const QgsPointXY &startPoint,
                                                     const QgsPointXY &endPoint,
                                                     const AIPathParameters &params)
{
    emit signalGenerationProgress(0, "Initializing AI path generation...");
    
    QList<QgsPointXYZ> path;
    
    switch (params.missionType) {
        case AI_MISSION_PATROL:
            path = generatePatrolPath(startPoint, endPoint, params);
            break;
        case AI_MISSION_STRIKE:
            path = generateStrikePath(startPoint, endPoint, params);
            break;
        case AI_MISSION_RECON:
            path = generateReconPath(startPoint, endPoint, params);
            break;
        case AI_MISSION_EVASION:
            path = generateEvasionPath(startPoint, endPoint, params);
            break;
        case AI_MISSION_INTERCEPT:
            path = generateInterceptPath(startPoint, endPoint, params);
            break;
        case AI_MISSION_ESCORT:
            path = generateEscortPath(startPoint, endPoint, params);
            break;
        case AI_MISSION_CUSTOM:
        default:
            path = generateCustomPath(startPoint, endPoint, params);
            break;
    }
    
    emit signalGenerationProgress(100, "AI path generation complete");
    return path;
}

void CAIPathGenerator::generateAIPathAsync(const QgsPointXY &startPoint,
                                            const QgsPointXY &endPoint,
                                            const AIPathParameters &params)
{
    if (params.useExternalAI && isExternalAIAvailable()) {
        callExternalAI(startPoint, endPoint, params);
    } else {
        // Use built-in AI and emit result
        QList<QgsPointXYZ> path = generateAIPath(startPoint, endPoint, params);
        emit signalAIPathGenerated(path, true, "");
    }
}

// ============ AI Path Generation Algorithms ============

QList<QgsPointXYZ> CAIPathGenerator::generatePatrolPath(const QgsPointXY &start, 
                                                         const QgsPointXY &end, 
                                                         const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Analyzing patrol area...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Create center point
    QgsPointXY center((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);
    
    // Patrol radius based on distance
    double patrolRadius = distance * 0.4;
    double patrolRadiusDeg = patrolRadius / 111.0;  // Convert km to degrees
    
    emit signalGenerationProgress(30, "Generating patrol pattern...");
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    // Generate racetrack/oval patrol pattern
    int lobes = 2 + static_cast<int>(params.pathRandomness * 2);
    int pointsPerSection = params.numWaypoints / (lobes * 2);
    
    for (int lobe = 0; lobe < lobes; lobe++) {
        double lobeAngle = bearing + (lobe * M_PI / lobes);
        
        // First half - outbound
        for (int i = 0; i < pointsPerSection; i++) {
            double t = static_cast<double>(i) / pointsPerSection;
            double angle = lobeAngle + t * M_PI;
            
            double radiusVar = patrolRadiusDeg * (0.8 + randomDouble(-0.2, 0.2) * params.pathRandomness);
            
            double x = center.x() + radiusVar * std::cos(angle);
            double y = center.y() + radiusVar * std::sin(angle) * 0.7;  // Slightly elliptical
            
            // Altitude variation for patrol
            double alt = params.preferredAltitude + 
                        (params.maxAltitude - params.preferredAltitude) * 0.2 * std::sin(t * M_PI);
            alt += randomDouble(-100, 100) * params.pathRandomness;
            alt = qBound(params.minAltitude, alt, params.maxAltitude);
            
            // Threat avoidance if enabled
            QgsPointXY pt(x, y);
            if (params.avoidDetection && !params.threatLocations.isEmpty()) {
                QgsPointXY lastPt(path.last().x(), path.last().y());
                pt = avoidThreat(lastPt, pt, params.threatLocations, params.threatRadius);
            }
            
            path.append(QgsPointXYZ(pt.x(), pt.y(), alt));
        }
    }
    
    emit signalGenerationProgress(70, "Optimizing patrol waypoints...");
    
    // End point
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude));
    
    // Smooth the path
    path = smoothPath(path, 3);
    
    emit signalGenerationProgress(90, "Finalizing patrol path...");
    
    return path;
}

QList<QgsPointXYZ> CAIPathGenerator::generateStrikePath(const QgsPointXY &start, 
                                                         const QgsPointXY &end, 
                                                         const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Planning strike approach...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Ingress phase - stay low if terrain following
    double ingressAlt = params.terrainFollowing ? 
                        params.minAltitude + params.terrainClearance : 
                        params.preferredAltitude * 0.5;
    
    emit signalGenerationProgress(30, "Calculating ingress route...");
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    // Initial climb/descent to ingress altitude
    int ingressPoints = params.numWaypoints * 0.3;
    int attackPoints = params.numWaypoints * 0.4;
    int egressPoints = params.numWaypoints * 0.3;
    
    // Ingress - approach from offset angle to avoid direct detection
    double offsetAngle = bearing + (randomDouble(-0.3, 0.3) * M_PI);
    double offsetDist = distance * 0.3;
    
    QgsPointXY ingressWaypoint = movePoint(end, offsetAngle + M_PI, offsetDist);
    
    for (int i = 1; i < ingressPoints; i++) {
        double t = static_cast<double>(i) / ingressPoints;
        
        double x = start.x() + t * (ingressWaypoint.x() - start.x());
        double y = start.y() + t * (ingressWaypoint.y() - start.y());
        
        // Gradual descent to ingress altitude
        double alt = params.preferredAltitude + (ingressAlt - params.preferredAltitude) * t;
        
        // Add subtle evasive variations
        if (params.avoidDetection) {
            x += randomDouble(-0.02, 0.02) * params.pathRandomness;
            y += randomDouble(-0.02, 0.02) * params.pathRandomness;
        }
        
        QgsPointXY pt(x, y);
        if (params.avoidDetection && !params.threatLocations.isEmpty()) {
            QgsPointXY lastPt(path.last().x(), path.last().y());
            pt = avoidThreat(lastPt, pt, params.threatLocations, params.threatRadius);
        }
        
        path.append(QgsPointXYZ(pt.x(), pt.y(), alt));
    }
    
    emit signalGenerationProgress(50, "Planning attack run...");
    
    // Attack run - direct approach with pop-up
    double popUpAlt = params.preferredAltitude * 1.2;  // Pop-up for weapons release
    
    for (int i = 0; i < attackPoints; i++) {
        double t = static_cast<double>(i) / attackPoints;
        
        double x = ingressWaypoint.x() + t * (end.x() - ingressWaypoint.x());
        double y = ingressWaypoint.y() + t * (end.y() - ingressWaypoint.y());
        
        // Pop-up profile - rise then descend for attack
        double popUpFactor = std::sin(t * M_PI);
        double alt = ingressAlt + (popUpAlt - ingressAlt) * popUpFactor;
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    // Target point
    path.append(QgsPointXYZ(end.x(), end.y(), ingressAlt));
    
    emit signalGenerationProgress(80, "Planning egress...");
    
    // Egress - escape in opposite direction
    double egressBearing = bearing + M_PI + randomDouble(-0.5, 0.5);
    QgsPointXY egressPoint = movePoint(end, egressBearing, distance * 0.5);
    
    for (int i = 1; i <= egressPoints; i++) {
        double t = static_cast<double>(i) / egressPoints;
        
        double x = end.x() + t * (egressPoint.x() - end.x());
        double y = end.y() + t * (egressPoint.y() - end.y());
        
        // Climb back to safe altitude
        double alt = ingressAlt + (params.preferredAltitude - ingressAlt) * t;
        
        // Evasive jinking during egress
        if (params.avoidDetection) {
            double jink = randomDouble(-0.03, 0.03) * (1.0 - t);
            x += jink;
            y += jink;
        }
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    emit signalGenerationProgress(95, "Finalizing strike path...");
    
    return smoothPath(path, 2);
}

QList<QgsPointXYZ> CAIPathGenerator::generateReconPath(const QgsPointXY &start, 
                                                        const QgsPointXY &end, 
                                                        const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Planning reconnaissance route...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // High altitude for better sensor coverage
    double reconAlt = params.maxAltitude * 0.8;
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(30, "Generating observation points...");
    
    // Create zigzag pattern for maximum area coverage
    int legs = 4 + static_cast<int>(params.pathRandomness * 3);
    int pointsPerLeg = params.numWaypoints / legs;
    
    // Perpendicular direction for zigzag
    double perpBearing = bearing + M_PI / 2;
    double sweepWidth = distance * 0.3;
    
    for (int leg = 0; leg < legs; leg++) {
        double legProgress = static_cast<double>(leg) / legs;
        
        // Alternate sides
        double sideOffset = (leg % 2 == 0 ? 1.0 : -1.0) * sweepWidth / 111.0;
        
        for (int i = 0; i < pointsPerLeg; i++) {
            double t = static_cast<double>(i) / pointsPerLeg;
            double totalProgress = legProgress + t / legs;
            
            // Main progression along route
            double mainX = start.x() + totalProgress * (end.x() - start.x());
            double mainY = start.y() + totalProgress * (end.y() - start.y());
            
            // Zigzag offset with smooth transition
            double zigzagT = std::sin(t * M_PI);
            double offsetX = sideOffset * std::cos(perpBearing) * zigzagT;
            double offsetY = sideOffset * std::sin(perpBearing) * zigzagT;
            
            // Add observation hold points
            bool isObservationPoint = (i == pointsPerLeg / 2);
            
            double alt = reconAlt;
            if (isObservationPoint) {
                // Slight altitude increase at observation points
                alt += 500;
            }
            
            // Slight randomization for unpredictability
            double x = mainX + offsetX + randomDouble(-0.01, 0.01) * params.pathRandomness;
            double y = mainY + offsetY + randomDouble(-0.01, 0.01) * params.pathRandomness;
            
            QgsPointXY pt(x, y);
            if (params.avoidDetection && !params.threatLocations.isEmpty()) {
                QgsPointXY lastPt(path.last().x(), path.last().y());
                pt = avoidThreat(lastPt, pt, params.threatLocations, params.threatRadius);
            }
            
            path.append(QgsPointXYZ(pt.x(), pt.y(), alt));
        }
    }
    
    emit signalGenerationProgress(80, "Optimizing coverage...");
    
    // End point
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude));
    
    return smoothPath(path, 2);
}

QList<QgsPointXYZ> CAIPathGenerator::generateEvasionPath(const QgsPointXY &start, 
                                                          const QgsPointXY &end, 
                                                          const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Generating evasive maneuvers...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(30, "Planning unpredictable route...");
    
    // Highly unpredictable path with random direction changes
    int numSegments = params.numWaypoints;
    double currentAlt = params.preferredAltitude;
    QgsPointXY currentPos = start;
    
    // Use random walk with bias toward destination
    double biasStrength = 0.3;  // How strongly to bias toward end
    
    for (int i = 1; i < numSegments; i++) {
        double t = static_cast<double>(i) / numSegments;
        
        emit signalGenerationProgress(30 + static_cast<int>(50 * t), "Generating evasive waypoint...");
        
        // Random direction change
        double randomAngle = randomDouble(-M_PI, M_PI) * params.pathRandomness;
        
        // Bias toward destination increases as we get closer
        double biasFactor = biasStrength + (1.0 - biasStrength) * t;
        double targetBearing = calculateBearing(currentPos, end);
        double actualBearing = targetBearing * biasFactor + randomAngle * (1.0 - biasFactor);
        
        // Step size varies randomly
        double stepSize = (distance / numSegments) * (0.5 + randomDouble(0, 1.0) * params.pathRandomness);
        stepSize = qMin(stepSize, calculateDistance(currentPos, end) * 0.5);
        
        QgsPointXY newPos = movePoint(currentPos, actualBearing, stepSize);
        
        // Dramatic altitude changes for evasion
        double altChange = randomDouble(-500, 500) * params.pathRandomness;
        if (randomDouble(0, 1) > 0.7) {
            // Occasional dramatic altitude change
            altChange = randomDouble(-1500, 1500);
        }
        currentAlt = qBound(params.minAltitude, currentAlt + altChange, params.maxAltitude);
        
        // Threat avoidance with aggressive maneuvering
        if (params.avoidDetection && !params.threatLocations.isEmpty()) {
            newPos = avoidThreat(currentPos, newPos, params.threatLocations, params.threatRadius * 1.5);
        }
        
        path.append(QgsPointXYZ(newPos.x(), newPos.y(), currentAlt));
        currentPos = newPos;
    }
    
    // Ensure we reach the end
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(90, "Finalizing evasion path...");
    
    // Don't smooth too much - keep the unpredictability
    return path;
}

QList<QgsPointXYZ> CAIPathGenerator::generateInterceptPath(const QgsPointXY &start, 
                                                            const QgsPointXY &end, 
                                                            const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Calculating intercept trajectory...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Intercept path - mostly direct with optimal climb/dive profile
    double optimalInterceptAlt = params.maxAltitude * 0.9;  // High for speed and energy
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(30, "Optimizing intercept angle...");
    
    // Lead angle for intercept (simulated)
    double leadAngle = randomDouble(-0.1, 0.1) * M_PI;  // Small lead adjustment
    
    int climbPhase = params.numWaypoints * 0.2;
    int cruisePhase = params.numWaypoints * 0.6;
    int attackPhase = params.numWaypoints * 0.2;
    
    // Climb phase - rapid climb to optimal altitude
    for (int i = 1; i <= climbPhase; i++) {
        double t = static_cast<double>(i) / climbPhase;
        
        double x = start.x() + t * 0.2 * (end.x() - start.x());
        double y = start.y() + t * 0.2 * (end.y() - start.y());
        
        // Rapid climb
        double alt = params.preferredAltitude + (optimalInterceptAlt - params.preferredAltitude) * t;
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    emit signalGenerationProgress(50, "Calculating cruise trajectory...");
    
    // Cruise phase - high speed at optimal altitude
    QgsPointXY lastClimbPos(path.last().x(), path.last().y());
    for (int i = 1; i <= cruisePhase; i++) {
        double t = static_cast<double>(i) / cruisePhase;
        double overallT = 0.2 + t * 0.6;
        
        // Apply lead angle
        double adjustedBearing = bearing + leadAngle * (1.0 - t);
        double x = start.x() + overallT * (end.x() - start.x());
        double y = start.y() + overallT * (end.y() - start.y());
        
        // Maintain optimal altitude with small variations
        double alt = optimalInterceptAlt + randomDouble(-50, 50);
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    emit signalGenerationProgress(75, "Planning final approach...");
    
    // Attack phase - dive to intercept
    QgsPointXY lastCruisePos(path.last().x(), path.last().y());
    for (int i = 1; i <= attackPhase; i++) {
        double t = static_cast<double>(i) / attackPhase;
        
        double x = lastCruisePos.x() + t * (end.x() - lastCruisePos.x());
        double y = lastCruisePos.y() + t * (end.y() - lastCruisePos.y());
        
        // Diving attack
        double alt = optimalInterceptAlt - (optimalInterceptAlt - params.preferredAltitude * 0.5) * t;
        
        path.append(QgsPointXYZ(x, y, alt));
    }
    
    // Final intercept point
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude * 0.5));
    
    emit signalGenerationProgress(95, "Finalizing intercept path...");
    
    return smoothPath(path, 2);
}

QList<QgsPointXYZ> CAIPathGenerator::generateEscortPath(const QgsPointXY &start, 
                                                         const QgsPointXY &end, 
                                                         const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Planning escort pattern...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Escort offset - maintain position relative to protected asset
    double escortOffset = 0.05;  // ~5km offset in degrees
    double escortAlt = params.preferredAltitude + 500;  // Slightly higher than protected asset
    
    // Start point
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(30, "Generating protective pattern...");
    
    // Weaving pattern around the main route
    for (int i = 1; i < params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Main progression
        double mainX = start.x() + t * (end.x() - start.x());
        double mainY = start.y() + t * (end.y() - start.y());
        
        // Weaving offset - S-curve around the protected asset
        double weavePhase = t * 4 * M_PI;  // Multiple weaves
        double weaveOffset = escortOffset * std::sin(weavePhase);
        
        // Perpendicular offset
        double perpX = -std::sin(bearing) * weaveOffset;
        double perpY = std::cos(bearing) * weaveOffset;
        
        double x = mainX + perpX;
        double y = mainY + perpY;
        
        // Altitude weaving
        double altWeave = 200 * std::sin(weavePhase * 0.5);
        double alt = escortAlt + altWeave;
        
        // Add random alertness movements
        if (randomDouble(0, 1) > 0.8) {
            x += randomDouble(-0.02, 0.02);
            y += randomDouble(-0.02, 0.02);
            alt += randomDouble(-100, 100);
        }
        
        alt = qBound(params.minAltitude, alt, params.maxAltitude);
        
        QgsPointXY pt(x, y);
        if (params.avoidDetection && !params.threatLocations.isEmpty()) {
            QgsPointXY lastPt(path.last().x(), path.last().y());
            pt = avoidThreat(lastPt, pt, params.threatLocations, params.threatRadius);
        }
        
        path.append(QgsPointXYZ(pt.x(), pt.y(), alt));
    }
    
    emit signalGenerationProgress(80, "Optimizing escort pattern...");
    
    // End point
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude));
    
    return smoothPath(path, 2);
}

QList<QgsPointXYZ> CAIPathGenerator::generateCustomPath(const QgsPointXY &start, 
                                                         const QgsPointXY &end, 
                                                         const AIPathParameters &params)
{
    emit signalGenerationProgress(10, "Generating custom AI path...");
    
    QList<QgsPointXYZ> path;
    
    double distance = calculateDistance(start, end);
    double bearing = calculateBearing(start, end);
    
    // Custom path combines elements based on parameters
    path.append(QgsPointXYZ(start.x(), start.y(), params.preferredAltitude));
    
    emit signalGenerationProgress(30, "Applying mission parameters...");
    
    // Dynamic path generation based on enabled features
    for (int i = 1; i < params.numWaypoints; i++) {
        double t = static_cast<double>(i) / params.numWaypoints;
        
        // Base progression
        double x = start.x() + t * (end.x() - start.x());
        double y = start.y() + t * (end.y() - start.y());
        double alt = params.preferredAltitude;
        
        // Apply terrain following simulation
        if (params.terrainFollowing) {
            // Simulate terrain with random variation
            double terrainHeight = params.minAltitude * (0.5 + 0.5 * std::sin(t * 10));
            alt = terrainHeight + params.terrainClearance;
        }
        
        // Apply randomness
        x += randomDouble(-0.05, 0.05) * params.pathRandomness;
        y += randomDouble(-0.05, 0.05) * params.pathRandomness;
        alt += randomDouble(-200, 200) * params.pathRandomness;
        
        // Apply fuel optimization (more direct path)
        if (params.optimizeForFuel) {
            // Reduce deviation
            x = start.x() + t * (end.x() - start.x()) + 
                (x - (start.x() + t * (end.x() - start.x()))) * 0.3;
            y = start.y() + t * (end.y() - start.y()) + 
                (y - (start.y() + t * (end.y() - start.y()))) * 0.3;
        }
        
        alt = qBound(params.minAltitude, alt, params.maxAltitude);
        
        QgsPointXY pt(x, y);
        if (params.avoidDetection && !params.threatLocations.isEmpty()) {
            QgsPointXY lastPt(path.last().x(), path.last().y());
            pt = avoidThreat(lastPt, pt, params.threatLocations, params.threatRadius);
        }
        
        path.append(QgsPointXYZ(pt.x(), pt.y(), alt));
    }
    
    emit signalGenerationProgress(80, "Finalizing custom path...");
    
    path.append(QgsPointXYZ(end.x(), end.y(), params.preferredAltitude));
    
    return smoothPath(path, 2);
}

// ============ Helper Functions ============

double CAIPathGenerator::calculateDistance(const QgsPointXY &p1, const QgsPointXY &p2) const
{
    // Haversine formula for distance in km
    double lat1 = p1.y() * M_PI / 180.0;
    double lat2 = p2.y() * M_PI / 180.0;
    double dLat = lat2 - lat1;
    double dLon = (p2.x() - p1.x()) * M_PI / 180.0;
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon/2) * std::sin(dLon/2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    
    return 6371.0 * c;  // Earth radius in km
}

double CAIPathGenerator::calculateBearing(const QgsPointXY &from, const QgsPointXY &to) const
{
    double lat1 = from.y() * M_PI / 180.0;
    double lat2 = to.y() * M_PI / 180.0;
    double dLon = (to.x() - from.x()) * M_PI / 180.0;
    
    double y = std::sin(dLon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    
    return std::atan2(y, x);
}

QgsPointXY CAIPathGenerator::movePoint(const QgsPointXY &point, double bearing, double distanceKm) const
{
    double lat1 = point.y() * M_PI / 180.0;
    double lon1 = point.x() * M_PI / 180.0;
    double d = distanceKm / 6371.0;  // Angular distance
    
    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                           std::cos(lat1) * std::sin(d) * std::cos(bearing));
    double lon2 = lon1 + std::atan2(std::sin(bearing) * std::sin(d) * std::cos(lat1),
                                    std::cos(d) - std::sin(lat1) * std::sin(lat2));
    
    return QgsPointXY(lon2 * 180.0 / M_PI, lat2 * 180.0 / M_PI);
}

QgsPointXY CAIPathGenerator::offsetPoint(const QgsPointXY &point, double dx, double dy) const
{
    return QgsPointXY(point.x() + dx, point.y() + dy);
}

double CAIPathGenerator::randomDouble(double min, double max) const
{
    return QRandomGenerator::global()->bounded(max - min) + min;
}

bool CAIPathGenerator::isNearThreat(const QgsPointXY &point, const QList<QPointF> &threats, double radius) const
{
    for (const QPointF &threat : threats) {
        QgsPointXY threatPos(threat.y(), threat.x());  // lon, lat
        if (calculateDistance(point, threatPos) < radius) {
            return true;
        }
    }
    return false;
}

QgsPointXY CAIPathGenerator::avoidThreat(const QgsPointXY &current, const QgsPointXY &desired, 
                                          const QList<QPointF> &threats, double radius) const
{
    if (!isNearThreat(desired, threats, radius)) {
        return desired;
    }
    
    // Find closest threat
    double minDist = std::numeric_limits<double>::max();
    QgsPointXY closestThreat;
    
    for (const QPointF &threat : threats) {
        QgsPointXY threatPos(threat.y(), threat.x());
        double dist = calculateDistance(desired, threatPos);
        if (dist < minDist) {
            minDist = dist;
            closestThreat = threatPos;
        }
    }
    
    // Move perpendicular to threat
    double threatBearing = calculateBearing(desired, closestThreat);
    double avoidBearing = threatBearing + M_PI / 2;  // Perpendicular
    
    if (randomDouble(0, 1) > 0.5) {
        avoidBearing = threatBearing - M_PI / 2;  // Other direction
    }
    
    double avoidDistance = radius - minDist + 5;  // Move outside threat radius
    return movePoint(desired, avoidBearing, avoidDistance);
}

double CAIPathGenerator::smoothAltitude(double currentAlt, double targetAlt, double smoothFactor) const
{
    return currentAlt + (targetAlt - currentAlt) * smoothFactor;
}

QgsPointXY CAIPathGenerator::bezierPoint(const QgsPointXY &p0, const QgsPointXY &p1,
                                          const QgsPointXY &p2, const QgsPointXY &p3, double t) const
{
    double u = 1.0 - t;
    double tt = t * t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * t;
    
    double x = uuu * p0.x() + 3 * uu * t * p1.x() + 3 * u * tt * p2.x() + ttt * p3.x();
    double y = uuu * p0.y() + 3 * uu * t * p1.y() + 3 * u * tt * p2.y() + ttt * p3.y();
    
    return QgsPointXY(x, y);
}

QList<QgsPointXYZ> CAIPathGenerator::smoothPath(const QList<QgsPointXYZ> &roughPath, int segments) const
{
    if (roughPath.size() < 4 || segments < 1) {
        return roughPath;
    }
    
    QList<QgsPointXYZ> smoothed;
    
    // Add first point
    smoothed.append(roughPath.first());
    
    // Smooth using Catmull-Rom-like interpolation
    for (int i = 0; i < roughPath.size() - 1; i++) {
        QgsPointXYZ p0 = roughPath[qMax(0, i - 1)];
        QgsPointXYZ p1 = roughPath[i];
        QgsPointXYZ p2 = roughPath[qMin(roughPath.size() - 1, i + 1)];
        QgsPointXYZ p3 = roughPath[qMin(roughPath.size() - 1, i + 2)];
        
        for (int j = 1; j <= segments; j++) {
            double t = static_cast<double>(j) / (segments + 1);
            
            // Simple smoothing - weighted average
            double x = p1.x() + t * (p2.x() - p1.x());
            double y = p1.y() + t * (p2.y() - p1.y());
            double z = p1.z() + t * (p2.z() - p1.z());
            
            // Add curvature influence from neighbors
            double curveInfluence = 0.1;
            x += curveInfluence * (0.5 * (p0.x() + p3.x()) - 0.5 * (p1.x() + p2.x())) * std::sin(t * M_PI);
            y += curveInfluence * (0.5 * (p0.y() + p3.y()) - 0.5 * (p1.y() + p2.y())) * std::sin(t * M_PI);
            
            smoothed.append(QgsPointXYZ(x, y, z));
        }
    }
    
    // Add last point
    smoothed.append(roughPath.last());
    
    return smoothed;
}

// ============ External AI Integration ============

void CAIPathGenerator::callExternalAI(const QgsPointXY &start, const QgsPointXY &end, 
                                       const AIPathParameters &params)
{
    if (_m_apiEndpoint.isEmpty() || _m_apiKey.isEmpty()) {
        emit signalAIPathGenerated(QList<QgsPointXYZ>(), false, "API not configured");
        return;
    }
    
    emit signalGenerationProgress(10, "Connecting to AI service...");
    
    // Build prompt for LLM
    QString prompt = QString(
        "Generate a flight path for a %1 mission.\n"
        "Start: lat=%2, lon=%3\n"
        "End: lat=%4, lon=%5\n"
        "Altitude range: %6m - %7m\n"
        "Number of waypoints: %8\n"
        "Constraints: %9\n"
        "Return JSON array of waypoints: [{\"lat\": x, \"lon\": y, \"alt\": z}, ...]"
    ).arg(getMissionTypeName(params.missionType))
     .arg(start.y()).arg(start.x())
     .arg(end.y()).arg(end.x())
     .arg(params.minAltitude).arg(params.maxAltitude)
     .arg(params.numWaypoints)
     .arg(params.avoidDetection ? "Avoid detection" : "None");
    
    QJsonObject requestBody;
    requestBody["prompt"] = prompt;
    requestBody["max_tokens"] = 2000;
    
    QNetworkRequest request(QUrl(_m_apiEndpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(_m_apiKey).toUtf8());
    
    _m_networkManager->post(request, QJsonDocument(requestBody).toJson());
}

void CAIPathGenerator::onNetworkReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit signalAIPathGenerated(QList<QgsPointXYZ>(), false, reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    
    QList<QgsPointXYZ> path = parseAIResponse(doc);
    
    bool success = !path.isEmpty();
    emit signalAIPathGenerated(path, success, success ? "" : "Failed to parse AI response");
    
    reply->deleteLater();
}

QList<QgsPointXYZ> CAIPathGenerator::parseAIResponse(const QJsonDocument &response) const
{
    QList<QgsPointXYZ> path;
    
    // Parse the waypoints from AI response
    // Format expected: {"waypoints": [{"lat": x, "lon": y, "alt": z}, ...]}
    
    QJsonObject obj = response.object();
    QJsonArray waypoints;
    
    // Try different response formats
    if (obj.contains("waypoints")) {
        waypoints = obj["waypoints"].toArray();
    } else if (obj.contains("choices")) {
        // OpenAI format
        QJsonArray choices = obj["choices"].toArray();
        if (!choices.isEmpty()) {
            QString content = choices[0].toObject()["message"].toObject()["content"].toString();
            // Parse JSON from content
            QJsonDocument contentDoc = QJsonDocument::fromJson(content.toUtf8());
            waypoints = contentDoc.array();
        }
    }
    
    for (const QJsonValue &val : waypoints) {
        QJsonObject wp = val.toObject();
        double lat = wp["lat"].toDouble();
        double lon = wp["lon"].toDouble();
        double alt = wp["alt"].toDouble(1000.0);
        
        path.append(QgsPointXYZ(lon, lat, alt));
    }
    
    return path;
}
