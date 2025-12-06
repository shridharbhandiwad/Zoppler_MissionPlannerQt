#ifndef CVISTAROBJECT_H
#define CVISTAROBJECT_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrenderer.h>
#include <qgsmarkersymbol.h>
#include <qgssymbollayer.h>
#include "globalConstants.h"
#include "qgspointxyz.h"

class CVistarObject : public QObject
{
    Q_OBJECT
public:
    explicit CVistarObject(QgsMapCanvas *pMapCanvas, QObject *parent = nullptr);
    ~CVistarObject();

    void Update(const QJsonObject &jsonData);
    void SetObjectID(const QString &strID);
    void SetObjectClass(int nClass);
    void SetPosition(double dLat, double dLon, double dAlt = 0.0);
    void SetHeading(double dHeading);
    void SetVelocity(double dVelocity);
    
    QString GetObjectID() const { return m_strObjectID; }
    int GetObjectClass() const { return m_nObjectClass; }
    QgsPointXYZ GetPosition() const { return m_position; }
    
    void Show();
    void Hide();
    bool IsVisible() const { return m_bVisible; }

private:
    void _createLayer();
    void _updateLayer();
    QColor _getColorForClass(int nClass);
    QString _getIconForClass(int nClass);
    
private:
    QgsMapCanvas *m_pMapCanvas;
    QgsVectorLayer *m_pLayer;
    
    QString m_strObjectID;
    int m_nObjectClass;
    QgsPointXYZ m_position;
    double m_dHeading;
    double m_dVelocity;
    bool m_bVisible;
    
signals:
};

#endif // CVISTAROBJECT_H
