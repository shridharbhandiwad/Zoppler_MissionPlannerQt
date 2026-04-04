#include "radarmanager.h"
#include <cmath>

RadarManager::RadarManager(QObject *parent)
    : QObject(parent)
{
    loadSampleData();
}

const RadarView::Radar* RadarManager::radarById(int id) const
{
    for (const auto &r : m_radars) {
        if (r.radarId == id)
            return &r;
    }
    return nullptr;
}

void RadarManager::loadSampleData()
{
    using namespace RadarView;

    // ---- Radar 1: Long-range surveillance ----
    Radar r1;
    r1.radarId   = 1;
    r1.radarName = "Alpha Surveillance";
    r1.active    = true;
    r1.maxRange  = 200.0;

    // Spread detections across multiple quadrants
    struct SeedDetection { int id; double range; double az; double el; };
    QVector<SeedDetection> seeds1 = {
        {101,  45.0,  30.0,  5.2},
        {102,  90.0,  75.0,  3.1},
        {103, 130.0, 145.0,  8.7},
        {104,  60.0, 200.0,  2.0},
        {105, 170.0, 310.0, 12.5},
        {106,  25.0, 355.0,  1.8},
        {107, 110.0, 260.0,  6.3},
        {108,  80.0,  15.0,  4.0},
    };
    for (const auto &s : seeds1)
        r1.detections.push_back({s.id, s.range, s.az, s.el});

    // ---- Radar 2: Short-range fire-control ----
    Radar r2;
    r2.radarId   = 2;
    r2.radarName = "Bravo Fire-Control";
    r2.active    = true;
    r2.maxRange  = 50.0;

    QVector<SeedDetection> seeds2 = {
        {201,  10.0,  45.0,  20.0},
        {202,  22.0, 120.0,  15.5},
        {203,  35.0, 190.0,   9.0},
        {204,  42.0, 270.0,  30.0},
        {205,   8.0, 330.0,  25.0},
    };
    for (const auto &s : seeds2)
        r2.detections.push_back({s.id, s.range, s.az, s.el});

    // ---- Radar 3: Inactive coastal radar ----
    Radar r3;
    r3.radarId   = 3;
    r3.radarName = "Charlie Coastal";
    r3.active    = false;
    r3.maxRange  = 120.0;
    // No detections (inactive)

    // ---- Radar 4: Medium-range tracking ----
    Radar r4;
    r4.radarId   = 4;
    r4.radarName = "Delta Tracking";
    r4.active    = true;
    r4.maxRange  = 100.0;

    QVector<SeedDetection> seeds4 = {
        {401,  15.0,  60.0,  7.0},
        {402,  55.0, 130.0, 11.0},
        {403,  70.0, 220.0,  4.5},
        {404,  88.0, 300.0,  6.2},
        {405,  30.0,  10.0, 18.0},
        {406,  95.0, 170.0,  2.8},
    };
    for (const auto &s : seeds4)
        r4.detections.push_back({s.id, s.range, s.az, s.el});

    m_radars = { r1, r2, r3, r4 };

    emit radarDataChanged();
}
