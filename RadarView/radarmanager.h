#ifndef RADARMANAGER_H
#define RADARMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include "radarattributes.h"

namespace RadarView {

struct Detection
{
    int     trackId;
    double  range;      // km
    double  azimuth;    // degrees (0–360)
    double  elevation;  // degrees
};

struct Radar
{
    int                      radarId;
    QString                  radarName;
    bool                     active;
    double                   maxRange;       // km (kept for backward compat; mirrors coverage.maxRangeKm)
    QVector<Detection>       detections;
    RadarCoverageParameters  coverage;       // operational coverage / display parameters
};

} // namespace RadarView

class RadarManager : public QObject
{
    Q_OBJECT

public:
    explicit RadarManager(QObject *parent = nullptr);

    const QVector<RadarView::Radar>& radars() const { return m_radars; }

    const RadarView::Radar* radarById(int id) const;

    // Add a radar entry derived from a scenario object placed on the map.
    // radarId   – unique integer id (e.g. sequential index)
    // name      – display name (typically the object's ID string)
    // coverage  – operational coverage parameters (defaults applied if omitted)
    void addRadar(int radarId, const QString &name,
                  const RadarCoverageParameters &coverage = RadarCoverageParameters{});

    // Remove a radar by id (e.g. when scenario is reset).
    void removeRadar(int radarId);

    // Clear all radars (called on scenario reset).
    void clearAll();

    // Update a radar's max range from the design attributes (called when operator edits attributes).
    void updateRadarRange(int radarId, double maxRangeKm);

    // Update a radar's coverage parameters (called when operator edits coverage via dialog).
    void updateRadarCoverage(int radarId, const RadarCoverageParameters &coverage);

signals:
    void radarDataChanged();

private:
    QVector<RadarView::Radar> m_radars;
    int m_nextId = 1;
};

#endif // RADARMANAGER_H
