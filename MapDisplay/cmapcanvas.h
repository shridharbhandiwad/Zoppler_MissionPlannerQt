#ifndef CMAPCANVAS_H
#define CMAPCANVAS_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QMap>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include <qgsproject.h>
#include <qgscoordinatereferencesystem.h>

class CMapCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit CMapCanvas(QWidget *parent = nullptr);
    ~CMapCanvas();
    
    // Initialization methods
    void Initialize();
    void InitializeAllObjects();
    
    // Object management
    void SetObjectToLoadOnClick(int nClass);
    
    // Map import
    void importRasterMap(const QString &filePath);
    
    // Get the actual QGIS map canvas
    QgsMapCanvas* getMapCanvas() { return m_mapCanvas; }

signals:
    void signalMouseRead(QString mouseInfo);
    void signalClearObjectSelection();

public slots:
    void slotUpdateObject(QJsonDocument doc);
    void slotLoadRoute(QJsonDocument doc);

private slots:
    void onMapCanvasMouseMove(const QgsPointXY &point);

private:
    QgsMapCanvas *m_mapCanvas;
    QVBoxLayout *m_layout;
    QgsRasterLayer *m_rasterLayer;
    QgsVectorLayer *m_objectLayer;
    QMap<QString, QJsonObject> m_objects;
    QMap<QString, QJsonObject> m_routes;
    int m_selectedObjectClass;
};

#endif // CMAPCANVAS_H
