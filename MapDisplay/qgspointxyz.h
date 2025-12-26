#ifndef QGSPOINTXYZ_H
#define QGSPOINTXYZ_H

#include <qgspointxy.h>


class QgsPointXYZ : public QgsPointXY
{
public:
    QgsPointXYZ(QgsPointXY pt, double z);
    QgsPointXYZ(double x, double y, double z);


    double z();
private :
    double _m_z;
};

#endif // QGSPOINTXYZ_H
