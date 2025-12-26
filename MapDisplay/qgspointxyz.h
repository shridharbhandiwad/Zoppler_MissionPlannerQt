#ifndef QGSPOINTXYZ_H
#define QGSPOINTXYZ_H

#include <qgspointxy.h>

class QgsPointXYZ : public QgsPointXY
{
public:
    QgsPointXYZ() : QgsPointXY(), m_z(0.0) {}
    QgsPointXYZ(double x, double y, double z) : QgsPointXY(x, y), m_z(z) {}
    
    double z() const { return m_z; }
    void setZ(double z) { m_z = z; }
    
private:
    double m_z;
};

#endif // QGSPOINTXYZ_H
