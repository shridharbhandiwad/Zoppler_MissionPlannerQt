#include "qgspointxyz.h"

QgsPointXYZ::QgsPointXYZ(QgsPointXY pt, double z) :
    QgsPointXY(pt) {
    _m_z = z;
}

QgsPointXYZ::QgsPointXYZ(double x, double y, double z) :
    QgsPointXY(x,y){
    _m_z = z;
}

double QgsPointXYZ::z() {
    return _m_z;
}
