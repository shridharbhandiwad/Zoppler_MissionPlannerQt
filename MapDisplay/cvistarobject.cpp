#include "cvistarobject.h"

CVistarObject::CVistarObject()
    : m_id("")
    , m_type("")
    , m_position(0, 0, 0)
{
}

CVistarObject::CVistarObject(const QString &id, const QString &type, const QgsPointXYZ &position)
    : m_id(id)
    , m_type(type)
    , m_position(position)
{
}

CVistarObject::~CVistarObject()
{
}

QJsonObject CVistarObject::toJson() const
{
    QJsonObject obj;
    obj["ID"] = m_id;
    obj["TYPE"] = m_type;
    obj["LAT"] = m_position.y();
    obj["LON"] = m_position.x();
    obj["ALT"] = m_position.z();
    
    // Merge additional data
    for (auto it = m_additionalData.begin(); it != m_additionalData.end(); ++it) {
        obj[it.key()] = it.value();
    }
    
    return obj;
}

CVistarObject CVistarObject::fromJson(const QJsonObject &json)
{
    QString id = json["ID"].toString();
    QString type = json["TYPE"].toString();
    double lat = json["LAT"].toDouble();
    double lon = json["LON"].toDouble();
    double alt = json["ALT"].toDouble();
    
    return CVistarObject(id, type, QgsPointXYZ(lon, lat, alt));
}
