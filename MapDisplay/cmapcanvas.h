#ifndef CMAPCANVAS_H
#define CMAPCANVAS_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>

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

signals:
    void signalMouseRead(QString mouseInfo);
    void signalClearObjectSelection();

public slots:
    void slotUpdateObject(QJsonDocument doc);
    void slotLoadRoute(QJsonDocument doc);

private:
    // Private members can be added as needed
};

#endif // CMAPCANVAS_H
