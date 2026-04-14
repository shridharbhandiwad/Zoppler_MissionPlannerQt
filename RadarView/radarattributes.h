#ifndef RADARATTRIBUTES_H
#define RADARATTRIBUTES_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

namespace RadarView {

// ---------------------------------------------------------------------------
// RadarAttributes – structured attribute set for a single radar system.
//
// Three sub-categories match the three tabs shown in the attribute editor:
//   1. DesignAttributes      – static / configuration parameters
//   2. OperationalAttributes – live / mission-time parameters
//   3. MaintenanceAttributes – health / logistics parameters
//
// All fields carry a meaningful default so a newly-placed radar is
// immediately usable without manual editing.
// ---------------------------------------------------------------------------

struct DesignAttributes
{
    // Identification
    QString radarType       = "Surveillance";  // Surveillance | Fire Control | Weather | SAR | Tracking
    QString manufacturer    = "Unknown";
    QString modelDesignation= "";

    // RF parameters
    double  frequencyMHz    = 3000.0;   // MHz  (S-band default)
    double  bandwidthMHz    = 10.0;     // MHz
    QString polarization    = "Linear"; // Linear | Circular | Dual

    // Geometry
    double  maxRangeKm      = 150.0;    // km
    double  minRangeKm      = 0.5;      // km
    double  azimuthCovDeg   = 360.0;    // degrees (360 = full)
    double  elevationMinDeg = 0.0;      // degrees
    double  elevationMaxDeg = 70.0;     // degrees

    // Sensitivity & precision
    double  peakPowerKw     = 100.0;    // kW
    double  rangeResolutionM= 50.0;     // metres
    double  azimuthResolutionDeg = 0.5; // degrees

    // Physical
    double  antennaHeightM  = 10.0;     // metres above ground
    double  weightKg        = 2500.0;
    QString mountingType    = "Fixed";  // Fixed | Mobile | Ship-borne | Airborne

    static DesignAttributes defaults() { return DesignAttributes{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["radarType"]              = radarType;
        o["manufacturer"]           = manufacturer;
        o["modelDesignation"]       = modelDesignation;
        o["frequencyMHz"]           = frequencyMHz;
        o["bandwidthMHz"]           = bandwidthMHz;
        o["polarization"]           = polarization;
        o["maxRangeKm"]             = maxRangeKm;
        o["minRangeKm"]             = minRangeKm;
        o["azimuthCovDeg"]          = azimuthCovDeg;
        o["elevationMinDeg"]        = elevationMinDeg;
        o["elevationMaxDeg"]        = elevationMaxDeg;
        o["peakPowerKw"]            = peakPowerKw;
        o["rangeResolutionM"]       = rangeResolutionM;
        o["azimuthResolutionDeg"]   = azimuthResolutionDeg;
        o["antennaHeightM"]         = antennaHeightM;
        o["weightKg"]               = weightKg;
        o["mountingType"]           = mountingType;
        return o;
    }

    static DesignAttributes fromJson(const QJsonObject &o)
    {
        DesignAttributes d;
        if (o.contains("radarType"))            d.radarType             = o["radarType"].toString(d.radarType);
        if (o.contains("manufacturer"))         d.manufacturer          = o["manufacturer"].toString(d.manufacturer);
        if (o.contains("modelDesignation"))     d.modelDesignation      = o["modelDesignation"].toString(d.modelDesignation);
        if (o.contains("frequencyMHz"))         d.frequencyMHz          = o["frequencyMHz"].toDouble(d.frequencyMHz);
        if (o.contains("bandwidthMHz"))         d.bandwidthMHz          = o["bandwidthMHz"].toDouble(d.bandwidthMHz);
        if (o.contains("polarization"))         d.polarization          = o["polarization"].toString(d.polarization);
        if (o.contains("maxRangeKm"))           d.maxRangeKm            = o["maxRangeKm"].toDouble(d.maxRangeKm);
        if (o.contains("minRangeKm"))           d.minRangeKm            = o["minRangeKm"].toDouble(d.minRangeKm);
        if (o.contains("azimuthCovDeg"))        d.azimuthCovDeg         = o["azimuthCovDeg"].toDouble(d.azimuthCovDeg);
        if (o.contains("elevationMinDeg"))      d.elevationMinDeg       = o["elevationMinDeg"].toDouble(d.elevationMinDeg);
        if (o.contains("elevationMaxDeg"))      d.elevationMaxDeg       = o["elevationMaxDeg"].toDouble(d.elevationMaxDeg);
        if (o.contains("peakPowerKw"))          d.peakPowerKw           = o["peakPowerKw"].toDouble(d.peakPowerKw);
        if (o.contains("rangeResolutionM"))     d.rangeResolutionM      = o["rangeResolutionM"].toDouble(d.rangeResolutionM);
        if (o.contains("azimuthResolutionDeg")) d.azimuthResolutionDeg  = o["azimuthResolutionDeg"].toDouble(d.azimuthResolutionDeg);
        if (o.contains("antennaHeightM"))       d.antennaHeightM        = o["antennaHeightM"].toDouble(d.antennaHeightM);
        if (o.contains("weightKg"))             d.weightKg              = o["weightKg"].toDouble(d.weightKg);
        if (o.contains("mountingType"))         d.mountingType          = o["mountingType"].toString(d.mountingType);
        return d;
    }
};

struct OperationalAttributes
{
    // Status
    QString operationalMode  = "Standby";    // Standby | Active | Maintenance | Degraded | Off
    QString assignedMission  = "";
    int     trackCapacity    = 100;           // max simultaneous tracks
    int     currentTracks    = 0;

    // Emission control
    bool    emconActive      = false;         // emission control (silent)
    double  transmitPowerPct = 100.0;         // % of peak power
    double  scanRateDegPerSec= 6.0;           // degrees/second (60 RPM = 360°/s → 6°/s is one scan per min)

    // Coverage sector override (–1 = use design default)
    double  sectorStartDeg   = 0.0;
    double  sectorEndDeg     = 360.0;

    // IFF
    bool    iffEnabled       = true;
    QString iffMode          = "Mode-3";     // Mode-1 | Mode-2 | Mode-3 | Mode-S

    // Jamming / interference
    bool    jammingDetected  = false;
    double  jamSignalDbm     = -999.0;

    static OperationalAttributes defaults() { return OperationalAttributes{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["operationalMode"]    = operationalMode;
        o["assignedMission"]    = assignedMission;
        o["trackCapacity"]      = trackCapacity;
        o["currentTracks"]      = currentTracks;
        o["emconActive"]        = emconActive;
        o["transmitPowerPct"]   = transmitPowerPct;
        o["scanRateDegPerSec"]  = scanRateDegPerSec;
        o["sectorStartDeg"]     = sectorStartDeg;
        o["sectorEndDeg"]       = sectorEndDeg;
        o["iffEnabled"]         = iffEnabled;
        o["iffMode"]            = iffMode;
        o["jammingDetected"]    = jammingDetected;
        o["jamSignalDbm"]       = jamSignalDbm;
        return o;
    }

    static OperationalAttributes fromJson(const QJsonObject &o)
    {
        OperationalAttributes op;
        if (o.contains("operationalMode"))   op.operationalMode  = o["operationalMode"].toString(op.operationalMode);
        if (o.contains("assignedMission"))   op.assignedMission  = o["assignedMission"].toString(op.assignedMission);
        if (o.contains("trackCapacity"))     op.trackCapacity    = o["trackCapacity"].toInt(op.trackCapacity);
        if (o.contains("currentTracks"))     op.currentTracks    = o["currentTracks"].toInt(op.currentTracks);
        if (o.contains("emconActive"))       op.emconActive      = o["emconActive"].toBool(op.emconActive);
        if (o.contains("transmitPowerPct"))  op.transmitPowerPct = o["transmitPowerPct"].toDouble(op.transmitPowerPct);
        if (o.contains("scanRateDegPerSec")) op.scanRateDegPerSec= o["scanRateDegPerSec"].toDouble(op.scanRateDegPerSec);
        if (o.contains("sectorStartDeg"))    op.sectorStartDeg   = o["sectorStartDeg"].toDouble(op.sectorStartDeg);
        if (o.contains("sectorEndDeg"))      op.sectorEndDeg     = o["sectorEndDeg"].toDouble(op.sectorEndDeg);
        if (o.contains("iffEnabled"))        op.iffEnabled       = o["iffEnabled"].toBool(op.iffEnabled);
        if (o.contains("iffMode"))           op.iffMode          = o["iffMode"].toString(op.iffMode);
        if (o.contains("jammingDetected"))   op.jammingDetected  = o["jammingDetected"].toBool(op.jammingDetected);
        if (o.contains("jamSignalDbm"))      op.jamSignalDbm     = o["jamSignalDbm"].toDouble(op.jamSignalDbm);
        return op;
    }
};

struct MaintenanceAttributes
{
    // Health
    QString systemHealth     = "Nominal";   // Nominal | Degraded | Critical | Failed
    double  healthPct        = 100.0;       // 0–100 %

    // Component status
    bool    transmitterOk    = true;
    bool    receiverOk       = true;
    bool    antennaOk        = true;
    bool    coolingSysOk     = true;
    bool    powerSupplyOk    = true;

    // Logistics
    QString lastServiceDate  = "";
    QString nextServiceDate  = "";
    int     operatingHours   = 0;
    int     serviceIntervalH = 500;        // hours between services

    // MTBF / reliability
    double  mtbfHours        = 2000.0;     // mean time between failures
    double  mttrHours        = 4.0;        // mean time to repair

    // Notes
    QString maintenanceNotes = "";

    static MaintenanceAttributes defaults() { return MaintenanceAttributes{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["systemHealth"]       = systemHealth;
        o["healthPct"]          = healthPct;
        o["transmitterOk"]      = transmitterOk;
        o["receiverOk"]         = receiverOk;
        o["antennaOk"]          = antennaOk;
        o["coolingSysOk"]       = coolingSysOk;
        o["powerSupplyOk"]      = powerSupplyOk;
        o["lastServiceDate"]    = lastServiceDate;
        o["nextServiceDate"]    = nextServiceDate;
        o["operatingHours"]     = operatingHours;
        o["serviceIntervalH"]   = serviceIntervalH;
        o["mtbfHours"]          = mtbfHours;
        o["mttrHours"]          = mttrHours;
        o["maintenanceNotes"]   = maintenanceNotes;
        return o;
    }

    static MaintenanceAttributes fromJson(const QJsonObject &o)
    {
        MaintenanceAttributes m;
        if (o.contains("systemHealth"))     m.systemHealth     = o["systemHealth"].toString(m.systemHealth);
        if (o.contains("healthPct"))        m.healthPct        = o["healthPct"].toDouble(m.healthPct);
        if (o.contains("transmitterOk"))    m.transmitterOk    = o["transmitterOk"].toBool(m.transmitterOk);
        if (o.contains("receiverOk"))       m.receiverOk       = o["receiverOk"].toBool(m.receiverOk);
        if (o.contains("antennaOk"))        m.antennaOk        = o["antennaOk"].toBool(m.antennaOk);
        if (o.contains("coolingSysOk"))     m.coolingSysOk     = o["coolingSysOk"].toBool(m.coolingSysOk);
        if (o.contains("powerSupplyOk"))    m.powerSupplyOk    = o["powerSupplyOk"].toBool(m.powerSupplyOk);
        if (o.contains("lastServiceDate"))  m.lastServiceDate  = o["lastServiceDate"].toString(m.lastServiceDate);
        if (o.contains("nextServiceDate"))  m.nextServiceDate  = o["nextServiceDate"].toString(m.nextServiceDate);
        if (o.contains("operatingHours"))   m.operatingHours   = o["operatingHours"].toInt(m.operatingHours);
        if (o.contains("serviceIntervalH")) m.serviceIntervalH = o["serviceIntervalH"].toInt(m.serviceIntervalH);
        if (o.contains("mtbfHours"))        m.mtbfHours        = o["mtbfHours"].toDouble(m.mtbfHours);
        if (o.contains("mttrHours"))        m.mttrHours        = o["mttrHours"].toDouble(m.mttrHours);
        if (o.contains("maintenanceNotes")) m.maintenanceNotes = o["maintenanceNotes"].toString(m.maintenanceNotes);
        return m;
    }
};

// Top-level container bundling all three sub-categories.
struct RadarAttributes
{
    DesignAttributes      design;
    OperationalAttributes operational;
    MaintenanceAttributes maintenance;

    static RadarAttributes defaults() { return RadarAttributes{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["design"]      = design.toJson();
        o["operational"] = operational.toJson();
        o["maintenance"] = maintenance.toJson();
        return o;
    }

    static RadarAttributes fromJson(const QJsonObject &o)
    {
        RadarAttributes a;
        if (o.contains("design"))      a.design      = DesignAttributes::fromJson(o["design"].toObject());
        if (o.contains("operational")) a.operational = OperationalAttributes::fromJson(o["operational"].toObject());
        if (o.contains("maintenance")) a.maintenance = MaintenanceAttributes::fromJson(o["maintenance"].toObject());
        return a;
    }
};

} // namespace RadarView

#endif // RADARATTRIBUTES_H
