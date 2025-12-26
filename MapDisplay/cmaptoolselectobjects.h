#ifndef CMAPTOOLSELECTOBJECTS_H
#define CMAPTOOLSELECTOBJECTS_H

#include <qgsmapcanvas.h>
#include <qgsmaptoolidentifyfeature.h>
#include <qgsvectorlayer.h>
#include <qgsfeature.h>
#include <qgsvectorlayerselectionmanager.h>
#include <qgsmapmouseevent.h>

class CMapToolSelectObjects : public QgsMapToolIdentifyFeature
{
public:
    CMapToolSelectObjects(QgsMapCanvas *canvas, QgsVectorLayer *layer);

protected:
    void canvasReleaseEvent(QgsMapMouseEvent *e) override;


private :
    QgsVectorLayer *mLayer;
};

#endif // CMAPTOOLSELECTOBJECTS_H
