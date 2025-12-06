#ifndef QGSPOINTXYZ_H
#define QGSPOINTXYZ_H

#include <qgspoint.h>

class QgsPointXYZ : public QgsPoint
{
public:
    QgsPointXYZ();
    QgsPointXYZ(double x, double y, double z = 0.0);
    
    double z() const { return mZ; }
    void setZ(double z) { mZ = z; }
    
private:
    double mZ;
};

#endif // QGSPOINTXYZ_H
