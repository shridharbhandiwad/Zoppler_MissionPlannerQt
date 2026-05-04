#ifndef RADARATTRIBUTES_H
#define RADARATTRIBUTES_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

namespace RadarView {

// ---------------------------------------------------------------------------
// RadarAttributes – structured attribute set for a single radar system.
//
// Contains only the RadarPhysicsParameters (RF/signal-processing values
// used by the radar equation and simulation backend).
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// RadarPhysicsParameters – RF/signal-processing parameters transmitted in
// the "parameters" block of every radar create message sent to the backend.
// These are simulation-oriented values derived from the radar equation.
// ---------------------------------------------------------------------------
struct RadarPhysicsParameters
{
    // Detection performance
    double Pd            = 0.9;      // probability of detection
    double Pfa           = 1e-6;     // probability of false alarm

    // Antenna beamwidth
    double beamwidth_az  = 3.0;      // azimuth beamwidth (degrees)
    double beamwidth_el  = 4.0;      // elevation beamwidth (degrees)

    // Transmitter
    double tx_power      = 1000.0;   // transmit power (W)
    double pulse_width   = 100e-6;   // pulse width (s)
    double bandwidth     = 5e6;      // signal bandwidth (Hz)

    // Frequency plan
    double freq_min      = 10.0e9;   // minimum frequency (Hz)
    double freq_max      = 10.5e9;   // maximum frequency (Hz)
    double freq_center   = 10.25e9;  // centre frequency (Hz) – (freq_min+freq_max)/2

    // Link budget
    double desired_snr   = 17.0;     // required SNR at detection threshold (dB)
    double noise_figure  = 3.0;      // receiver noise figure (dB)
    double system_temp   = 290.0;    // system noise temperature (K)
    double receiver_gain = 50.0;     // receiver gain (dB)

    // Scan
    double prf           = 1000.0;   // pulse repetition frequency (Hz)
    double scan_time     = 2.0;      // time for one full scan (s)

    // Target / loss
    double rcs           = 2.0;      // reference target RCS (m²)
    double loss          = 6.0;      // total system loss (dB)

    static RadarPhysicsParameters defaults() { return RadarPhysicsParameters{}; }

    // Recompute freq_center from freq_min/freq_max before serialising
    void updateDerivedFields()
    {
        freq_center = (freq_min + freq_max) / 2.0;
    }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["Pd"]            = Pd;
        o["Pfa"]           = Pfa;
        o["beamwidth_az"]  = beamwidth_az;
        o["beamwidth_el"]  = beamwidth_el;
        o["tx_power"]      = tx_power;
        o["pulse_width"]   = pulse_width;
        o["bandwidth"]     = bandwidth;
        o["freq_min"]      = freq_min;
        o["freq_max"]      = freq_max;
        o["freq_center"]   = (freq_min + freq_max) / 2.0;
        o["desired_snr"]   = desired_snr;
        o["noise_figure"]  = noise_figure;
        o["system_temp"]   = system_temp;
        o["receiver_gain"] = receiver_gain;
        o["prf"]           = prf;
        o["scan_time"]     = scan_time;
        o["rcs"]           = rcs;
        o["loss"]          = loss;
        return o;
    }

    static RadarPhysicsParameters fromJson(const QJsonObject &o)
    {
        RadarPhysicsParameters p;
        if (o.contains("Pd"))            p.Pd            = o["Pd"].toDouble(p.Pd);
        if (o.contains("Pfa"))           p.Pfa           = o["Pfa"].toDouble(p.Pfa);
        if (o.contains("beamwidth_az"))  p.beamwidth_az  = o["beamwidth_az"].toDouble(p.beamwidth_az);
        if (o.contains("beamwidth_el"))  p.beamwidth_el  = o["beamwidth_el"].toDouble(p.beamwidth_el);
        if (o.contains("tx_power"))      p.tx_power      = o["tx_power"].toDouble(p.tx_power);
        if (o.contains("pulse_width"))   p.pulse_width   = o["pulse_width"].toDouble(p.pulse_width);
        if (o.contains("bandwidth"))     p.bandwidth     = o["bandwidth"].toDouble(p.bandwidth);
        if (o.contains("freq_min"))      p.freq_min      = o["freq_min"].toDouble(p.freq_min);
        if (o.contains("freq_max"))      p.freq_max      = o["freq_max"].toDouble(p.freq_max);
        if (o.contains("freq_center"))   p.freq_center   = o["freq_center"].toDouble(p.freq_center);
        if (o.contains("desired_snr"))   p.desired_snr   = o["desired_snr"].toDouble(p.desired_snr);
        if (o.contains("noise_figure"))  p.noise_figure  = o["noise_figure"].toDouble(p.noise_figure);
        if (o.contains("system_temp"))   p.system_temp   = o["system_temp"].toDouble(p.system_temp);
        if (o.contains("receiver_gain")) p.receiver_gain = o["receiver_gain"].toDouble(p.receiver_gain);
        if (o.contains("prf"))           p.prf           = o["prf"].toDouble(p.prf);
        if (o.contains("scan_time"))     p.scan_time     = o["scan_time"].toDouble(p.scan_time);
        if (o.contains("rcs"))           p.rcs           = o["rcs"].toDouble(p.rcs);
        if (o.contains("loss"))          p.loss          = o["loss"].toDouble(p.loss);
        return p;
    }
};

// ---------------------------------------------------------------------------
// RadarCoverageParameters – operational coverage / display parameters.
//
// These control the geographic range rings drawn on the map canvas and the
// azimuth / elevation sector shown in the PPI view.
// ---------------------------------------------------------------------------
struct RadarCoverageParameters
{
    double maxRangeKm      = 100.0;  // maximum detection range (km)
    double minAzimuthDeg   = 0.0;    // minimum azimuth of coverage sector (deg)
    double maxAzimuthDeg   = 360.0;  // maximum azimuth of coverage sector (deg)
    double minElevationDeg = 0.0;    // minimum elevation angle (deg)
    double maxElevationDeg = 70.0;   // maximum elevation angle (deg)
    double rangeRingSpacingKm = 10.0; // spacing between range rings on map (km)

    static RadarCoverageParameters defaults() { return RadarCoverageParameters{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["maxRangeKm"]          = maxRangeKm;
        o["minAzimuthDeg"]       = minAzimuthDeg;
        o["maxAzimuthDeg"]       = maxAzimuthDeg;
        o["minElevationDeg"]     = minElevationDeg;
        o["maxElevationDeg"]     = maxElevationDeg;
        o["rangeRingSpacingKm"]  = rangeRingSpacingKm;
        return o;
    }

    static RadarCoverageParameters fromJson(const QJsonObject &o)
    {
        RadarCoverageParameters p;
        if (o.contains("maxRangeKm"))         p.maxRangeKm         = o["maxRangeKm"].toDouble(p.maxRangeKm);
        if (o.contains("minAzimuthDeg"))       p.minAzimuthDeg      = o["minAzimuthDeg"].toDouble(p.minAzimuthDeg);
        if (o.contains("maxAzimuthDeg"))       p.maxAzimuthDeg      = o["maxAzimuthDeg"].toDouble(p.maxAzimuthDeg);
        if (o.contains("minElevationDeg"))     p.minElevationDeg    = o["minElevationDeg"].toDouble(p.minElevationDeg);
        if (o.contains("maxElevationDeg"))     p.maxElevationDeg    = o["maxElevationDeg"].toDouble(p.maxElevationDeg);
        if (o.contains("rangeRingSpacingKm"))  p.rangeRingSpacingKm = o["rangeRingSpacingKm"].toDouble(p.rangeRingSpacingKm);
        return p;
    }
};

// Top-level container – holds only the radar physics / simulation parameters.
struct RadarAttributes
{
    RadarPhysicsParameters  physics;
    RadarCoverageParameters coverage;

    static RadarAttributes defaults() { return RadarAttributes{}; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["physics"]  = physics.toJson();
        o["coverage"] = coverage.toJson();
        return o;
    }

    static RadarAttributes fromJson(const QJsonObject &o)
    {
        RadarAttributes a;
        if (o.contains("physics"))  a.physics  = RadarPhysicsParameters::fromJson(o["physics"].toObject());
        if (o.contains("coverage")) a.coverage = RadarCoverageParameters::fromJson(o["coverage"].toObject());
        return a;
    }
};

} // namespace RadarView

#endif // RADARATTRIBUTES_H
