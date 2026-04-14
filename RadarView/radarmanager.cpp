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

RadarView::Radar* RadarManager::radarById(int id)
{
    for (auto &r : m_radars) {
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

    // Initialise attributes with defaults; sync maxRange into design
    radar.attributes                    = RadarView::RadarAttributes::defaults();
    radar.attributes.design.maxRangeKm  = radar.maxRange;

    m_radars.append(radar);
    emit radarDataChanged();
}

void RadarManager::addRadar(int radarId, const QString &name,
                             const RadarView::RadarAttributes &attrs)
{
    for (const auto &r : m_radars) {
        if (r.radarId == radarId)
            return;
    }

    RadarView::Radar radar;
    radar.radarId    = radarId;
    radar.radarName  = name;
    radar.active     = (attrs.operational.operationalMode != "Off");
    radar.maxRange   = attrs.design.maxRangeKm;
    radar.attributes = attrs;

    m_radars.append(radar);
    emit radarDataChanged();
}

void RadarManager::updateAttributes(int radarId, const RadarView::RadarAttributes &attrs)
{
    for (auto &r : m_radars) {
        if (r.radarId == radarId) {
            r.attributes = attrs;
            // Keep convenience fields in sync
            r.maxRange  = attrs.design.maxRangeKm;
            r.active    = (attrs.operational.operationalMode != "Off");
            emit radarAttributesChanged(radarId);
            emit radarDataChanged();
            return;
        }
    }
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
