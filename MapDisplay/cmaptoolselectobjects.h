#ifndef CMAPTOOLSELECTOBJECTS_H
#define CMAPTOOLSELECTOBJECTS_H

#include <qgsmaptool.h>
#include <qgspointxy.h>

class CMapToolSelectObjects : public QgsMapTool
{
    Q_OBJECT

public:
    explicit CMapToolSelectObjects(QgsMapCanvas *canvas);
    ~CMapToolSelectObjects();
    
    void canvasPressEvent(QgsMapMouseEvent *event) override;
    void canvasMoveEvent(QgsMapMouseEvent *event) override;
    void canvasReleaseEvent(QgsMapMouseEvent *event) override;

signals:
    void pointSelected(const QgsPointXY &point);
    void objectSelected(const QString &objectId);

private:
    QgsPointXY m_clickPoint;
};

#endif // CMAPTOOLSELECTOBJECTS_H
