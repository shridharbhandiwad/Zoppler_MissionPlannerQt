#ifndef CMAPCANVAS_H
#define CMAPCANVAS_H

#include <QObject>
#include <QWidget>
#include <qgsmapcanvas.h>
#include <QPoint>
#include "qgsrasterlayer.h"
#include <QProcess>
#include <QProgressDialog>
#include "cvistarobject.h"
#include "cvistarroute.h"
#include "cpathgenerator.h"
#include <QTimer>
#include "../cupdateposition.h"
#include "../cupdateroute.h"
#include "../cscenariomanager.h"

class CMapCanvas : public QgsMapCanvas
{
    Q_OBJECT

public:
    explicit CMapCanvas(QWidget *parent = nullptr);

    void Initialize();

    void mapHome();

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent( QMouseEvent *e ) override;
    void keyPressEvent(QKeyEvent *event) override;

    void loadRasterTiledMap(QString tileDir);
    void loadRasterFile(QString rasterPath);
    void importRasterMap(QString inputPath);
    void SetObjectToLoadOnClick(int nClass);
    void InitializeAllObjects();
    void showContextMenu(QPoint pos);

    // Scenario management
    bool saveCurrentScenario(const QString &filePath = "");
    bool loadScenario(const QString &filePath = "");
    bool autoSaveScenario();
    bool autoLoadScenario();
    Scenario createScenarioFromCurrentState();
    void loadScenarioToCanvas(const Scenario &scenario);
    void resetScenario();
    QString getClassNameFromEnum(int nClass);
    int getEnumFromClassName(const QString &className);

    // Path generation
    void startPathGeneration(eVISTAR_PATH_TYPE pathType);
    void cancelPathGeneration();
    bool isPathGenerationActive() const;
    eVISTAR_PATH_TYPE getCurrentPathType() const;
    CPathGenerator* getPathGenerator();

    QTimer timerUpdate;
private:

    QProcess* m_translateProcess = nullptr;
    QProcess* m_addoProcess = nullptr;
    QString m_inputPath;
    QString m_outputPath;

    int _m_nCurrentObjectClassForLoading;
    QString _m_sCurrentRoute;

    QProgressDialog* m_progressDialog = nullptr;

    QgsRectangle mWorldExtentPadded;
    void zoomBy(double factor);
    void panCanvas(double dx, double dy);

    QPoint mLastMousePos;
    QCursor mPreviousCursor;
    bool mPanning = false;

    void _loadVectorMaps();
    void loadShapeFile(const QString &shpPath);

    void _loadLayers();
    void startGdal2Tiles();
    void convertAndCacheRaster(const QString inputPath);
    void _loadRasterMaps();
    QString GenerateObjectIdFromClass(int nClass);
    CVistarObject *getVistarObjectById(QString sObjectId);
    CVistarRoute* getVistarRouteById(QString sObjectId);
    bool isVistarObjectByIdExists(QString sObjectId);
    bool isVistarRouteByIdExists(QString sObjectId);

    QList<QString> _m_listVistarObjectIds;
    QHash<QString,CVistarObject*> _m_listVistarObjects;
    QHash<QString,CVistarRoute*> _m_listVistarRoutes;

    QgsCoordinateReferenceSystem _m_crs;

    CUpdatePosition _m_objUpdatePosition;
    CUpdateRoute _m_objUpdateRoute;

    CScenarioManager *_m_scenarioManager;

    // Path generation members
    CPathGenerator *_m_pathGenerator;
    eVISTAR_PATH_TYPE _m_currentPathType;
    bool _m_bPathGenerationMode;
    QgsPointXY _m_pathStartPoint;
    bool _m_bPathStartPointSet;
    QGraphicsEllipseItem *_m_pathStartMarker;
    QGraphicsTextItem *_m_pathInstructionText;

    void enforceLayerOrder();
    int computeMaxZoom(double rasterRes);
    void showPathGenerationInstruction(const QString &text);
    void clearPathGenerationMarkers();
    void createGeneratedRoute(const QList<QgsPointXYZ> &points);
signals:
    void signalMouseRead(QString);
    void signalClearObjectSelection();
    void signalPathGenerationStarted(eVISTAR_PATH_TYPE pathType);
    void signalPathGenerationCompleted(QString routeId);
    void signalPathGenerationCancelled();
public slots:
    void slotUpdateObject(QJsonDocument doc);
    void slotUpdatePosition(QString, double dLat, double dLon, double dAlt);
    void slotUpdatePoints(QString sObjectId,QList<QgsPointXYZ> listPoints);
private slots:
    void startGdalTranslate();
    void startGdalAddo();
    void handleGdalStdout();
    void loadCachedAfterProcessing();
};

#endif // CMAPCANVAS_H
