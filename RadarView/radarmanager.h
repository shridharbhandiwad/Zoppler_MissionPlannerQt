#ifndef RADARMANAGER_H
#define RADARMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>

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
    double              maxRange;   // km
    QVector<Detection>  detections;
};

} // namespace RadarView

class RadarManager : public QObject
{
    Q_OBJECT

public:
    explicit RadarManager(QObject *parent = nullptr);

    const QVector<RadarView::Radar>& radars() const { return m_radars; }

    const RadarView::Radar* radarById(int id) const;

    void loadSampleData();

signals:
    void radarDataChanged();

private:
    QVector<RadarView::Radar> m_radars;
};

#endif // RADARMANAGER_H
