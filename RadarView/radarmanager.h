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
    int                 radarId;
    QString             radarName;
    bool                active;
    double              maxRange;       // km – kept as convenience shortcut mirroring design.maxRangeKm
    QVector<Detection>  detections;
    RadarAttributes     attributes;     // full structured attribute set
};

} // namespace RadarView

class RadarManager : public QObject
{
    Q_OBJECT

public:
    explicit RadarManager(QObject *parent = nullptr);

    const QVector<RadarView::Radar>& radars() const { return m_radars; }

    const RadarView::Radar* radarById(int id) const;
    RadarView::Radar*       radarById(int id);

    // Add a radar entry derived from a scenario object placed on the map.
    // radarId    – unique integer id (e.g. sequential index)
    // name       – display name (typically the object's ID string)
    // maxRangeKm – detection radius in km (defaults to 150 km if 0)
    void addRadar(int radarId, const QString &name, double maxRangeKm = 150.0);

    // Add/update a radar with a fully specified attribute set.
    void addRadar(int radarId, const QString &name,
                  const RadarView::RadarAttributes &attrs);

    // Update attributes for an existing radar (no-op if id not found).
    void updateAttributes(int radarId, const RadarView::RadarAttributes &attrs);

    // Remove a radar by id (e.g. when scenario is reset).
    void removeRadar(int radarId);

    // Clear all radars (called on scenario reset).
    void clearAll();

signals:
    void radarDataChanged();
    void radarAttributesChanged(int radarId);

private:
    QVector<RadarView::Radar> m_radars;
    int m_nextId = 1;
};

#endif // RADARMANAGER_H
