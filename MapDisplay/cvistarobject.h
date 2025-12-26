#ifndef CVISTAROBJECT_H
#define CVISTAROBJECT_H

#include <QString>
#include <QJsonObject>
#include "qgspointxyz.h"

class CVistarObject
{
public:
    CVistarObject();
    CVistarObject(const QString &id, const QString &type, const QgsPointXYZ &position);
    ~CVistarObject();
    
    QString id() const { return m_id; }
    QString type() const { return m_type; }
    QgsPointXYZ position() const { return m_position; }
    
    void setId(const QString &id) { m_id = id; }
    void setType(const QString &type) { m_type = type; }
    void setPosition(const QgsPointXYZ &position) { m_position = position; }
    
    QJsonObject toJson() const;
    static CVistarObject fromJson(const QJsonObject &json);
    
private:
    QString m_id;
    QString m_type;
    QgsPointXYZ m_position;
    QJsonObject m_additionalData;
};

#endif // CVISTAROBJECT_H
