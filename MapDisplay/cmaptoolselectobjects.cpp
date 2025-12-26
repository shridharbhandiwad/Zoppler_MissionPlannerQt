#include "cmaptoolselectobjects.h"

CMapToolSelectObjects::CMapToolSelectObjects(QgsMapCanvas *canvas, QgsVectorLayer *layer)
    : QgsMapToolIdentifyFeature(canvas, layer)
    , mLayer(layer){


}

void CMapToolSelectObjects::canvasReleaseEvent(QgsMapMouseEvent *e)
{
    // // Identify clicked feature
    // const QList<QgsMapToolIdentify::IdentifyResult> results = identify(e->x(), e->y(),IdentifyMode::De LayerSelection::AllLayers);

    // if (!results.isEmpty()) {
    //     auto feature = results.first().mFeature;
    //     QgsFeatureId fid = feature.id();

    //     // Clear previous selection and select this feature
    //     mLayer->removeSelection();
    //     mLayer->select(fid);

    //     qDebug() << "Selected feature ID:" << fid;
    // }
    // else {
    //     // Clicked on empty area — clear selection
    //     mLayer->removeSelection();
    //     qDebug() << "No feature clicked";
    // }
}
