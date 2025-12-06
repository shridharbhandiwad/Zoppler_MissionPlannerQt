#ifndef CMAPTOOLSELECTOBJECTS_H
#define CMAPTOOLSELECTOBJECTS_H

#include <qgsmaptool.h>
#include <qgsmapcanvas.h>
#include <QObject>

class CMapToolSelectObjects : public QgsMapTool
{
    Q_OBJECT
public:
    explicit CMapToolSelectObjects(QgsMapCanvas *pCanvas);
    ~CMapToolSelectObjects() override;

    void canvasPressEvent(QgsMapMouseEvent *e) override;
    void canvasMoveEvent(QgsMapMouseEvent *e) override;
    void canvasReleaseEvent(QgsMapMouseEvent *e) override;

    void SetObjectToCreate(int nClass);

signals:
    void signalObjectCreated(int nClass, double dLat, double dLon);

private:
    int m_nObjectClass;
};

#endif // CMAPTOOLSELECTOBJECTS_H
