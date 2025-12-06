#include "qgspointxyz.h"

QgsPointXYZ::QgsPointXYZ()
    : QgsPoint(), mZ(0.0)
{
}

QgsPointXYZ::QgsPointXYZ(double x, double y, double z)
    : QgsPoint(x, y), mZ(z)
{
}
