#include "cmapcanvas.h"
#include <qgsvectorlayer.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <qgsrectangle.h>
#include <QDir>
#include <QDateTime>
#include <QFileInfoList>
#include <qgscoordinatetransformcontext.h>
#include <qgsfillsymbol.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsfillsymbol.h>
#include <qgsrenderer.h>
#include <qgsproject.h>
#include <qgssymbollayerutils.h>  // sometimes contains symbol layer helpers
#include <qgsfillsymbollayer.h> // *If* it exists in your build
#include <qgslinesymbollayer.h>
#include  <QFileInfo>
#include  <QProcess>
#include "globalConstants.h"
#include "cvistarobject.h"
#include <qgsmaplayer.h>
#include <qgsproject.h>
#include <qgslayertree.h>
#include <QMenu>
#include <qgsproviderregistry.h>
#include <QOpenGLWidget>
#include <QThread>

CMapCanvas::CMapCanvas(QWidget *parent) : QgsMapCanvas(parent)
{
    _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
    _m_scenarioManager = new CScenarioManager(this);
    
    QgsRectangle fixedWorldExtent(-180.0, -90.0, 180.0, 90.0);
     mPreviousCursor = Qt::ArrowCursor;
    // Add padding (e.g., 10% of width/height)
    double padX = fixedWorldExtent.width() * 0.1;
    double padY = fixedWorldExtent.height() * 0.1;




    mWorldExtentPadded = QgsRectangle(
        fixedWorldExtent.xMinimum() - padX,
        fixedWorldExtent.yMinimum() - padY,
        fixedWorldExtent.xMaximum() + padX,
        fixedWorldExtent.yMaximum() + padY);

    setCanvasColor(QColor("#012D42"));
    enableAntiAliasing(true);

    QgsSettings settings;
    settings.setValue("/qgis/enable_render_caching", true);
    settings.setValue("qgis/default_tiles_loading_strategy", "async");


    // Add this for smoother updates
    setMapUpdateInterval(30);  // Update every 50ms instead of every frame

    // Use OpenGL viewport
    //QOpenGLWidget* glViewport = new QOpenGLWidget();
    //setViewport(glViewport);

    // Set cache size for better performance
    //setCacheMode(QGraphicsView::CacheBackground);

    setCachingEnabled(true);
    setParallelRenderingEnabled(true);
    setSegmentationTolerance(500);

     setRenderFlag(true);
     freeze(false);

     _m_crs = QgsCoordinateReferenceSystem("EPSG:4326");
     setDestinationCrs(_m_crs);

     connect(&_m_objUpdatePosition,SIGNAL(signalUpdatePosition(QString,double,double,double)),
             this,SLOT(slotUpdatePosition(QString,double,double,double)));


     connect(&_m_objUpdateRoute,SIGNAL(signalUpdatePoints(QString,QList<QgsPointXYZ>)),
             this,SLOT(slotUpdatePoints(QString,QList<QgsPointXYZ>)));

     connect(&timerUpdate,SIGNAL(timeout()),this,SLOT(update()));
     timerUpdate.start(10);
     //QgsCoordinateReferenceSystem crs("EPSG:3857"); // WGS 84 (lat/lon)
     //setDestinationCrs(crs);

}

void CMapCanvas::Initialize() {

    _loadRasterMaps();
    _loadVectorMaps();
    //_loadLayers();
    enforceLayerOrder();
    zoomToFullExtent();
    refresh();
    
    // Auto-load the last saved scenario
    QTimer::singleShot(500, this, [this]() {
        autoLoadScenario();
    });
}

void CMapCanvas::importRasterMap(QString inputPath) {
    convertAndCacheRaster(inputPath);
}

void CMapCanvas::convertAndCacheRaster(const QString inputPath)
{
    QFileInfo info(inputPath);
    QString cacheDir = QDir::currentPath() + "/../maps/RasterRepository";
    QDir().mkpath(cacheDir);

    m_outputPath = cacheDir + "/"+info.baseName()+".TIFF";
    m_inputPath = inputPath;

    QDir().mkpath(QFileInfo(m_outputPath).absolutePath());

    if (m_progressDialog)
        delete m_progressDialog;

    m_progressDialog = new QProgressDialog("Preparing raster...", "Cancel", 0, 100, this);
    m_progressDialog->setWindowModality(Qt::ApplicationModal);
    m_progressDialog->setWindowTitle("Raster Processing");
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setAutoReset(false);
    m_progressDialog->show();

    QTimer::singleShot(100, this, &CMapCanvas::startGdal2Tiles);

}

int CMapCanvas::computeMaxZoom(double rasterRes)
{
    for (int z = 0; z <= 25; z++)
    {
        double res = 156543.03392804097 / (1 << z);
        if (res <= rasterRes)
            return z;
    }
    return 25;
}

void CMapCanvas::startGdal2Tiles()
{
    if (m_translateProcess)
        delete m_translateProcess;

    m_translateProcess = new QProcess(this);
    connect(m_translateProcess, &QProcess::readyReadStandardOutput, this, &CMapCanvas::handleGdalStdout);
    connect(m_translateProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
           this, &CMapCanvas::loadCachedAfterProcessing);

    QgsRasterLayer raster(m_inputPath, "raster");
    double pixelSizeX = raster.rasterUnitsPerPixelX();
    double pixelSizeY = raster.rasterUnitsPerPixelY();
    double rasterResolution = std::max(pixelSizeX, pixelSizeY); // meters/pixel

    QgsRectangle extent = raster.extent();
    double worldSize = 40075016.6856; // meters width of Web Mercator
    double ratio = worldSize / extent.width();

    // tiles double each zoom → log2 ratio of world to raster width
    int nMinZoomLevel =  std::floor(std::log2(ratio));
    int nMaxZoomLevel = computeMaxZoom(rasterResolution);


    QString cmd = QString("python.exe Scripts/gdal2tiles.py --tiling-scheme=geodetic -z %1-%2 %3 %4").arg(nMinZoomLevel).arg(nMaxZoomLevel)
                      .arg(m_inputPath, m_outputPath);
    qDebug() << "Running gdal2tiles:" << cmd;

    //m_translateProcess->setProcessChannelMode(QProcess::MergedChannels);
    //m_translateProcess->start("cmd.exe", QStringList() << "/c" << cmd);
}

void CMapCanvas::startGdalTranslate()
{
    if (m_translateProcess)
        delete m_translateProcess;

    m_translateProcess = new QProcess(this);
    connect(m_translateProcess, &QProcess::readyReadStandardOutput, this, &CMapCanvas::handleGdalStdout);
    connect(m_translateProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::startGdalAddo);

    QString cmd = QString("gdal_translate.exe -of GTiff -co TILED=YES -co COMPRESS=DEFLATE %1 %2")
                  .arg(m_inputPath, m_outputPath);
    qDebug() << "Running gdal_translate:" << cmd;

    m_translateProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_translateProcess->start("cmd.exe", QStringList() << "/c" << cmd);
}

void CMapCanvas::startGdalAddo()
{
    if (m_addoProcess)
        delete m_addoProcess;

    m_addoProcess = new QProcess(this);
    connect(m_addoProcess, &QProcess::readyReadStandardOutput, this, &CMapCanvas::handleGdalStdout);
    connect(m_addoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::loadCachedAfterProcessing);

    QString cmd = QString("gdaladdo.exe -r average %1 2 4 8 16 32 64 128 256").arg(m_outputPath);
    qDebug() << "Running gdaladdo:" << cmd;

    m_addoProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_addoProcess->start("cmd.exe", QStringList() << "/c" << cmd);
}

void CMapCanvas::handleGdalStdout()
{
    QProcess* proc = qobject_cast<QProcess*>(sender());
    if (!proc) return;

    QByteArray output = proc->readAllStandardOutput();
    QString text = QString::fromUtf8(output);
    //qDebug()<<text;
    //m_progressDialog->setLabelText(text);

    QRegularExpression re(R"((\d+))");  // Matches one or more digits

    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch()) {
        QString numberStr = match.captured(1); // First capturing group
        int number = numberStr.toInt();
        m_progressDialog->setValue(number);
    }


//    // GDAL prints progress like: "0...10...20...30..."
//    static QRegularExpression re(R"((\d{1,3})\.\.\.)");
//    QRegularExpressionMatchIterator i = re.globalMatch(text);
//    int lastValue = -1;

//    while (i.hasNext()) {
//        QRegularExpressionMatch match = i.next();
//        lastValue = match.captured(1).toInt();
//    }

//    if (lastValue >= 0 && lastValue <= 100 && m_progressDialog) {
//        m_progressDialog->setValue(lastValue);
//    }
}

void CMapCanvas::loadCachedAfterProcessing()
{
    if (m_progressDialog) {
        m_progressDialog->setValue(100);
        m_progressDialog->close();
    }

    //loadRasterFile(m_outputPath);
}

void CMapCanvas::loadRasterTiledMap(QString tileDir)
{
    //Ensure folder exists
    QDir dir(tileDir);
    if (!dir.exists()) {
        qDebug() << "Tile directory does not exist:" << tileDir;
        return;
    }

    QString urlTileMapRepository = tileDir.replace("\\", "/") + "/mapml.mapml";

    QFile file(urlTileMapRepository);
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qDebug() << "Invalid mapml XML!";
        return;
    }
    QDomElement root = doc.documentElement();
    QDomNode body = root.firstChildElement("body");
    QDomNode extent = body.firstChildElement("extent");

    int nMinZoom = 0;
    int nMaxZoom = 0;
    // --- Extract zoom info ---
    QDomNodeList inputs = extent.toElement().elementsByTagName("input");
    for (int i = 0; i < inputs.size(); i++) {
        QDomElement input = inputs.at(i).toElement();
        if (input.attribute("type") == "zoom") {
            nMinZoom = input.attribute("min").toInt();
            nMaxZoom = input.attribute("max").toInt();
        }
    }



    QString url = "file:///" + tileDir.replace("\\", "/") + "/{z}/{x}/{y}.png";
    QString uri = QString("type=xyz&url=%1&zmin=%2&zmax=%3").arg(url).arg(nMinZoom).arg(nMaxZoom);
    //qDebug() << "Attempting to load tile URI:" << uri;
    //qDebug()<<uri<<dir.dirName();

    // Use provider "xyz" (preferred for XYZ tiles)
    QgsRasterLayer* tileLayer = new QgsRasterLayer(uri,dir.dirName(),"wms");
    if (!tileLayer->isValid())
    {
        qDebug() << "Failed to load tile layer!  Layer is invalid.";
        delete tileLayer;
        return;
    }

    QgsProject::instance()->addMapLayer(tileLayer);

    QList<QgsMapLayer *> currentLayers = layers();
    currentLayers.append(tileLayer);
    setLayers(currentLayers);
    refresh();
}

void CMapCanvas::loadRasterFile( QString rasterPath) {

    // QFileInfo fileInfo(rasterPath);
    // if (!fileInfo.exists()) {
    //     qDebug() << "Raster file not found:" << rasterPath;
    //     return;
    // }


    // // Create raster layer
    // QgsRasterLayer* rasterLayer = new QgsRasterLayer(rasterPath, fileInfo.completeBaseName(),"gdal");

    // if (!rasterLayer->isValid()) {
    //     qDebug() << "Invalid raster layer: " << rasterPath;
    //     delete rasterLayer;
    //     return;
    // }


    // // Add to QGIS project and canvas

    // QgsProject::instance()->addMapLayer(rasterLayer);
    // QList<QgsMapLayer*> alllayers = layers();
    // alllayers.insert(0, rasterLayer);  // Background (lowest)
    // setLayers(alllayers);
    // //setDestinationCrs(wgs84); // Important for canvas reprojectionsetDestinationCrs(wgs84); // Important for canvas reprojection
    // //setExtent(rasterLayer->extent());
    // //refresh();

    // //qDebug() << "Loaded raster with world file:" << rasterPath<<rasterLayer->extent();
}

void CMapCanvas::_loadRasterMaps() {

    QString directoryPath = "../maps/RasterRepository/";
    QDir dir(directoryPath);

    QFileInfoList rasterFiles = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (rasterFiles.isEmpty()) {
        qDebug() << "No raster folder found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : rasterFiles) {
        QString filePath = fileInfo.absoluteFilePath();
        loadRasterTiledMap(filePath);
    }
}

void CMapCanvas::_loadVectorMaps() {

    QString directoryPath = "../maps/ShpRepository/";
    QDir dir(directoryPath);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << directoryPath;
        return;
    }

    QStringList filters;
    filters << "*.shp";

    QFileInfoList shpFiles = dir.entryInfoList(filters, QDir::Files);

    if (shpFiles.isEmpty()) {
        qDebug() << "No shapefiles found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : shpFiles) {
        QString filePath = fileInfo.absoluteFilePath();
        //qDebug() << "Loading shapefile:" << filePath;
        loadShapeFile(filePath);
    }
}

void CMapCanvas::_loadLayers() {

}

void CMapCanvas::enforceLayerOrder()
{
    QList<QgsMapLayer*> orderedLayers;

    // Step 2. Then all vector layers
    for (QgsMapLayer *layer : QgsProject::instance()->mapLayers().values())
    {
        if (layer->type() == Qgis::LayerType::Vector)
            orderedLayers.append(layer);
    }

    // Step 1. Get all rasters first
    for (QgsMapLayer *layer : QgsProject::instance()->mapLayers().values())
    {
        if (layer->type() == Qgis::LayerType::Raster)
            orderedLayers.append(layer);
    }

    // Step 3. Apply to both project & canvas
    QgsProject::instance()->layerTreeRoot()->setHasCustomLayerOrder(true);
    QgsProject::instance()->layerTreeRoot()->setCustomLayerOrder(orderedLayers);
    setLayers(orderedLayers);
    refresh();
}

void CMapCanvas::loadShapeFile(const QString &shpPath)
{
    QgsVectorLayer *layer = new QgsVectorLayer(shpPath, QFileInfo(shpPath).baseName(), "ogr");

    if (!layer->isValid()) {
        qDebug() << "Layer failed to load: " << shpPath;
        delete layer;  // Avoid memory leak
        return;
    }
    layer->setBlendMode(QPainter::CompositionMode_SourceOver);
    QString baseName = QFileInfo(shpPath).baseName().toLower();

// //         QgsGradientFillSymbolLayer *gradientLayer = new QgsGradientFillSymbolLayer();
// // //        gradientLayer->setColor(QColor("#008888"));     // Start color (cyan)
// // //        gradientLayer->setColor2(QColor("#005555"));    // End color (dark blue)
// //         gradientLayer->setColor(QColor("#000000"));     // Start color (cyan)
// //         gradientLayer->setColor2(QColor("#505050"));    // End color (dark blue)
// //         gradientLayer->setGradientSpread(Qgis::GradientSpread::Pad); // Optional: for better visuals
// //         // Set gradient type (Linear or Radial)
// //         gradientLayer->setGradientType(Qgis::GradientType::Radial);
// //         // Set coordinate mode
// //         gradientLayer->setCoordinateMode(Qgis::SymbolCoordinateReference::Viewport);

// //         // Optionally adjust reference points (between 0 and 1)
// //         gradientLayer->setReferencePoint1(QPointF(0.0, 0.0));
// //         gradientLayer->setReferencePoint2(QPointF(1.0, 1.0));

         QgsFillSymbol *symbol = new QgsFillSymbol();
         symbol->deleteSymbolLayer(0);  // Remove the default simple fill
//         symbol->appendSymbolLayer(gradientLayer);


        // Now add a stroke layer for outline
        QgsSimpleLineSymbolLayer *outlineLayer = new QgsSimpleLineSymbolLayer();
        outlineLayer->setColor(QColor("#222222"));  // Stroke color (blue)
        outlineLayer->setWidth(0.3);                 // Stroke width in map units (adjust as needed)

        symbol->appendSymbolLayer(outlineLayer);

        QgsSingleSymbolRenderer *renderer = dynamic_cast<QgsSingleSymbolRenderer *>(layer->renderer());
        if (renderer)
        {
            renderer->setSymbol(symbol);
        }
        else
        {
            // If it's not already a single symbol renderer, create a new one
            renderer = new QgsSingleSymbolRenderer(symbol);
            layer->setRenderer(renderer);
        }

        // Assign it to your vector layer
        //layer->renderer()->setSymbol(symbol);

        // Refresh the layer rendering

        layer->triggerRepaint();



    QgsProject::instance()->addMapLayer(layer);

    // Keep existing layers and add the new one
    QList<QgsMapLayer *> currentLayers = layers();
    currentLayers.append(layer);
    setLayers(currentLayers);



    // Optional: Zoom to the extent of the first layer loaded
//    if (currentLayers.size() == 1) {
//        setExtent(layer->extent());
//        refresh();
//    }
}

void CMapCanvas::mapHome() {


}

void CMapCanvas::zoomBy(double factor)
{
    QgsRectangle currentExtent = extent();
    QgsPointXY center = currentExtent.center();

    double newWidth = currentExtent.width() * factor;
    double newHeight = currentExtent.height() * factor;

    QgsRectangle newExtent(
        center.x() - newWidth / 2,
        center.y() - newHeight / 2,
        center.x() + newWidth / 2,
        center.y() + newHeight / 2
    );

    if (factor > 1)  // zooming out
    {
        if (newExtent.width() > mWorldExtentPadded.width() ||
            newExtent.height() > mWorldExtentPadded.height())
        {
            newExtent = mWorldExtentPadded;
        }
    }

    //Limit zoom out but allow wrapping horizontally
    if (factor > 1) {
        // Limit vertical extent
        if (newExtent.height() > 180.0) {
            double ratio = 180.0 / newExtent.height();
            newHeight = 180.0;
            newWidth = newWidth * ratio;

            newExtent = QgsRectangle(
                center.x() - newWidth / 2,
                -90.0,
                center.x() + newWidth / 2,
                90.0
                );
        }
    }
    setRenderFlag(false);
    setExtent(newExtent);
    setRenderFlag(true);
    //update();
}

void CMapCanvas::panCanvas(double dx, double dy)
{
    QgsRectangle currentExtent = extent();
    currentExtent.setXMinimum(currentExtent.xMinimum() + dx);
    currentExtent.setXMaximum(currentExtent.xMaximum() + dx);
    currentExtent.setYMinimum(currentExtent.yMinimum() + dy);
    currentExtent.setYMaximum(currentExtent.yMaximum() + dy);

    // Allow longitude to exceed ±180° for wrapping
    // QGIS will automatically wrap the display

    // Optional: Keep latitude within bounds
    if (currentExtent.yMinimum() < -90.0) {
        double shift = -90.0 - currentExtent.yMinimum();
        currentExtent.setYMinimum(-90.0);
        currentExtent.setYMaximum(currentExtent.yMaximum() + shift);
    }
    if (currentExtent.yMaximum() > 90.0) {
        double shift = currentExtent.yMaximum() - 90.0;
        currentExtent.setYMaximum(90.0);
        currentExtent.setYMinimum(currentExtent.yMinimum() - shift);
    }
    setRenderFlag(false);
    setExtent(currentExtent);
    setRenderFlag(true);
    //refresh();

}

void CMapCanvas::wheelEvent(QWheelEvent *event)
{
//    const double zoomFactor = 1.1;
//    if (event->angleDelta().y() > 0)
//        zoomByFactor(1 / zoomFactor); // Zoom in
//    else
//        zoomByFactor(zoomFactor);     // Zoom out

    const double zoomFactor = 1.1;
    double factor = (event->angleDelta().y() > 0) ? (1.0 / zoomFactor) : zoomFactor;

    // Get the position of the mouse in map coordinates
    QPoint mousePos = event->pos();
    //qDebug()<<mousePos;
    QgsPointXY mapPointBeforeZoom = getCoordinateTransform()->toMapCoordinates(mousePos.x(), mousePos.y());


    double dScale = scale()/1000.0;
    if ( factor < 1 && dScale < 1.1 ) {
        return;
    }
    setRenderFlag(false);
    zoomByFactor(factor);
    //setCenter(mapPointBeforeZoom);

    QgsPointXY after = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());

    // 5️⃣ Shift map so "before" stays at mouse
    QgsPointXY center1 = center() + (mapPointBeforeZoom - after);
    setCenter(center1);

    setRenderFlag(true);


    // // Get the new map coordinates of the same screen point
    // QgsPointXY mapPointAfterZoom = getCoordinateTransform()->toMapCoordinates(mousePos.x(), mousePos.y());

    // // Compute how much the map moved due to zoom
    // double dx = mapPointBeforeZoom.x() - mapPointAfterZoom.x();
    // double dy = mapPointBeforeZoom.y() - mapPointAfterZoom.y();


    // // Pan the canvas to offset the difference — this keeps the cursor point stable
    // panCanvas(dx, dy);
    //setRenderFlag(true);
    //update();
    refresh();

}

void CMapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _m_nCurrentObjectClassForLoading == VISTAR_CLASS_NONE ) {
        mLastMousePos = event->pos();
        mPanning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CMapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (mPanning) {
//        QPoint delta = event->pos() - mLastMousePos;
//        panCanvas(-delta.x(), delta.y());
//        mLastMousePos = event->pos();

        QPoint delta = event->pos() - mLastMousePos;

        double dx = -delta.x() * mapSettings().mapUnitsPerPixel();
        double dy = delta.y() * mapSettings().mapUnitsPerPixel();  // Negative because Y axis is inverted in screen coords
        //setRenderFlag(false);
        panCanvas(dx, dy);  // Now using proper map units
        mLastMousePos = event->pos();
        //setRenderFlag(true);
        //refresh();
        //update();
    }

    // Convert mouse screen position to map coordinates
    QgsPointXY mapPoint = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());

    // Convert to geographic (lat/lon) using the destination CRS (assumed WGS84)
    QgsCoordinateTransform transform(mapSettings().destinationCrs(), _m_crs, QgsProject::instance());
    QgsPointXY geoPoint = transform.transform(mapPoint);

    //double alt = 0.0;  // Optional: add altitude logic here if needed
    emit signalMouseRead(QString("Lon : %1°    Lat : %2° ").arg(geoPoint.x(),11,'f',6).arg(geoPoint.y(),10,'f',6));
}

void CMapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mPanning ) {
        mPanning = false;
        setCursor(mPreviousCursor);
    }
    else if (event->button() == Qt::LeftButton) {

        mPanning = false;
        // Convert mouse screen position to map coordinates
        QgsPointXY mapPoint = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());

        // Convert to geographic (lat/lon) using the destination CRS (assumed WGS84)
        QgsCoordinateTransform transform(mapSettings().destinationCrs(), _m_crs, QgsProject::instance());
        QgsPointXY geoPoint = transform.transform(mapPoint);

        if ( _m_nCurrentObjectClassForLoading == VISTAR_CLASS_ROUTE ) {
            if ( _m_sCurrentRoute.isEmpty() ) {
                QString sObjectId = GenerateObjectIdFromClass(_m_nCurrentObjectClassForLoading);

                if ( !sObjectId.isEmpty() ) {
                    CVistarRoute *vistarRoute = new CVistarRoute(this,sObjectId,geoPoint.x(),geoPoint.y());
                    _m_listVistarRoutes.insert(sObjectId,vistarRoute);
                    _m_sCurrentRoute = sObjectId;
                }
            }
            else {
                CVistarRoute *vistarRoute = getVistarRouteById(_m_sCurrentRoute);
                if (vistarRoute) {
                    if ( vistarRoute->getPointCount() < 10 ) {
                        vistarRoute->addPoint(geoPoint);
                    }
                    if (vistarRoute->getPointCount() >= 10) {
                        _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
                        mPreviousCursor = Qt::ArrowCursor;
                        _m_sCurrentRoute = "";
                        setCursor(mPreviousCursor);
                        signalClearObjectSelection();
                        
                        // Auto-save after completing route
                        QTimer::singleShot(100, this, [this]() {
                            autoSaveScenario();
                        });
                    }
                }
            }
        }
        else if ( _m_nCurrentObjectClassForLoading != VISTAR_CLASS_NONE ) {
            QString sObjectId = GenerateObjectIdFromClass(_m_nCurrentObjectClassForLoading);
            qDebug()<<"in VISTAR object "<<sObjectId;
            if ( !sObjectId.isEmpty() ) {
                CVistarObject *vistarObject = new CVistarObject(this,sObjectId,_m_nCurrentObjectClassForLoading,geoPoint.x(),geoPoint.y());
                _m_listVistarObjects.insert(sObjectId,vistarObject);
                _m_listVistarObjectIds.append(sObjectId);
                qDebug()<<"VISTAR object added, ID= "<<sObjectId;

                if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_LAUNCHER ||
                    _m_nCurrentObjectClassForLoading == VISTAR_CLASS_FIGHTER  ||
                    _m_nCurrentObjectClassForLoading == VISTAR_CLASS_UAV) {

                    eVISTAR_CLASS mslClass = VISTAR_CLASS_MISSILE;
                    int nChildMslCount = 4;
                    if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_FIGHTER) {
                        nChildMslCount = 0;
                    }
                    else if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_LAUNCHER) {
                        nChildMslCount = 12;
                    }
                    for ( int i = 1; i<= nChildMslCount; i++) {
                        QString sChildObjectId = GenerateObjectIdFromClass(mslClass);

                        if ( !sChildObjectId.isEmpty() ) {
                            CVistarObject *vistarChildObject = new CVistarObject(this,sChildObjectId,mslClass,geoPoint.x(),geoPoint.y());
                            vistarChildObject->setParent(sObjectId,i);
                            _m_listVistarObjects.insert(sChildObjectId,vistarChildObject);
                            _m_listVistarObjectIds.append(sChildObjectId);
                        }
                    }

                }
                refresh();
            }

            _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
            mPreviousCursor = Qt::ArrowCursor;
            _m_sCurrentRoute = "";
            setCursor(mPreviousCursor);
            signalClearObjectSelection();
            
            // Auto-save after adding new object
            QTimer::singleShot(100, this, [this]() {
                autoSaveScenario();
            });
        }
        else {

        }
    }
    else if (event->button() == Qt::RightButton) {
        showContextMenu(event->pos());
    }
}

QString CMapCanvas::GenerateObjectIdFromClass( int nClass ) {

    QString sObjectId = "";
    QString sClass = "";
    switch ( nClass ) {
    case VISTAR_CLASS_DRONE : sClass = "DRONE";
        break;
    case VISTAR_CLASS_DRONE_SWARM : sClass = "SWARM";
        break;
    case VISTAR_CLASS_FIGHTER : sClass = "FIGHTER";
        break;
    case VISTAR_CLASS_UAV : sClass = "UAV";
        break;
    case VISTAR_CLASS_RADAR : sClass = "RADAR";
        break;
    case VISTAR_CLASS_LAUNCHER : sClass = "LAUNCHER";
        break;
    case VISTAR_CLASS_MISSILE : sClass = "MISSILE";
        break;
    case VISTAR_CLASS_ROUTE : sClass = "ROUTE";
        break;
    }

    if ( !sClass.isEmpty() ) {
        int i = 1;
        while (true && i < 100) {
            QString sTempId = sClass+"_"+QString::number(i);
            if (nClass == VISTAR_CLASS_ROUTE ) {
                if ( !isVistarRouteByIdExists(sTempId) ) {
                    sObjectId = sTempId;
                    break;
                }
            }
            else {
                if ( !isVistarObjectByIdExists(sTempId) ) {
                    sObjectId = sTempId;
                    break;
                }
            }
            i++;
        }
    }
    return sObjectId;
}

CVistarObject* CMapCanvas::getVistarObjectById(QString sObjectId) {

    if ( _m_listVistarObjects.contains(sObjectId) ) {
        qDebug()<<"Not null";
        return _m_listVistarObjects.value(sObjectId);
    }
    else{
        qDebug()<<"Null";
        return nullptr;
    }
}

CVistarRoute* CMapCanvas::getVistarRouteById(QString sObjectId) {

    if ( _m_listVistarRoutes.contains(sObjectId) ) {
        return _m_listVistarRoutes.value(sObjectId);
    }
    else{
        return nullptr;
    }
}

bool CMapCanvas::isVistarObjectByIdExists(QString sObjectId) {

    return _m_listVistarObjects.contains(sObjectId);
}

bool CMapCanvas::isVistarRouteByIdExists(QString sObjectId) {

    return _m_listVistarRoutes  .contains(sObjectId);
}

void CMapCanvas::mouseDoubleClickEvent( QMouseEvent *e ) {

    if (e->button() == Qt::LeftButton) {
        if ( _m_nCurrentObjectClassForLoading == VISTAR_CLASS_ROUTE ) {
            _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
            mPreviousCursor = Qt::ArrowCursor;
            _m_sCurrentRoute = "";
            setCursor(mPreviousCursor);
            signalClearObjectSelection();
            
            // Auto-save after finishing route
            QTimer::singleShot(100, this, [this]() {
                autoSaveScenario();
            });
        }
        else if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_NONE) {
            for (CVistarObject* item : _m_listVistarObjects)
            {
                QgsPointXY screenPoint = mapSettings().mapToPixel().transform(item->getPoint());
                int nRadius = 20;
                QRectF rect(screenPoint.x()-nRadius/2,screenPoint.y()-nRadius/2,nRadius,nRadius);
                //qDebug()<<event->pos().x()<<event->pos().y()<<screenPoint.x()<<screenPoint.y();
                if ( rect.contains(e->pos()) ) {
                    item->setHighlighted(true);
                }
                else {
                    item->setHighlighted(false);
                }
            }
            for (CVistarRoute* item : _m_listVistarRoutes)
            {
                item->setHighlighted(false);
                int nCount = item->getPointCount();
                for ( int i = 0; i < nCount; i++ ) {
                    QgsPointXY pt = item->getPointAt(i);

                    QgsPointXY screenPoint = mapSettings().mapToPixel().transform(pt);
                    int nRadius = 20;
                    QRectF rect(screenPoint.x()-nRadius/2,screenPoint.y()-nRadius/2,nRadius,nRadius);
                    //qDebug()<<event->pos().x()<<event->pos().y()<<screenPoint.x()<<screenPoint.y();
                    if ( rect.contains(e->pos()) ) {
                        item->setHighlighted(true);
                        break;
                    }
                }
            }
            refresh();
        }
    }

    QgsMapCanvas::mouseDoubleClickEvent(e);
}

void CMapCanvas::keyPressEvent(QKeyEvent *event)
{
    QgsRectangle extent = this->extent();
    double moveX = extent.width() * 0.1; // 10% move

    switch (event->key()) {
    case Qt::Key_Left:
        panCanvas(-moveX, 0);
        break;
    case Qt::Key_Right:
        panCanvas(moveX, 0);
        break;
    case Qt::Key_Up:
        panCanvas(0, moveX);
        break;
    case Qt::Key_Down:
        panCanvas(0, -moveX);
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:  // Shift + '=' is usually '+'
        zoomBy(1 / 1.1);  // Zoom in
        break;    case Qt::Key_Underscore:
    case Qt::Key_Minus:
        zoomBy(1.1);      // Zoom out
        break;
    default:
        QgsMapCanvas::keyPressEvent(event); // Default handling
        break;
    }
}


void CMapCanvas::SetObjectToLoadOnClick( int nClass ) {
    _m_nCurrentObjectClassForLoading = nClass;

    QString strPath = "";
    int nSize = 50;
    switch ( nClass ) {
    case VISTAR_CLASS_DRONE : strPath = ":/icons/cursor/drone.png"; nSize = 40;
        break;
    case VISTAR_CLASS_DRONE_SWARM : strPath = ":/icons/cursor/drone_swarm.png";
        break;
    case VISTAR_CLASS_FIGHTER : strPath = ":/icons/cursor/fighter.png";
        break;
    case VISTAR_CLASS_UAV : strPath = ":/icons/cursor/uav.png";
        break;
    case VISTAR_CLASS_RADAR : strPath = ":/icons/cursor/radar.png"; nSize = 40;
        break;
    case VISTAR_CLASS_LAUNCHER : strPath = ":/icons/cursor/launcher.png";
        break;
    case VISTAR_CLASS_MISSILE : strPath = ":/icons/cursor/missile.png";
        break;
    case VISTAR_CLASS_ROUTE : strPath = ":/icons/cursor/route.png"; nSize = 40;
        break;
    }

    if (strPath.isEmpty()) {
        mPreviousCursor = Qt::ArrowCursor;
        setCursor(Qt::ArrowCursor);
    }
    else {
        QPixmap cursorPixmap(strPath);
        QPixmap scaledPix = cursorPixmap.scaled(nSize, nSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int nHostY = -1;
        if ( nClass == VISTAR_CLASS_ROUTE) {
            nHostY = -10;
        }
        QCursor customCursor(scaledPix, -1,nHostY);
        mPreviousCursor = customCursor;
        setCursor(customCursor);
    }
}

void CMapCanvas::InitializeAllObjects() {

    for ( CVistarRoute *vistarRoute : _m_listVistarRoutes ) {
        vistarRoute->TransmitSelfInfo();
    }

    // QThread* thread = new QThread;

    // QObject::connect(thread, &QThread::started, [=]() {
    //     for ( QString vistarObjectId : _m_listVistarObjectIds ) {
    //         qDebug()<<vistarObjectId;
    //         CVistarObject *vistarObject = _m_listVistarObjects.value(vistarObjectId);
    //         if (vistarObject) {
    //             vistarObject->TransmitSelfInfo();
    //         }
    //         Sleep(100);
    //     }

    //     thread->quit();   // End thread after work
    // });

    // thread->start();

    for ( QString vistarObjectId : _m_listVistarObjectIds ) {
        qDebug()<<vistarObjectId;
        CVistarObject *vistarObject = _m_listVistarObjects.value(vistarObjectId);
        if (vistarObject) {
            vistarObject->TransmitSelfInfo();
        }
        //Sleep(100);
    }
}

void CMapCanvas::slotUpdateObject(QJsonDocument doc) {


    QJsonObject jsonObject = doc.object();
    QString sSrc =  jsonObject["SRC"].toString();
    if (sSrc == VISTAR_SRC_MISSION_PLANNER) {
        return;
    }
    QString sStream =  jsonObject["STREAM"].toString();
    QString sId = jsonObject["ID"].toString();
    QString obj = jsonObject["LOCATION"].toString();
    qDebug()<<"Data recvd here"<<sId<<doc.toJson()<<obj;
    qDebug()<<"sId="<<sId;
    CVistarObject *vistarObject = getVistarObjectById(sId);
    qDebug()<<vistarObject<<sId;
    if (vistarObject) {
        qDebug()<<"Data recvd1="<<sStream;
        if ( sStream.contains("create") || sStream.contains("update")) {
            qDebug()<<"Data recvd2";
            vistarObject->UpdateObject(jsonObject);
        }
        else if (sStream.contains("action")) {
            QString sAction =  jsonObject["ACTION"].toString();
            if (sAction.contains("destroy")) {
                _m_listVistarObjects.remove(sId);
                for (int i = 0; i < _m_listVistarObjectIds.size(); i++ ) {
                    if (_m_listVistarObjectIds.at(i) == sId ) {
                        _m_listVistarObjectIds.removeAt(i);
                        break;
                    }
                }
                delete vistarObject;
            }
        }

    }
}

void CMapCanvas::showContextMenu(QPoint pos) {

    QList<QMenu *> listMenus;
    QMenu menu;
    for ( CVistarObject *vistarObject : _m_listVistarObjects ) {
        if ( vistarObject->isHighlighted()) {
            QMenu *menuObject = new QMenu();
            listMenus<<menuObject;
            menuObject->setTitle(vistarObject->getObjectId());

            QMenu *menuAttachRoute = new QMenu();
            listMenus<<menuAttachRoute;
            menuAttachRoute->setTitle("Attach Route");

            QAction* actionUpdate = menuObject->addAction("Update");
            actionUpdate->setObjectName(vistarObject->getObjectId());

            for ( CVistarRoute *vistarRoute : _m_listVistarRoutes ) {
                QAction* action = menuAttachRoute->addAction(vistarRoute->getObjectId());
                action->setObjectName(vistarObject->getObjectId());
            }
            if ( menuAttachRoute->actions().count() > 0) {
                menuObject->addMenu(menuAttachRoute);
            }

            QAction* actionDelete = menuObject->addAction("Delete");
            actionDelete->setObjectName(vistarObject->getObjectId());

            if (menuObject->actions().count() > 0) {
                menu.addMenu(menuObject);
            }
        }
    }

    for ( CVistarRoute *vistarRoute : _m_listVistarRoutes ) {
        if ( vistarRoute->isHighlighted()) {
            QMenu *menuObject = new QMenu();
            listMenus<<menuObject;
            menuObject->setTitle(vistarRoute->getObjectId());

            QAction* actionUpdate = menuObject->addAction("Update");
            actionUpdate->setObjectName(vistarRoute->getObjectId());

            QAction* actionDelete = menuObject->addAction("Delete");
            actionDelete->setObjectName(vistarRoute->getObjectId());

            if (menuObject->actions().count() > 0) {
                menu.addMenu(menuObject);
            }
        }
    }

    if (menu.actions().count() > 0 ) {
        QAction* selected = menu.exec(pos);
        if ( selected ) {
            qDebug()<<selected->text()<<selected->objectName();
            CVistarObject* object = getVistarObjectById(selected->objectName());
            if (object) {
                if ( selected->text() == "Update") {
                    _m_objUpdatePosition.setObjectId(selected->objectName());
                    QgsPointXYZ pt = object->getPointXYZ();
                    _m_objUpdatePosition.setPosition(pt.y(),pt.x(),pt.z());
                    _m_objUpdatePosition.show();
                }
                else if ( selected->text() == "Delete") {
                    _m_listVistarObjects.remove(selected->objectName());
                    for (int i = 0; i < _m_listVistarObjectIds.size(); i++ ) {
                        CVistarObject* objectTemp = getVistarObjectById(_m_listVistarObjectIds.at(i));
                        if (objectTemp) {
                            if ( objectTemp->getParent() == selected->objectName() ) {
                                if (objectTemp->getChildId() > 0 ) {
                                    _m_listVistarObjectIds.removeAt(i);
                                    delete objectTemp;
                                    continue;
                                }
                            }
                        }
                        if (_m_listVistarObjectIds.at(i) == selected->objectName() ) {
                            _m_listVistarObjectIds.removeAt(i);
                            break;
                        }
                    }
                    delete object;
                    
                    // Auto-save after deleting object
                    QTimer::singleShot(100, this, [this]() {
                        autoSaveScenario();
                    });
                }
                else if (selected->text() == "Attach Route") {
                    object->attachRoute(selected->text());
                    object->setHighlighted(false);
                }
            }
            else {
                CVistarRoute* route = getVistarRouteById(selected->objectName());
                if (route) {
                    if ( selected->text() == "Update") {
                        _m_objUpdateRoute.setObjectId(selected->objectName());
                        QList<QgsPointXYZ> listPoints = route->getPoints();
                        _m_objUpdateRoute.setPoints(listPoints);
                        _m_objUpdateRoute.show();
                    }
                    else if ( selected->text() == "Delete") {
                        _m_listVistarRoutes.remove(selected->objectName());
                        delete route;
                        
                        // Auto-save after deleting route
                        QTimer::singleShot(100, this, [this]() {
                            autoSaveScenario();
                        });
                    }
                }
            }
        }
    }

    for ( QMenu *menu1 : listMenus) {
        delete menu1;
    }
}

void CMapCanvas::slotUpdatePosition(QString sObjectId,double dLat,double dLon,double dAlt) {
    CVistarObject *vistarObject = getVistarObjectById(sObjectId);
    if (vistarObject) {
        vistarObject->UpdateLocation(dLat,dLon,dAlt);
        
        // Auto-save after updating position
        QTimer::singleShot(100, this, [this]() {
            autoSaveScenario();
        });
    }
}

void CMapCanvas::slotUpdatePoints(QString sObjectId,QList<QgsPointXYZ> listPoints) {
    CVistarRoute *vistarRoute = getVistarRouteById(sObjectId);
    if (vistarRoute) {
        vistarRoute->UpdatePoints(listPoints);
        
        // Auto-save after updating route points
        QTimer::singleShot(100, this, [this]() {
            autoSaveScenario();
        });
    }
}

// ============ Scenario Management Methods ============

QString CMapCanvas::getClassNameFromEnum(int nClass) {
    switch (nClass) {
        case VISTAR_CLASS_DRONE: return "DRONE";
        case VISTAR_CLASS_DRONE_SWARM: return "DRONE_SWARM";
        case VISTAR_CLASS_FIGHTER: return "FIGHTER";
        case VISTAR_CLASS_UAV: return "UAV";
        case VISTAR_CLASS_RADAR: return "RADAR";
        case VISTAR_CLASS_LAUNCHER: return "LAUNCHER";
        case VISTAR_CLASS_MISSILE: return "MISSILE";
        case VISTAR_CLASS_ROUTE: return "ROUTE";
        default: return "UNKNOWN";
    }
}

int CMapCanvas::getEnumFromClassName(const QString &className) {
    if (className == "DRONE") return VISTAR_CLASS_DRONE;
    if (className == "DRONE_SWARM") return VISTAR_CLASS_DRONE_SWARM;
    if (className == "FIGHTER") return VISTAR_CLASS_FIGHTER;
    if (className == "UAV") return VISTAR_CLASS_UAV;
    if (className == "RADAR") return VISTAR_CLASS_RADAR;
    if (className == "LAUNCHER") return VISTAR_CLASS_LAUNCHER;
    if (className == "MISSILE") return VISTAR_CLASS_MISSILE;
    if (className == "ROUTE") return VISTAR_CLASS_ROUTE;
    return VISTAR_CLASS_NONE;
}

Scenario CMapCanvas::createScenarioFromCurrentState() {
    Scenario scenario;
    scenario.name = "Auto-saved Scenario";
    scenario.description = "Automatically saved vistar routes and objects";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // Save all routes
    for (CVistarRoute *vistarRoute : _m_listVistarRoutes) {
        if (!vistarRoute) continue;
        
        ScenarioRoute route;
        route.id = vistarRoute->getObjectId();
        route.name = vistarRoute->getObjectId();
        
        QList<QgsPointXYZ> points = vistarRoute->getPoints();
        for (const QgsPointXYZ &pt : points) {
            route.waypoints.append(QPointF(pt.y(), pt.x())); // lat, lon
            route.altitudes.append(pt.z());
        }
        
        scenario.routes.append(route);
    }
    
    // Save all objects (only parent objects, not child missiles)
    for (const QString &objectId : _m_listVistarObjectIds) {
        CVistarObject *vistarObject = _m_listVistarObjects.value(objectId);
        if (!vistarObject) continue;
        
        // Skip child objects (missiles attached to launchers/UAVs)
        if (vistarObject->getChildId() != 0) continue;
        
        ScenarioObject obj;
        obj.id = vistarObject->getObjectId();
        
        QgsPointXYZ pt = vistarObject->getPointXYZ();
        obj.latitude = pt.y();
        obj.longitude = pt.x();
        obj.altitude = pt.z();
        
        // Get object type from ID prefix
        QString idPrefix = objectId.split("_").first();
        obj.type = idPrefix;
        
        // Store additional data
        obj.additionalData["parent"] = vistarObject->getParent();
        obj.additionalData["childId"] = vistarObject->getChildId();
        obj.additionalData["attachedRoute"] = vistarObject->getAttachedRoute();
        
        scenario.objects.append(obj);
    }
    
    scenario.metadata["version"] = "1.0";
    scenario.metadata["autoSaved"] = true;
    
    return scenario;
}

void CMapCanvas::loadScenarioToCanvas(const Scenario &scenario) {
    // Clear existing objects and routes
    qDebug() << "Loading scenario:" << scenario.name;
    
    // Clear existing routes
    for (CVistarRoute *route : _m_listVistarRoutes) {
        delete route;
    }
    _m_listVistarRoutes.clear();
    
    // Clear existing objects
    for (CVistarObject *obj : _m_listVistarObjects) {
        delete obj;
    }
    _m_listVistarObjects.clear();
    _m_listVistarObjectIds.clear();
    
    // Load routes first
    for (const ScenarioRoute &route : scenario.routes) {
        if (route.waypoints.isEmpty()) continue;
        
        // Create the route with first waypoint
        QPointF firstPt = route.waypoints.first();
        CVistarRoute *vistarRoute = new CVistarRoute(this, route.id, firstPt.y(), firstPt.x());
        
        // Add remaining waypoints
        for (int i = 1; i < route.waypoints.size(); i++) {
            QPointF pt = route.waypoints[i];
            double alt = (i < route.altitudes.size()) ? route.altitudes[i] : 1000.0;
            vistarRoute->addPoint(QgsPointXY(pt.y(), pt.x()));
        }
        
        _m_listVistarRoutes.insert(route.id, vistarRoute);
        qDebug() << "Loaded route:" << route.id << "with" << route.waypoints.size() << "waypoints";
    }
    
    // Load objects
    QHash<QString, int> childCounts; // Track how many children each parent should have
    
    for (const ScenarioObject &obj : scenario.objects) {
        int nClass = getEnumFromClassName(obj.type);
        if (nClass == VISTAR_CLASS_NONE) continue;
        
        CVistarObject *vistarObject = new CVistarObject(
            this, obj.id, nClass, obj.longitude, obj.latitude
        );
        
        // Update altitude
        vistarObject->UpdateLocation(obj.latitude, obj.longitude, obj.altitude);
        
        // Restore attached route if any
        if (obj.additionalData.contains("attachedRoute")) {
            QString attachedRoute = obj.additionalData["attachedRoute"].toString();
            if (!attachedRoute.isEmpty()) {
                vistarObject->attachRoute(attachedRoute);
            }
        }
        
        _m_listVistarObjects.insert(obj.id, vistarObject);
        _m_listVistarObjectIds.append(obj.id);
        
        qDebug() << "Loaded object:" << obj.id << "at" << obj.latitude << "," << obj.longitude;
        
        // Create child missiles if this is a launcher, fighter, or UAV
        if (nClass == VISTAR_CLASS_LAUNCHER || 
            nClass == VISTAR_CLASS_FIGHTER || 
            nClass == VISTAR_CLASS_UAV) {
            
            int nChildMslCount = 0;
            if (nClass == VISTAR_CLASS_LAUNCHER) {
                nChildMslCount = 12;
            } else if (nClass == VISTAR_CLASS_UAV) {
                nChildMslCount = 4;
            }
            
            for (int i = 1; i <= nChildMslCount; i++) {
                QString sChildObjectId = GenerateObjectIdFromClass(VISTAR_CLASS_MISSILE);
                if (!sChildObjectId.isEmpty()) {
                    CVistarObject *vistarChildObject = new CVistarObject(
                        this, sChildObjectId, VISTAR_CLASS_MISSILE, 
                        obj.longitude, obj.latitude
                    );
                    vistarChildObject->setParent(obj.id, i);
                    _m_listVistarObjects.insert(sChildObjectId, vistarChildObject);
                    _m_listVistarObjectIds.append(sChildObjectId);
                }
            }
        }
    }
    
    refresh();
    qDebug() << "Scenario loaded successfully with" << scenario.objects.size() << "objects and" << scenario.routes.size() << "routes";
}

bool CMapCanvas::saveCurrentScenario(const QString &filePath) {
    Scenario scenario = createScenarioFromCurrentState();
    
    QString savePath = filePath;
    if (savePath.isEmpty()) {
        savePath = _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json";
    }
    
    bool success = _m_scenarioManager->saveScenario(scenario, savePath);
    if (success) {
        qDebug() << "Scenario saved successfully to:" << savePath;
    } else {
        qWarning() << "Failed to save scenario to:" << savePath;
    }
    
    return success;
}

bool CMapCanvas::loadScenario(const QString &filePath) {
    QString loadPath = filePath;
    if (loadPath.isEmpty()) {
        loadPath = _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json";
    }
    
    QFileInfo fileInfo(loadPath);
    if (!fileInfo.exists()) {
        qDebug() << "Scenario file does not exist:" << loadPath;
        return false;
    }
    
    Scenario scenario;
    bool success = _m_scenarioManager->loadScenario(loadPath, scenario);
    if (success) {
        loadScenarioToCanvas(scenario);
        qDebug() << "Scenario loaded successfully from:" << loadPath;
    } else {
        qWarning() << "Failed to load scenario from:" << loadPath;
    }
    
    return success;
}

bool CMapCanvas::autoSaveScenario() {
    // Only auto-save if there are routes or objects to save
    if (_m_listVistarRoutes.isEmpty() && _m_listVistarObjects.isEmpty()) {
        qDebug() << "No routes or objects to auto-save";
        return false;
    }
    
    return saveCurrentScenario();
}

bool CMapCanvas::autoLoadScenario() {
    QString autoSavePath = _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json";
    
    QFileInfo fileInfo(autoSavePath);
    if (!fileInfo.exists()) {
        qDebug() << "No auto-save scenario file found";
        return false;
    }
    
    qDebug() << "Auto-loading scenario from:" << autoSavePath;
    return loadScenario(autoSavePath);
}


