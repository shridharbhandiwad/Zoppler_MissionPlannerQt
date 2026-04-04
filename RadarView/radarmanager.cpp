#include "radarmanager.h"

RadarManager::RadarManager(QObject *parent)
    : QObject(parent)
{
    // No sample data – radars are populated from actual scenario objects.
}

const RadarView::Radar* RadarManager::radarById(int id) const
{
    for (const auto &r : m_radars) {
        if (r.radarId == id)
            return &r;
    }
    return nullptr;
}

void RadarManager::addRadar(int radarId, const QString &name, double maxRangeKm)
{
    // Avoid duplicates
    for (const auto &r : m_radars) {
        if (r.radarId == radarId)
            return;
    }

    RadarView::Radar radar;
    radar.radarId   = radarId;
    radar.radarName = name;
    radar.active    = true;
    radar.maxRange  = (maxRangeKm > 0.0) ? maxRangeKm : 150.0;

    m_radars.append(radar);
    emit radarDataChanged();
}

void RadarManager::removeRadar(int radarId)
{
    for (int i = 0; i < m_radars.size(); ++i) {
        if (m_radars[i].radarId == radarId) {
            m_radars.removeAt(i);
            emit radarDataChanged();
            return;
        }
    }
}

void RadarManager::clearAll()
{
    if (!m_radars.isEmpty()) {
        m_radars.clear();
        m_nextId = 1;
        emit radarDataChanged();
    }
}
