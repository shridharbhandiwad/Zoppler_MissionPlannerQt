#ifndef CMAPCANVAS_H
#define CMAPCANVAS_H

#include <QWidget>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include "cvistarobject.h"
#include "cvistarroute.h"
#include "cmaptoolselectobjects.h"

class CMapCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit CMapCanvas(QWidget *parent = nullptr);
    ~CMapCanvas();

    void Initialize();
    void InitializeAllObjects();
    void SetObjectToLoadOnClick(int nClass);
    
    // Import maps
    void importRasterMap(const QString &filePath);

public slots:
    void slotUpdateObject(QJsonDocument doc);

private slots:
    void _slotObjectCreated(int nClass, double dLat, double dLon);

private:
    void _setupCanvas();
    void _addBaseMap();
    QString _generateObjectID(int nClass);
    
signals:
    void signalMouseRead(QString strMouseInfo);
    void signalClearObjectSelection();

private:
    QgsMapCanvas *m_pMapCanvas;
    QVBoxLayout *m_pLayout;
    CMapToolSelectObjects *m_pMapTool;
    
    // Store objects by ID
    QMap<QString, CVistarObject*> m_objects;
    QMap<QString, CVistarRoute*> m_routes;
    
    // Object creation state
    int m_nObjectToCreate;
    int m_nObjectCounter;
};

#endif // CMAPCANVAS_H
