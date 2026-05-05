#include "cmapcanvas.h"
#include <qgsvectorlayer.h>
#include <QMouseEvent>
#include <cmath>
#include <QKeyEvent>
#include <qgsrectangle.h>
#include <QDir>
#include <QUrl>
#include <QDateTime>
#include <QFileInfoList>
#include <QDomDocument>
#include <QFile>
#include <QRegularExpression>
#include <algorithm>
#include <qgscoordinatetransformcontext.h>
#include <qgsfillsymbol.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsfillsymbol.h>
#include <qgsrenderer.h>
#include <qgsproject.h>
#include <qgssymbollayerutils.h>
#include <qgsfillsymbollayer.h>
#include <qgslinesymbollayer.h>
#include <QFileInfo>
#include <QProcess>
#include "globalConstants.h"
#include "cvistarobject.h"
#include <qgsmaplayer.h>
#include <qgsproject.h>
#include <qgslayertree.h>
#include <QMenu>
#include <qgsproviderregistry.h>
#include <QOpenGLWidget>
#include <QThread>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QDebug>

CMapCanvas::CMapCanvas(QWidget *parent) : QgsMapCanvas(parent)
{
    _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
    _m_scenarioManager = new CScenarioManager(this);

    // Initialize path generation
    _m_pathGenerator = new CPathGenerator();
    _m_currentPathType = PATH_TYPE_NONE;
    _m_bPathGenerationMode = false;
    _m_bPathStartPointSet = false;
    _m_pathStartMarker = nullptr;
    _m_pathInstructionText = nullptr;
    _m_pathInstructionBgRect = nullptr;

    // Initialize path generation parameters with defaults
    _m_pathParams = CPathGenerator::PathParameters();

    _m_pathParams.numWaypoints       = 50;
    _m_pathParams.defaultAltitude    = 1000.0;
    _m_pathParams.curveFactor        = 0.6;
    _m_pathParams.spreadRadiusKm     = 15.0;
    _m_pathParams.spiralTurns        = 4.0;
    _m_pathParams.spiralExpansionRate= 0.8;
    _m_pathParams.spiralTightness    = 0.5;
    _m_pathParams.spiralClockwise    = true;
    _m_pathParams.zigzagAmplitude    = 0.45;
    _m_pathParams.zigzagFrequency    = 8;
    _m_pathParams.maxTurnRadius      = 0.1;
    _m_pathParams.randomVariance     = 0.45;
    _m_pathParams.randomnessLevel    = 0.5;
    _m_pathParams.noiseScale         = 0.3;
    _m_pathParams.altitudeVariation  = 150.0;
    _m_pathParams.wobbleIntensity    = 0.4;
    _m_pathParams.pathAsymmetry      = 0.3;
    _m_pathParams.clusterTendency    = 0.5;

    QgsRectangle fixedWorldExtent(-180.0, -90.0, 180.0, 90.0);
    mPreviousCursor = Qt::ArrowCursor;

    double padX = fixedWorldExtent.width()  * 0.1;
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

    setMapUpdateInterval(30);
    setCachingEnabled(true);
    setParallelRenderingEnabled(true);
    setSegmentationTolerance(500);

    setRenderFlag(true);
    freeze(false);

    _m_crs = QgsCoordinateReferenceSystem("EPSG:4326");
    setDestinationCrs(_m_crs);

    connect(&_m_objUpdatePosition, SIGNAL(signalUpdatePosition(QString,double,double,double)),
            this, SLOT(slotUpdatePosition(QString,double,double,double)));

    connect(&_m_objUpdateRoute, SIGNAL(signalUpdatePoints(QString,QList<QgsPointXYZ>,QStringList)),
            this, SLOT(slotUpdatePoints(QString,QList<QgsPointXYZ>,QStringList)));

    connect(&timerUpdate, SIGNAL(timeout()), this, SLOT(update()));
    timerUpdate.start(10);
}

// ============================================================
void CMapCanvas::Initialize()
{
    _loadRasterMaps();
    _loadVectorMaps();
    enforceLayerOrder();
    mapHome();
    refresh();

    QTimer::singleShot(500, this, [this]() {
        autoLoadScenario();
    });
}

// ============================================================
//  Raster import  (OS-agnostic: no cmd.exe / .exe)
// ============================================================
void CMapCanvas::importRasterMap(QString inputPath)
{
    convertAndCacheRaster(inputPath);
}

void CMapCanvas::convertAndCacheRaster(const QString inputPath)
{
    QFileInfo info(inputPath);
    // Use applicationDirPath so the path is correct regardless of working directory
    QString cacheDir = QCoreApplication::applicationDirPath() + "/maps/RasterRepository";
    QDir().mkpath(cacheDir);

    m_outputPath = cacheDir + "/" + info.baseName() + ".TIFF";
    m_inputPath  = inputPath;
    QDir().mkpath(QFileInfo(m_outputPath).absolutePath());

    if (m_progressDialog) delete m_progressDialog;

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
    for (int z = 0; z <= 25; z++) {
        double res = 156543.03392804097 / (1 << z);
        if (res <= rasterRes) return z;
    }
    return 25;
}

void CMapCanvas::startGdal2Tiles()
{
    if (m_translateProcess) delete m_translateProcess;
    m_translateProcess = new QProcess(this);

    connect(m_translateProcess, &QProcess::readyReadStandardOutput,
            this, &CMapCanvas::handleGdalStdout);
    connect(m_translateProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::loadCachedAfterProcessing);

    QgsRasterLayer raster(m_inputPath, "raster");
    double rasterResolution = std::max(raster.rasterUnitsPerPixelX(),
                                       raster.rasterUnitsPerPixelY());
    double ratio      = 40075016.6856 / raster.extent().width();
    int nMinZoomLevel = static_cast<int>(std::floor(std::log2(ratio)));
    int nMaxZoomLevel = computeMaxZoom(rasterResolution);

    qDebug() << "Running gdal2tiles zoom" << nMinZoomLevel << "-" << nMaxZoomLevel;

    m_translateProcess->setProcessChannelMode(QProcess::MergedChannels);

#ifdef Q_OS_WIN
    // Windows OSGeo4W: python is on PATH when launched from OSGeo4W shell
    m_translateProcess->start("python", QStringList()
                              << "-m" << "gdal2tiles"
                              << QString("-z%1-%2").arg(nMinZoomLevel).arg(nMaxZoomLevel)
                              << m_inputPath << m_outputPath);
#else
    // Linux / macOS
    m_translateProcess->start("python3", QStringList()
                              << "-m" << "gdal2tiles"
                              << QString("-z%1-%2").arg(nMinZoomLevel).arg(nMaxZoomLevel)
                              << m_inputPath << m_outputPath);
#endif
}

void CMapCanvas::startGdalTranslate()
{
    if (m_translateProcess) delete m_translateProcess;
    m_translateProcess = new QProcess(this);

    connect(m_translateProcess, &QProcess::readyReadStandardOutput,
            this, &CMapCanvas::handleGdalStdout);
    connect(m_translateProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::startGdalAddo);

    QStringList args;
    args << "-of" << "GTiff" << "-co" << "TILED=YES"
         << "-co" << "COMPRESS=DEFLATE" << m_inputPath << m_outputPath;

    m_translateProcess->setProcessChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_WIN
    m_translateProcess->start("gdal_translate.exe", args);
#else
    m_translateProcess->start("gdal_translate", args);
#endif
}

void CMapCanvas::startGdalAddo()
{
    if (m_addoProcess) delete m_addoProcess;
    m_addoProcess = new QProcess(this);

    connect(m_addoProcess, &QProcess::readyReadStandardOutput,
            this, &CMapCanvas::handleGdalStdout);
    connect(m_addoProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::loadCachedAfterProcessing);

    QStringList args;
    args << "-r" << "average" << m_outputPath
         << "2" << "4" << "8" << "16" << "32" << "64" << "128" << "256";

    m_addoProcess->setProcessChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_WIN
    m_addoProcess->start("gdaladdo.exe", args);
#else
    m_addoProcess->start("gdaladdo", args);
#endif
}

void CMapCanvas::handleGdalStdout()
{
    QProcess *proc = qobject_cast<QProcess *>(sender());
    if (!proc) return;

    QByteArray output = proc->readAllStandardOutput();
    QString text = QString::fromUtf8(output);

    QRegularExpression re(R"((\d+))");
    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch() && m_progressDialog)
        m_progressDialog->setValue(match.captured(1).toInt());
}

void CMapCanvas::loadCachedAfterProcessing()
{
    if (m_progressDialog) {
        m_progressDialog->setValue(100);
        m_progressDialog->close();
    }
}

// ============================================================
//  Tile loading — GDAL TMS XML (works on all platforms,
//  no WMS provider required)
// ============================================================
void CMapCanvas::loadRasterTiledMap(QString tileDir)
{
    QDir dir(tileDir);
    if (!dir.exists()) {
        qDebug() << "Tile directory does not exist:" << tileDir;
        return;
    }

    // --- Auto-detect zoom range from numeric sub-folder names ---
    int nMinZoom = 99, nMaxZoom = 0;
    bool foundZoom = false;
    QFileInfoList zoomDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &zDir : zoomDirs) {
        bool ok = false;
        int z = zDir.fileName().toInt(&ok);
        if (ok) {
            nMinZoom = std::min(nMinZoom, z);
            nMaxZoom = std::max(nMaxZoom, z);
            foundZoom = true;
        }
    }
    if (!foundZoom) { nMinZoom = 0; nMaxZoom = 13; }
    qDebug() << "Zoom range for" << dir.dirName() << ":" << nMinZoom << "-" << nMaxZoom;

    // --- Detect tiling scheme from x-tile count at minimum zoom ---
    // Web Mercator (EPSG:3857): x-count == 2^z  at zoom z
    // Geodetic     (EPSG:4326): x-count == 2^(z+1) at zoom z
    bool isWebMercator = true;
    {
        QDir z0(tileDir + "/" + QString::number(nMinZoom));
        if (z0.exists()) {
            int xCount = z0.entryList(QDir::Dirs | QDir::NoDotAndDotDot).count();
            int expWM  = 1 << nMinZoom;
            int expGeo = 1 << (nMinZoom + 1);
            if (std::abs(xCount - expGeo) < std::abs(xCount - expWM))
                isWebMercator = false;
            qDebug() << "x-tiles at zoom" << nMinZoom << ":" << xCount
                     << "→" << (isWebMercator ? "WebMercator EPSG:3857" : "Geodetic EPSG:4326");
        }
    }

    // --- Auto-detect band count from first available tile ---
    int nBands = 4;
    for (const QFileInfo &zDir : zoomDirs) {
        bool ok = false; zDir.fileName().toInt(&ok); if (!ok) continue;
        QDir xDir(zDir.absoluteFilePath());
        QFileInfoList xDirs = xDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        if (xDirs.isEmpty()) continue;
        QDir yDir(xDirs.first().absoluteFilePath());
        QFileInfoList tiles = yDir.entryInfoList({"*.png","*.jpg","*.jpeg"}, QDir::Files);
        if (tiles.isEmpty()) continue;
        QgsRasterLayer probe(tiles.first().absoluteFilePath(), "probe", "gdal");
        if (probe.isValid()) { nBands = probe.bandCount(); }
        qDebug() << "Detected" << nBands << "bands from tile";
        break;
    }

    // --- Build GDAL TMS XML ---
    // Always use forward slashes in the URL — critical on Windows
    QString cleanDir = QDir::cleanPath(tileDir);
    cleanDir.replace("\\", "/");

    QString srs, uxMin, uyMax, uxMax, uyMin, tileCountX, tileCountY, yOrigin;
    if (isWebMercator) {
        srs = "EPSG:3857";
        uxMin = "-20037508.34"; uyMax = "20037508.34";
        uxMax =  "20037508.34"; uyMin = "-20037508.34";
        tileCountX = "1"; tileCountY = "1";
        yOrigin = "top";     // XYZ / Google / OSM convention
    } else {
        srs = "EPSG:4326";
        uxMin = "-180.0"; uyMax = "90.0";
        uxMax =  "180.0"; uyMin = "-90.0";
        tileCountX = "2"; tileCountY = "1";
        yOrigin = "top";     // gdal2tiles geodetic uses top
    }

    QString xml = QString(
        "<GDAL_WMS>\n"
        "  <Service name=\"TMS\">\n"
        "    <ServerUrl>file://%1/${z}/${x}/${y}.png</ServerUrl>\n"
        "    <SRS>%2</SRS>\n"
        "    <ImageFormat>image/png</ImageFormat>\n"
        "  </Service>\n"
        "  <DataWindow>\n"
        "    <UpperLeftX>%3</UpperLeftX>\n"
        "    <UpperLeftY>%4</UpperLeftY>\n"
        "    <LowerRightX>%5</LowerRightX>\n"
        "    <LowerRightY>%6</LowerRightY>\n"
        "    <TileLevel>%7</TileLevel>\n"
        "    <TileCountX>%8</TileCountX>\n"
        "    <TileCountY>%9</TileCountY>\n"
        "    <YOrigin>%10</YOrigin>\n"
        "  </DataWindow>\n"
        "  <Projection>%2</Projection>\n"
        "  <BlockSizeX>256</BlockSizeX>\n"
        "  <BlockSizeY>256</BlockSizeY>\n"
        "  <BandsCount>%11</BandsCount>\n"
        "  <ZeroBlockHttpCodes>204,404</ZeroBlockHttpCodes>\n"
        "  <ZeroBlockOnServerException>true</ZeroBlockOnServerException>\n"
        "</GDAL_WMS>\n"
    )
    .arg(cleanDir)    // %1
    .arg(srs)         // %2  (used twice via Projection tag)
    .arg(uxMin)       // %3
    .arg(uyMax)       // %4
    .arg(uxMax)       // %5
    .arg(uyMin)       // %6
    .arg(nMaxZoom)    // %7
    .arg(tileCountX)  // %8
    .arg(tileCountY)  // %9
    .arg(yOrigin)     // %10
    .arg(nBands);     // %11

    QString vrtPath = QDir::tempPath() + "/" + dir.dirName() + "_tms.xml";
    QFile vrtFile(vrtPath);
    if (!vrtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot write TMS XML:" << vrtPath; return;
    }
    vrtFile.write(xml.toUtf8());
    vrtFile.close();
    qDebug() << "TMS XML written to:" << vrtPath;

    // --- Load via GDAL provider (always present on all platforms) ---
    QgsRasterLayer *tileLayer = new QgsRasterLayer(vrtPath, dir.dirName(), "gdal");
    if (!tileLayer->isValid()) {
        qDebug() << "Failed to load tile layer!" << dir.dirName();
        qDebug() << "QGIS error:" << tileLayer->error().message();
        delete tileLayer; return;
    }

    tileLayer->setCrs(QgsCoordinateReferenceSystem(srs));
    qDebug() << "Layer CRS:" << tileLayer->crs().authid()
             << "| extent:" << tileLayer->extent().toString();

    QgsProject::instance()->addMapLayer(tileLayer);
    QList<QgsMapLayer *> currentLayers = layers();
    currentLayers.append(tileLayer);
    setLayers(currentLayers);
    refresh();

    qDebug() << "Tile layer loaded:" << dir.dirName()
             << "bands:" << nBands << "zoom:" << nMinZoom << "-" << nMaxZoom;
}

void CMapCanvas::loadRasterFile(QString rasterPath)
{
    Q_UNUSED(rasterPath)
    // Reserved — use loadRasterTiledMap for XYZ tile sets
}

// ============================================================
//  Map loading  — paths use applicationDirPath (OS-agnostic)
// ============================================================
void CMapCanvas::_loadRasterMaps()
{
    QString directoryPath = QCoreApplication::applicationDirPath() + "/../maps/RasterRepository/";
    qDebug() << "Path--------------------------------------->" << directoryPath;
    QDir dir(directoryPath);

    if (!dir.exists()) {
        qDebug() << "No raster folder found in directory:" << directoryPath;
        return;
    }

    QFileInfoList rasterFiles = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (rasterFiles.isEmpty()) {
        qDebug() << "No raster sub-folders found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : rasterFiles)
        loadRasterTiledMap(fileInfo.absoluteFilePath());
}

void CMapCanvas::_loadVectorMaps()
{
    QString directoryPath = QCoreApplication::applicationDirPath() + "/../maps/ShpRepository/";
    QDir dir(directoryPath);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << directoryPath;
        return;
    }

    QStringList filters; filters << "*.shp";
    QFileInfoList shpFiles = dir.entryInfoList(filters, QDir::Files);

    if (shpFiles.isEmpty()) {
        qDebug() << "No shapefiles found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : shpFiles)
        loadShapeFile(fileInfo.absoluteFilePath());
}

void CMapCanvas::_loadLayers() { }

void CMapCanvas::enforceLayerOrder()
{
    QList<QgsMapLayer *> orderedLayers;

    for (QgsMapLayer *layer : QgsProject::instance()->mapLayers().values())
        if (layer->type() == Qgis::LayerType::Vector)
            orderedLayers.append(layer);

    for (QgsMapLayer *layer : QgsProject::instance()->mapLayers().values())
        if (layer->type() == Qgis::LayerType::Raster)
            orderedLayers.append(layer);

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
        delete layer; return;
    }
    layer->setBlendMode(QPainter::CompositionMode_SourceOver);

    QgsFillSymbol *symbol = new QgsFillSymbol();
    symbol->deleteSymbolLayer(0);

    QgsSimpleLineSymbolLayer *outlineLayer = new QgsSimpleLineSymbolLayer();
    outlineLayer->setColor(QColor("#222222"));
    outlineLayer->setWidth(0.3);
    symbol->appendSymbolLayer(outlineLayer);

    QgsSingleSymbolRenderer *renderer = dynamic_cast<QgsSingleSymbolRenderer *>(layer->renderer());
    if (renderer)
        renderer->setSymbol(symbol);
    else {
        renderer = new QgsSingleSymbolRenderer(symbol);
        layer->setRenderer(renderer);
    }
    layer->triggerRepaint();

    QgsProject::instance()->addMapLayer(layer);
    QList<QgsMapLayer *> currentLayers = layers();
    currentLayers.append(layer);
    setLayers(currentLayers);
}

// ============================================================
//  Navigation
// ============================================================
void CMapCanvas::mapHome()
{
    double indiaMinLon = 65.0, indiaMaxLon = 100.0;
    double indiaMinLat = 5.0,  indiaMaxLat = 40.0;
    QgsRectangle indiaExtent(indiaMinLon, indiaMinLat, indiaMaxLon, indiaMaxLat);
    setRenderFlag(false);
    setExtent(indiaExtent);
    setRenderFlag(true);
    refresh();
    qDebug() << "Map set to India Home view";
}

void CMapCanvas::zoomBy(double factor)
{
    QgsRectangle currentExtent = extent();
    QgsPointXY center = currentExtent.center();
    double newWidth  = currentExtent.width()  * factor;
    double newHeight = currentExtent.height() * factor;

    QgsRectangle newExtent(
        center.x() - newWidth  / 2, center.y() - newHeight / 2,
        center.x() + newWidth  / 2, center.y() + newHeight / 2);

    if (factor > 1) {
        if (newExtent.width()  > mWorldExtentPadded.width() ||
            newExtent.height() > mWorldExtentPadded.height())
            newExtent = mWorldExtentPadded;

        if (newExtent.height() > 180.0) {
            double ratio = 180.0 / newExtent.height();
            newHeight = 180.0; newWidth = newWidth * ratio;
            newExtent = QgsRectangle(
                center.x() - newWidth / 2, -90.0,
                center.x() + newWidth / 2,  90.0);
        }
    }
    setRenderFlag(false); setExtent(newExtent); setRenderFlag(true);
}

void CMapCanvas::panCanvas(double dx, double dy)
{
    QgsRectangle currentExtent = extent();
    currentExtent.setXMinimum(currentExtent.xMinimum() + dx);
    currentExtent.setXMaximum(currentExtent.xMaximum() + dx);
    currentExtent.setYMinimum(currentExtent.yMinimum() + dy);
    currentExtent.setYMaximum(currentExtent.yMaximum() + dy);

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
    setRenderFlag(false); setExtent(currentExtent); setRenderFlag(true);
}

// ============================================================
//  Mouse / keyboard events  (unchanged from original)
// ============================================================
void CMapCanvas::wheelEvent(QWheelEvent *event)
{
    const double zoomFactor = 1.1;
    double factor = (event->angleDelta().y() > 0) ? (1.0 / zoomFactor) : zoomFactor;

    QPoint mousePos = event->pos();
    QgsPointXY mapPointBeforeZoom = getCoordinateTransform()->toMapCoordinates(mousePos.x(), mousePos.y());

    double dScale = scale() / 1000.0;
    if (factor < 1 && dScale < 1.1) return;

    setRenderFlag(false);
    zoomByFactor(factor);

    QgsPointXY after = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());
    QgsPointXY center1 = center() + (mapPointBeforeZoom - after);
    setCenter(center1);

    setRenderFlag(true);
    refresh();
}

void CMapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
        _m_nCurrentObjectClassForLoading == VISTAR_CLASS_NONE &&
        !_m_bPathGenerationMode)
    {
        mLastMousePos = event->pos();
        mPanning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CMapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (mPanning) {
        QPoint delta = event->pos() - mLastMousePos;
        double dx = -delta.x() * mapSettings().mapUnitsPerPixel();
        double dy =  delta.y() * mapSettings().mapUnitsPerPixel();
        panCanvas(dx, dy);
        mLastMousePos = event->pos();
    }

    QgsPointXY mapPoint = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());
    QgsCoordinateTransform transform(mapSettings().destinationCrs(), _m_crs, QgsProject::instance());
    QgsPointXY geoPoint = transform.transform(mapPoint);
    emit signalMouseRead(QString("Lon : %1°    Lat : %2° ")
                             .arg(geoPoint.x(), 11, 'f', 6)
                             .arg(geoPoint.y(), 10, 'f', 6));
}

void CMapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mPanning) {
        mPanning = false;
        setCursor(mPreviousCursor);

        QPoint delta = event->pos() - mLastMousePos;
        bool wasJustAClick = (std::abs(delta.x()) < 5 && std::abs(delta.y()) < 5);

        if (wasJustAClick && _m_nCurrentObjectClassForLoading == VISTAR_CLASS_NONE) {
            bool anySelected = false;

            for (CVistarObject *item : _m_listVistarObjects) {
                QgsPointXY screenPoint = mapSettings().mapToPixel().transform(item->getPoint());
                int nRadius = 20;
                QRectF rect(screenPoint.x() - nRadius/2, screenPoint.y() - nRadius/2, nRadius, nRadius);
                if (rect.contains(event->pos())) {
                    item->setHighlighted(true); anySelected = true;
                } else {
                    item->setHighlighted(false);
                }
            }

            for (CVistarRoute *item : _m_listVistarRoutes) {
                item->setHighlighted(false);
                for (int i = 0; i < item->getPointCount(); i++) {
                    QgsPointXY screenPoint = mapSettings().mapToPixel().transform(item->getPointAt(i));
                    int nRadius = 20;
                    QRectF rect(screenPoint.x()-nRadius/2, screenPoint.y()-nRadius/2, nRadius, nRadius);
                    if (rect.contains(event->pos())) {
                        item->setHighlighted(true); anySelected = true; break;
                    }
                }
            }

            if (anySelected) refresh();
        }

    } else if (event->button() == Qt::LeftButton) {

        mPanning = false;
        QgsPointXY mapPoint = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());
        QgsCoordinateTransform transform(mapSettings().destinationCrs(), _m_crs, QgsProject::instance());
        QgsPointXY geoPoint = transform.transform(mapPoint);

        // --- Path generation ---
        if (_m_bPathGenerationMode) {
            if (!_m_bPathStartPointSet) {
                _m_pathStartPoint = geoPoint;
                _m_bPathStartPointSet = true;

                QgsPointXY screenStartPt = mapSettings().mapToPixel().transform(geoPoint);
                _m_pathStartMarker = scene()->addEllipse(
                    screenStartPt.x() - 8, screenStartPt.y() - 8, 16, 16,
                    QPen(Qt::green, 3), QBrush(QColor(0, 255, 0, 100)));
                _m_pathStartMarker->setZValue(1000);

                showPathGenerationInstruction("Select END point for " +
                    CPathGenerator::getPathTypeName(_m_currentPathType) + " path");
                qDebug() << "Path start point set:" << geoPoint.x() << "," << geoPoint.y();
            } else {
                QgsPointXY endPoint = geoPoint;
                qDebug() << "Path end point set:" << endPoint.x() << "," << endPoint.y();
                qDebug() << "Using configuration: waypoints=" << _m_pathParams.numWaypoints
                         << ", altitude=" << _m_pathParams.defaultAltitude
                         << ", curveFactor=" << _m_pathParams.curveFactor;

                QList<QgsPointXYZ> pathPoints = _m_pathGenerator->generatePath(
                    _m_pathStartPoint, endPoint, _m_currentPathType, _m_pathParams);
                createGeneratedRoute(pathPoints);

                _m_currentPathType = PATH_TYPE_NONE;
                _m_bPathGenerationMode = false;
                _m_bPathStartPointSet = false;
                mPreviousCursor = Qt::ArrowCursor;
                setCursor(Qt::ArrowCursor);
                clearPathGenerationMarkers();
                emit signalClearObjectSelection();
            }
            return;
        }

        // --- Route placement ---
        if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_ROUTE) {
            if (_m_sCurrentRoute.isEmpty()) {
                QString sObjectId = GenerateObjectIdFromClass(_m_nCurrentObjectClassForLoading);
                if (!sObjectId.isEmpty()) {
                    CVistarRoute *vistarRoute = new CVistarRoute(this, sObjectId, geoPoint.x(), geoPoint.y());
                    _m_listVistarRoutes.insert(sObjectId, vistarRoute);
                    _m_sCurrentRoute = sObjectId;
                }
            } else {
                CVistarRoute *vistarRoute = getVistarRouteById(_m_sCurrentRoute);
                if (vistarRoute) {
                    if (vistarRoute->getPointCount() < 10)
                        vistarRoute->addPoint(geoPoint);
                    if (vistarRoute->getPointCount() >= 10) {
                        _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
                        mPreviousCursor = Qt::ArrowCursor;
                        _m_sCurrentRoute = "";
                        setCursor(mPreviousCursor);
                        signalClearObjectSelection();
                        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
                    }
                }
            }

        // --- Object placement ---
        } else if (_m_nCurrentObjectClassForLoading != VISTAR_CLASS_NONE) {
            QString sObjectId = GenerateObjectIdFromClass(_m_nCurrentObjectClassForLoading);
            qDebug() << "in VISTAR object" << sObjectId;
            if (!sObjectId.isEmpty()) {
                CVistarObject *vistarObject = new CVistarObject(
                    this, sObjectId, _m_nCurrentObjectClassForLoading, geoPoint.x(), geoPoint.y());
                _m_listVistarObjects.insert(sObjectId, vistarObject);
                _m_listVistarObjectIds.append(sObjectId);
                qDebug() << "VISTAR object added, ID=" << sObjectId;

                if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_RADAR)
                    emit signalRadarObjectAdded(sObjectId, RadarView::RadarCoverageParameters{});

                if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_CLUTTER) {
                    CClutterParamsDialog dlg(this);
                    if (dlg.exec() == QDialog::Accepted)
                        vistarObject->setClutterParams(dlg.getClutterParams());
                }

                if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_LAUNCHER ||
                    _m_nCurrentObjectClassForLoading == VISTAR_CLASS_FIGHTER  ||
                    _m_nCurrentObjectClassForLoading == VISTAR_CLASS_UAV)
                {
                    eVISTAR_CLASS mslClass = VISTAR_CLASS_MISSILE;
                    int nChildMslCount = 4;
                    if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_FIGHTER)     nChildMslCount = 0;
                    else if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_LAUNCHER) nChildMslCount = 12;

                    for (int i = 1; i <= nChildMslCount; i++) {
                        QString sChildObjectId = GenerateObjectIdFromClass(mslClass);
                        if (!sChildObjectId.isEmpty()) {
                            CVistarObject *child = new CVistarObject(
                                this, sChildObjectId, mslClass, geoPoint.x(), geoPoint.y());
                            child->setParent(sObjectId, i);
                            _m_listVistarObjects.insert(sChildObjectId, child);
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
            QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
        }

    } else if (event->button() == Qt::RightButton) {
        showContextMenu(event->pos());
    }
}

QString CMapCanvas::GenerateObjectIdFromClass(int nClass)
{
    QString sObjectId = "", sClass = "";
    switch (nClass) {
    case VISTAR_CLASS_DRONE:       sClass = "DRONE";        break;
    case VISTAR_CLASS_DRONE_SWARM: sClass = "SWARM";        break;
    case VISTAR_CLASS_FIGHTER:     sClass = "FIGHTER";      break;
    case VISTAR_CLASS_UAV:         sClass = "UAV";          break;
    case VISTAR_CLASS_RADAR:       sClass = "RADAR";        break;
    case VISTAR_CLASS_LAUNCHER:    sClass = "LAUNCHER";     break;
    case VISTAR_CLASS_MISSILE:     sClass = "MISSILE";      break;
    case VISTAR_CLASS_ROUTE:       sClass = "ROUTE";        break;
    case VISTAR_CLASS_JAMMER:      sClass = "JAMMER";       break;
    case VISTAR_CLASS_CLUTTER:     sClass = "CLUTTER";      break;
    case VISTAR_CLASS_RF_DETECTOR: sClass = "RF_DETECTOR";  break;
    }

    if (!sClass.isEmpty()) {
        for (int i = 1; i < 100; i++) {
            QString sTempId = sClass + "_" + QString::number(i);
            bool exists = (nClass == VISTAR_CLASS_ROUTE)
                              ? isVistarRouteByIdExists(sTempId)
                              : isVistarObjectByIdExists(sTempId);
            if (!exists) { sObjectId = sTempId; break; }
        }
    }
    return sObjectId;
}

CVistarObject *CMapCanvas::getVistarObjectById(QString sObjectId)
{
    if (_m_listVistarObjects.contains(sObjectId)) { qDebug() << "Not null"; return _m_listVistarObjects.value(sObjectId); }
    qDebug() << "Null"; return nullptr;
}

CVistarRoute *CMapCanvas::getVistarRouteById(QString sObjectId)
{
    return _m_listVistarRoutes.value(sObjectId, nullptr);
}

bool CMapCanvas::isVistarObjectByIdExists(QString sObjectId) { return _m_listVistarObjects.contains(sObjectId); }
bool CMapCanvas::isVistarRouteByIdExists(QString sObjectId)  { return _m_listVistarRoutes.contains(sObjectId); }

void CMapCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_ROUTE) {
            _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
            mPreviousCursor = Qt::ArrowCursor;
            _m_sCurrentRoute = "";
            setCursor(mPreviousCursor);
            signalClearObjectSelection();
            QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
        } else if (_m_nCurrentObjectClassForLoading == VISTAR_CLASS_NONE) {
            for (CVistarObject *item : _m_listVistarObjects) {
                QgsPointXY sp = mapSettings().mapToPixel().transform(item->getPoint());
                int nRadius = 20;
                QRectF rect(sp.x()-nRadius/2, sp.y()-nRadius/2, nRadius, nRadius);
                item->setHighlighted(rect.contains(e->pos()));
            }
            for (CVistarRoute *item : _m_listVistarRoutes) {
                item->setHighlighted(false);
                for (int i = 0; i < item->getPointCount(); i++) {
                    QgsPointXY sp = mapSettings().mapToPixel().transform(item->getPointAt(i));
                    int nRadius = 20;
                    QRectF rect(sp.x()-nRadius/2, sp.y()-nRadius/2, nRadius, nRadius);
                    if (rect.contains(e->pos())) { item->setHighlighted(true); break; }
                }
            }
            refresh();
        }
    }
    QgsMapCanvas::mouseDoubleClickEvent(e);
}

void CMapCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (_m_bPathGenerationMode) { cancelPathGeneration(); return; }
        if (_m_nCurrentObjectClassForLoading != VISTAR_CLASS_NONE) {
            _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
            mPreviousCursor = Qt::ArrowCursor;
            _m_sCurrentRoute = "";
            setCursor(mPreviousCursor);
            emit signalClearObjectSelection(); return;
        }
    }

    QgsRectangle ext = this->extent();
    double moveX = ext.width() * 0.1;
    switch (event->key()) {
    case Qt::Key_Home:       mapHome();          break;
    case Qt::Key_Left:       panCanvas(-moveX,0); break;
    case Qt::Key_Right:      panCanvas( moveX,0); break;
    case Qt::Key_Up:         panCanvas(0, moveX); break;
    case Qt::Key_Down:       panCanvas(0,-moveX); break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:      zoomBy(1.0/1.1);    break;
    case Qt::Key_Underscore:
    case Qt::Key_Minus:      zoomBy(1.1);         break;
    default: QgsMapCanvas::keyPressEvent(event);  break;
    }
}

void CMapCanvas::SetObjectToLoadOnClick(int nClass)
{
    _m_nCurrentObjectClassForLoading = nClass;
    QString strPath = ""; int nSize = 50;
    switch (nClass) {
    case VISTAR_CLASS_DRONE:       strPath = ":/icons/cursor/drone.png";        nSize = 40; break;
    case VISTAR_CLASS_DRONE_SWARM: strPath = ":/icons/cursor/drone_swarm.png";              break;
    case VISTAR_CLASS_FIGHTER:     strPath = ":/icons/cursor/fighter.png";                  break;
    case VISTAR_CLASS_UAV:         strPath = ":/icons/cursor/uav.png";                      break;
    case VISTAR_CLASS_RADAR:       strPath = ":/icons/cursor/radar.png";        nSize = 40; break;
    case VISTAR_CLASS_LAUNCHER:    strPath = ":/icons/cursor/launcher.png";                 break;
    case VISTAR_CLASS_MISSILE:     strPath = ":/icons/cursor/missile.png";                  break;
    case VISTAR_CLASS_ROUTE:       strPath = ":/icons/cursor/route.png";        nSize = 40; break;
    case VISTAR_CLASS_JAMMER:      strPath = ":/icons/cursor/jammer.png";       nSize = 44; break;
    case VISTAR_CLASS_CLUTTER:     strPath = ":/icons/cursor/clutter.png";      nSize = 44; break;
    case VISTAR_CLASS_RF_DETECTOR: strPath = ":/icons/cursor/rf_detector.png";  nSize = 44; break;
    }

    if (strPath.isEmpty()) {
        mPreviousCursor = Qt::ArrowCursor; setCursor(Qt::ArrowCursor);
    } else {
        QPixmap cursorPixmap(strPath);
        QPixmap scaledPix = cursorPixmap.scaled(nSize, nSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int nHostY = (nClass == VISTAR_CLASS_ROUTE) ? -10 : -1;
        QCursor customCursor(scaledPix, -1, nHostY);
        mPreviousCursor = customCursor;
        setCursor(customCursor);
    }
}

void CMapCanvas::InitializeAllObjects()
{
    for (CVistarRoute *vistarRoute : _m_listVistarRoutes)
        vistarRoute->TransmitSelfInfo();

    for (const QString &vistarObjectId : _m_listVistarObjectIds) {
        qDebug() << vistarObjectId;
        CVistarObject *vistarObject = _m_listVistarObjects.value(vistarObjectId);
        if (vistarObject) vistarObject->TransmitSelfInfo();
    }
}

void CMapCanvas::slotUpdateObject(QJsonDocument doc)
{
    QJsonObject jsonObject = doc.object();
    if (jsonObject["SRC"].toString() == VISTAR_SRC_MISSION_PLANNER) return;

    QString sStream = jsonObject["STREAM"].toString();
    QString sId     = jsonObject["ID"].toString();
    qDebug() << "Data recvd here" << sId << doc.toJson();

    CVistarObject *vistarObject = getVistarObjectById(sId);
    if (!vistarObject) return;

    if (sStream.contains("create") || sStream.contains("update"))
        vistarObject->UpdateObject(jsonObject);
    else if (sStream.contains("action") &&
             jsonObject["ACTION"].toString().contains("destroy")) {
        _m_listVistarObjects.remove(sId);
        _m_listVistarObjectIds.removeAll(sId);
        delete vistarObject;
    }
}

void CMapCanvas::showContextMenu(QPoint pos)
{
    QList<QMenu *> listMenus;
    QMenu menu;

    for (CVistarObject *vistarObject : _m_listVistarObjects) {
        if (!vistarObject->isHighlighted()) continue;

        QMenu *menuObject = new QMenu(); listMenus << menuObject;
        menuObject->setTitle(vistarObject->getObjectId());

        QMenu *menuAttachRoute = new QMenu(); listMenus << menuAttachRoute;
        menuAttachRoute->setTitle("Attach Route");

        QAction *actionUpdate = menuObject->addAction("Update");
        actionUpdate->setObjectName(vistarObject->getObjectId());

        for (CVistarRoute *vistarRoute : _m_listVistarRoutes) {
            QAction *action = menuAttachRoute->addAction(vistarRoute->getObjectId());
            action->setObjectName(vistarObject->getObjectId());
        }
        if (menuAttachRoute->actions().count() > 0)
            menuObject->addMenu(menuAttachRoute);

        if (vistarObject->objectClass() == VISTAR_CLASS_RADAR) {
            QAction *actionDesignParams = menuObject->addAction("Design Parameters");
            actionDesignParams->setObjectName(vistarObject->getObjectId());
            QAction *actionCoverage = menuObject->addAction("Update Coverage Parameters");
            actionCoverage->setObjectName(vistarObject->getObjectId());
        }

        QAction *actionDelete = menuObject->addAction("Delete");
        actionDelete->setObjectName(vistarObject->getObjectId());

        if (menuObject->actions().count() > 0) menu.addMenu(menuObject);
    }

    for (CVistarRoute *vistarRoute : _m_listVistarRoutes) {
        if (!vistarRoute->isHighlighted()) continue;

        QMenu *menuObject = new QMenu(); listMenus << menuObject;
        menuObject->setTitle(vistarRoute->getObjectId());

        QAction *actionUpdate = menuObject->addAction("Update");
        actionUpdate->setObjectName(vistarRoute->getObjectId());
        QAction *actionDelete = menuObject->addAction("Delete");
        actionDelete->setObjectName(vistarRoute->getObjectId());

        if (menuObject->actions().count() > 0) menu.addMenu(menuObject);
    }

    if (menu.actions().count() > 0) {
        QAction *selected = menu.exec(mapToGlobal(pos));
        if (selected) {
            qDebug() << selected->text() << selected->objectName();
            CVistarObject *object = getVistarObjectById(selected->objectName());
            if (object) {
                if (selected->text() == "Update") {
                    _m_objUpdatePosition.setObjectId(selected->objectName());
                    QgsPointXYZ pt = object->getPointXYZ();
                    _m_objUpdatePosition.setPosition(pt.y(), pt.x(), pt.z());
                    _m_objUpdatePosition.show();

                } else if (selected->text() == "Delete") {
                    _m_listVistarObjects.remove(selected->objectName());
                    for (int i = 0; i < _m_listVistarObjectIds.size(); i++) {
                        CVistarObject *objectTemp = getVistarObjectById(_m_listVistarObjectIds.at(i));
                        if (objectTemp && objectTemp->getParent() == selected->objectName() &&
                            objectTemp->getChildId() > 0) {
                            _m_listVistarObjectIds.removeAt(i); delete objectTemp; continue;
                        }
                        if (_m_listVistarObjectIds.at(i) == selected->objectName()) {
                            _m_listVistarObjectIds.removeAt(i); break;
                        }
                    }
                    delete object;
                    QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });

                } else if (selected->text() == "Design Parameters" &&
                           object->objectClass() == VISTAR_CLASS_RADAR) {
                    QString radarId = selected->objectName();
                    auto *dlg = new RadarAttributesDialog(radarId, object->radarAttributes(), this);
                    connect(dlg, &RadarAttributesDialog::attributesApplied,
                            this, [this, radarId, object](const RadarView::RadarAttributes &attrs) {
                        object->setRadarAttributes(attrs);
                        emit signalRadarAttributesChanged(radarId, attrs);
                        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
                        object->TransmitSelfInfo();
                    });
                    dlg->exec(); dlg->deleteLater();

                } else if (selected->text() == "Update Coverage Parameters" &&
                           object->objectClass() == VISTAR_CLASS_RADAR) {
                    QString radarId = selected->objectName();
                    RadarView::RadarAttributes curAttrs = object->radarAttributes();
                    auto *dlg = new CCoverageParamsDialog(radarId, curAttrs.coverage, this);
                    connect(dlg, &CCoverageParamsDialog::coverageApplied,
                            this, [this, radarId, object](const RadarView::RadarCoverageParameters &cov) {
                        RadarView::RadarAttributes attrs = object->radarAttributes();
                        attrs.coverage = cov;
                        object->setRadarAttributes(attrs);
                        emit signalRadarCoverageChanged(radarId, cov);
                        refresh();
                        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
                    });
                    dlg->exec(); dlg->deleteLater();

                } else {
                    CVistarRoute *route = getVistarRouteById(selected->text());
                    if (route) {
                        object->attachRoute(selected->text());
                        object->setHighlighted(false);
                        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
                    }
                }
            } else {
                CVistarRoute *route = getVistarRouteById(selected->objectName());
                if (route) {
                    if (selected->text() == "Update") {
                        _m_objUpdateRoute.setObjectId(selected->objectName());
                        _m_objUpdateRoute.setPoints(route->getPoints());
                        _m_objUpdateRoute.setMANEUVERs(route->getMANEUVERs());
                        _m_objUpdateRoute.show();
                    } else if (selected->text() == "Delete") {
                        _m_listVistarRoutes.remove(selected->objectName());
                        delete route;
                        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
                    }
                }
            }
        }
    }

    for (QMenu *menu1 : listMenus) delete menu1;
}

void CMapCanvas::slotUpdatePosition(QString sObjectId, double dLat, double dLon, double dAlt)
{
    CVistarObject *vistarObject = getVistarObjectById(sObjectId);
    if (vistarObject) {
        vistarObject->UpdateLocation(dLat, dLon, dAlt);
        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
    }
}

void CMapCanvas::slotUpdatePoints(QString sObjectId, QList<QgsPointXYZ> listPoints, QStringList maneuverTypes)
{
    CVistarRoute *vistarRoute = getVistarRouteById(sObjectId);
    if (vistarRoute) {
        vistarRoute->UpdatePoints(listPoints, maneuverTypes);
        QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
    }
}

// ============================================================
//  Scenario management
// ============================================================
QString CMapCanvas::getClassNameFromEnum(int nClass)
{
    switch (nClass) {
    case VISTAR_CLASS_DRONE:       return "DRONE";
    case VISTAR_CLASS_DRONE_SWARM: return "DRONE_SWARM";
    case VISTAR_CLASS_FIGHTER:     return "FIGHTER";
    case VISTAR_CLASS_UAV:         return "UAV";
    case VISTAR_CLASS_RADAR:       return "RADAR";
    case VISTAR_CLASS_LAUNCHER:    return "LAUNCHER";
    case VISTAR_CLASS_MISSILE:     return "MISSILE";
    case VISTAR_CLASS_ROUTE:       return "ROUTE";
    case VISTAR_CLASS_JAMMER:      return "JAMMER";
    case VISTAR_CLASS_CLUTTER:     return "CLUTTER";
    case VISTAR_CLASS_RF_DETECTOR: return "RF_DETECTOR";
    default:                       return "UNKNOWN";
    }
}

int CMapCanvas::getEnumFromClassName(const QString &className)
{
    if (className == "DRONE")        return VISTAR_CLASS_DRONE;
    if (className == "DRONE_SWARM")  return VISTAR_CLASS_DRONE_SWARM;
    if (className == "FIGHTER")      return VISTAR_CLASS_FIGHTER;
    if (className == "UAV")          return VISTAR_CLASS_UAV;
    if (className == "RADAR")        return VISTAR_CLASS_RADAR;
    if (className == "LAUNCHER")     return VISTAR_CLASS_LAUNCHER;
    if (className == "MISSILE")      return VISTAR_CLASS_MISSILE;
    if (className == "ROUTE")        return VISTAR_CLASS_ROUTE;
    if (className == "JAMMER")       return VISTAR_CLASS_JAMMER;
    if (className == "CLUTTER")      return VISTAR_CLASS_CLUTTER;
    if (className == "RF_DETECTOR")  return VISTAR_CLASS_RF_DETECTOR;
    return VISTAR_CLASS_NONE;
}

Scenario CMapCanvas::createScenarioFromCurrentState()
{
    Scenario scenario;
    scenario.name        = "Auto-saved Scenario";
    scenario.description = "Automatically saved vistar routes and objects";
    scenario.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);

    for (CVistarRoute *vistarRoute : _m_listVistarRoutes) {
        if (!vistarRoute) continue;
        ScenarioRoute route;
        route.id = route.name = vistarRoute->getObjectId();
        QList<QgsPointXYZ> points = vistarRoute->getPoints();
        QStringList MANEUVERs = vistarRoute->getMANEUVERs();
        for (int i = 0; i < points.size(); i++) {
            const QgsPointXYZ &pt = points.at(i);
            route.waypoints.append(QPointF(pt.y(), pt.x()));
            route.altitudes.append(pt.z());
            route.maneuverTypes.append(i < MANEUVERs.size() ? MANEUVERs.at(i) : "DIRECT");
        }
        scenario.routes.append(route);
    }

    for (const QString &objectId : _m_listVistarObjectIds) {
        CVistarObject *vistarObject = _m_listVistarObjects.value(objectId);
        if (!vistarObject || vistarObject->getChildId() != 0) continue;

        ScenarioObject obj;
        obj.id = vistarObject->getObjectId();
        QgsPointXYZ pt = vistarObject->getPointXYZ();
        obj.latitude = pt.y(); obj.longitude = pt.x(); obj.altitude = pt.z();
        obj.type = objectId.split("_").first();

        obj.additionalData["parent"]        = vistarObject->getParent();
        obj.additionalData["childId"]       = vistarObject->getChildId();
        obj.additionalData["attachedRoute"] = vistarObject->getAttachedRoute();

        if (vistarObject->objectClass() == VISTAR_CLASS_RADAR) {
            const RadarView::RadarAttributes &attrs = vistarObject->radarAttributes();
            obj.additionalData["radarAttributes"] = attrs.toJson();
            obj.additionalData["range"] = attrs.coverage.maxRangeKm * 1000.0;
        }
        scenario.objects.append(obj);
    }

    scenario.metadata["version"]   = "1.0";
    scenario.metadata["autoSaved"] = true;
    return scenario;
}

void CMapCanvas::resetScenario()
{
    qDebug() << "Resetting scenario - clearing all objects and routes";
    for (CVistarRoute *route : _m_listVistarRoutes) delete route;
    _m_listVistarRoutes.clear();
    for (CVistarObject *obj : _m_listVistarObjects) delete obj;
    _m_listVistarObjects.clear();
    _m_listVistarObjectIds.clear();
    refresh();
    emit signalScenarioCleared();
    qDebug() << "Scenario reset complete";
}

void CMapCanvas::loadScenarioToCanvas(const Scenario &scenario)
{
    emit signalScenarioCleared();
    qDebug() << "Loading scenario:" << scenario.name;

    for (CVistarRoute *route : _m_listVistarRoutes) delete route;
    _m_listVistarRoutes.clear();
    for (CVistarObject *obj : _m_listVistarObjects) delete obj;
    _m_listVistarObjects.clear();
    _m_listVistarObjectIds.clear();

    for (const ScenarioRoute &route : scenario.routes) {
        if (route.waypoints.isEmpty()) continue;
        QPointF firstPt = route.waypoints.first();
        CVistarRoute *vistarRoute = new CVistarRoute(this, route.id, firstPt.y(), firstPt.x());
        for (int i = 1; i < route.waypoints.size(); i++) {
            QPointF pt = route.waypoints[i];
            vistarRoute->addPoint(QgsPointXY(pt.y(), pt.x()));
        }
        if (!route.maneuverTypes.isEmpty()) vistarRoute->setMANEUVERs(route.maneuverTypes);
        _m_listVistarRoutes.insert(route.id, vistarRoute);
        qDebug() << "Loaded route:" << route.id << "with" << route.waypoints.size() << "waypoints";
    }

    for (const ScenarioObject &obj : scenario.objects) {
        int nClass = getEnumFromClassName(obj.type);
        if (nClass == VISTAR_CLASS_NONE) continue;

        CVistarObject *vistarObject = new CVistarObject(
            this, obj.id, nClass, obj.longitude, obj.latitude);
        vistarObject->UpdateLocation(obj.latitude, obj.longitude, obj.altitude);

        if (obj.additionalData.contains("attachedRoute")) {
            QString attachedRoute = obj.additionalData["attachedRoute"].toString();
            if (!attachedRoute.isEmpty()) vistarObject->attachRoute(attachedRoute);
        }

        _m_listVistarObjects.insert(obj.id, vistarObject);
        _m_listVistarObjectIds.append(obj.id);

        if (nClass == VISTAR_CLASS_RADAR) {
            RadarView::RadarAttributes attrs = RadarView::RadarAttributes::defaults();
            if (obj.additionalData.contains("radarAttributes"))
                attrs = RadarView::RadarAttributes::fromJson(obj.additionalData["radarAttributes"].toObject());
            else if (obj.additionalData.contains("range"))
                attrs.coverage.maxRangeKm = obj.additionalData["range"].toDouble() / 1000.0;
            vistarObject->setRadarAttributes(attrs);
            emit signalRadarObjectAdded(obj.id, attrs.coverage);
        }

        qDebug() << "Loaded object:" << obj.id << "at" << obj.latitude << "," << obj.longitude;

        if (nClass == VISTAR_CLASS_LAUNCHER || nClass == VISTAR_CLASS_FIGHTER || nClass == VISTAR_CLASS_UAV) {
            int nChildMslCount = (nClass == VISTAR_CLASS_LAUNCHER) ? 12 : (nClass == VISTAR_CLASS_UAV) ? 4 : 0;
            for (int i = 1; i <= nChildMslCount; i++) {
                QString sChildObjectId = GenerateObjectIdFromClass(VISTAR_CLASS_MISSILE);
                if (!sChildObjectId.isEmpty()) {
                    CVistarObject *child = new CVistarObject(
                        this, sChildObjectId, VISTAR_CLASS_MISSILE, obj.longitude, obj.latitude);
                    child->setParent(obj.id, i);
                    _m_listVistarObjects.insert(sChildObjectId, child);
                    _m_listVistarObjectIds.append(sChildObjectId);
                }
            }
        }
    }

    refresh();
    qDebug() << "Scenario loaded successfully with" << scenario.objects.size()
             << "objects and" << scenario.routes.size() << "routes";
}

bool CMapCanvas::saveCurrentScenario(const QString &filePath)
{
    Scenario scenario = createScenarioFromCurrentState();
    QString savePath = filePath.isEmpty()
                           ? _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json"
                           : filePath;
    bool success = _m_scenarioManager->saveScenario(scenario, savePath);
    if (success) qDebug() << "Scenario saved successfully to:" << savePath;
    else         qWarning() << "Failed to save scenario to:" << savePath;
    return success;
}

bool CMapCanvas::loadScenario(const QString &filePath)
{
    QString loadPath = filePath.isEmpty()
                           ? _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json"
                           : filePath;
    if (!QFileInfo(loadPath).exists()) {
        qDebug() << "Scenario file does not exist:" << loadPath; return false;
    }
    Scenario scenario;
    bool success = _m_scenarioManager->loadScenario(loadPath, scenario);
    if (success) { loadScenarioToCanvas(scenario); qDebug() << "Scenario loaded from:" << loadPath; }
    else         qWarning() << "Failed to load scenario from:" << loadPath;
    return success;
}

bool CMapCanvas::autoSaveScenario()
{
    if (_m_listVistarRoutes.isEmpty() && _m_listVistarObjects.isEmpty()) {
        qDebug() << "No routes or objects to auto-save"; return false;
    }
    return saveCurrentScenario();
}

bool CMapCanvas::autoLoadScenario()
{
    QString autoSavePath = _m_scenarioManager->getDefaultScenariosDirectory() + "/current_scenario.json";
    if (!QFileInfo(autoSavePath).exists()) { qDebug() << "No auto-save scenario file found"; return false; }
    qDebug() << "Auto-loading scenario from:" << autoSavePath;
    return loadScenario(autoSavePath);
}

// ============================================================
//  Path generation
// ============================================================
void CMapCanvas::startPathGeneration(eVISTAR_PATH_TYPE pathType)
{
    if (pathType == PATH_TYPE_NONE) return;

    if (_m_nCurrentObjectClassForLoading != VISTAR_CLASS_NONE) {
        _m_nCurrentObjectClassForLoading = VISTAR_CLASS_NONE;
        emit signalClearObjectSelection();
    }

    _m_currentPathType    = pathType;
    _m_bPathGenerationMode = true;
    _m_bPathStartPointSet  = false;

    QPixmap cursorPixmap(":/icons/cursor/route.png");
    QPixmap scaledPix = cursorPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPix, -1, -10);
    mPreviousCursor = customCursor;
    setCursor(customCursor);

    QString pathName = CPathGenerator::getPathTypeName(pathType);
    showPathGenerationInstruction("Select START point for " + pathName + " path");
    emit signalPathGenerationStarted(pathType);
    qDebug() << "Path generation started for:" << pathName;
}

void CMapCanvas::cancelPathGeneration()
{
    if (!_m_bPathGenerationMode) return;
    _m_currentPathType    = PATH_TYPE_NONE;
    _m_bPathGenerationMode = false;
    _m_bPathStartPointSet  = false;
    mPreviousCursor = Qt::ArrowCursor;
    setCursor(Qt::ArrowCursor);
    clearPathGenerationMarkers();
    emit signalPathGenerationCancelled();
    qDebug() << "Path generation cancelled";
}

bool              CMapCanvas::isPathGenerationActive() const { return _m_bPathGenerationMode; }
eVISTAR_PATH_TYPE CMapCanvas::getCurrentPathType()     const { return _m_currentPathType; }
CPathGenerator   *CMapCanvas::getPathGenerator()             { return _m_pathGenerator; }

void CMapCanvas::showPathGenerationInstruction(const QString &text)
{
    if (_m_pathInstructionBgRect) {
        scene()->removeItem(_m_pathInstructionBgRect);
        delete _m_pathInstructionBgRect; _m_pathInstructionBgRect = nullptr;
    }
    if (_m_pathInstructionText) {
        scene()->removeItem(_m_pathInstructionText);
        delete _m_pathInstructionText; _m_pathInstructionText = nullptr;
    }

    _m_pathInstructionText = scene()->addText(text);
    _m_pathInstructionText->setDefaultTextColor(Qt::white);
    _m_pathInstructionText->setFont(QFont("Arial", 14, QFont::Bold));
    _m_pathInstructionText->setZValue(1000);

    QRectF sceneRect = scene()->sceneRect();
    double textWidth = _m_pathInstructionText->boundingRect().width();
    _m_pathInstructionText->setPos((sceneRect.width() - textWidth) / 2, 80);

    _m_pathInstructionBgRect = scene()->addRect(
        _m_pathInstructionText->boundingRect().adjusted(-10, -5, 10, 5),
        QPen(Qt::transparent), QBrush(QColor(0, 100, 200, 180)));
    _m_pathInstructionBgRect->setPos(_m_pathInstructionText->pos() + QPointF(-10, -5));
    _m_pathInstructionBgRect->setZValue(999);
    refresh();
}

void CMapCanvas::clearPathGenerationMarkers()
{
    if (_m_pathStartMarker) {
        scene()->removeItem(_m_pathStartMarker); delete _m_pathStartMarker; _m_pathStartMarker = nullptr;
    }
    if (_m_pathInstructionBgRect) {
        scene()->removeItem(_m_pathInstructionBgRect); delete _m_pathInstructionBgRect; _m_pathInstructionBgRect = nullptr;
    }
    if (_m_pathInstructionText) {
        scene()->removeItem(_m_pathInstructionText); delete _m_pathInstructionText; _m_pathInstructionText = nullptr;
    }
    refresh();
}

void CMapCanvas::createGeneratedRoute(const QList<QgsPointXYZ> &points)
{
    if (points.isEmpty()) { qDebug() << "No points to create route"; return; }

    QString sObjectId = GenerateObjectIdFromClass(VISTAR_CLASS_ROUTE);
    if (sObjectId.isEmpty()) { qDebug() << "Failed to generate route ID"; return; }

    QgsPointXYZ firstPt = points.first();
    CVistarRoute *vistarRoute = new CVistarRoute(this, sObjectId, firstPt.x(), firstPt.y());
    for (int i = 1; i < points.size(); i++)
        vistarRoute->addPoint(QgsPointXY(points[i].x(), points[i].y()));

    _m_listVistarRoutes.insert(sObjectId, vistarRoute);
    QTimer::singleShot(100, this, [this]() { autoSaveScenario(); });
    emit signalPathGenerationCompleted(sObjectId);
    qDebug() << "Generated route:" << sObjectId << "with" << points.size() << "waypoints";
    refresh();
}

// ============================================================
//  Path parameter accessors
// ============================================================
void CMapCanvas::setPathParameters(const CPathGenerator::PathParameters &params)
{
    _m_pathParams = params;
    qDebug() << "Path parameters updated - waypoints:" << _m_pathParams.numWaypoints
             << ", altitude:" << _m_pathParams.defaultAltitude;
}

CPathGenerator::PathParameters CMapCanvas::getPathParameters() const { return _m_pathParams; }

void   CMapCanvas::setNumWaypoints(int n)       { _m_pathParams.numWaypoints    = qBound(2,    n,   100); qDebug()<<"Waypoints:"<<_m_pathParams.numWaypoints; }
int    CMapCanvas::getNumWaypoints()      const { return _m_pathParams.numWaypoints; }
void   CMapCanvas::setDefaultAltitude(double v) { _m_pathParams.defaultAltitude = qMax(0.0,   v);         qDebug()<<"Altitude:"<<v; }
double CMapCanvas::getDefaultAltitude()   const { return _m_pathParams.defaultAltitude; }
void   CMapCanvas::setCurveFactor(double v)     { _m_pathParams.curveFactor     = qBound(0.0, v,   1.0);  qDebug()<<"CurveFactor:"<<v; }
double CMapCanvas::getCurveFactor()       const { return _m_pathParams.curveFactor; }
void   CMapCanvas::setSpiralTurns(double v)     { _m_pathParams.spiralTurns     = qBound(0.5, v,  10.0);  qDebug()<<"SpiralTurns:"<<v; }
double CMapCanvas::getSpiralTurns()       const { return _m_pathParams.spiralTurns; }
void   CMapCanvas::setZigzagAmplitude(double v) { _m_pathParams.zigzagAmplitude = qBound(0.05,v,   0.5);  qDebug()<<"ZigzagAmplitude:"<<v; }
double CMapCanvas::getZigzagAmplitude()   const { return _m_pathParams.zigzagAmplitude; }
void   CMapCanvas::setZigzagFrequency(int v)    { _m_pathParams.zigzagFrequency = qBound(2,   v,    20);  qDebug()<<"ZigzagFrequency:"<<v; }
int    CMapCanvas::getZigzagFrequency()   const { return _m_pathParams.zigzagFrequency; }
void   CMapCanvas::setMaxTurnRadius(double v)   { _m_pathParams.maxTurnRadius   = qBound(0.01,v,   0.2);  qDebug()<<"MaxTurnRadius:"<<v; }
double CMapCanvas::getMaxTurnRadius()     const { return _m_pathParams.maxTurnRadius; }
void   CMapCanvas::setRandomVariance(double v)  { _m_pathParams.randomVariance  = qBound(0.05,v,   0.5);  qDebug()<<"RandomVariance:"<<v; }
double CMapCanvas::getRandomVariance()    const { return _m_pathParams.randomVariance; }
void   CMapCanvas::setSpreadRadiusKm(double v)  { _m_pathParams.spreadRadiusKm  = qBound(0.5, v, 100.0);  qDebug()<<"SpreadRadius:"<<v<<"km"; }
double CMapCanvas::getSpreadRadiusKm()    const { return _m_pathParams.spreadRadiusKm; }
