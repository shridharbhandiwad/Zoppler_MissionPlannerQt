#ifndef RADARDOCKWIDGET_H
#define RADARDOCKWIDGET_H

#include <QDockWidget>
#include "radarppiwidget.h"
#include "radarmanager.h"

class RadarDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit RadarDockWidget(const RadarView::Radar &radar, QWidget *parent = nullptr);

    int radarId() const { return m_radarId; }

    void updateRadar(const RadarView::Radar &radar);

private:
    int             m_radarId;
    RadarPPIWidget *m_ppiWidget;
};

#endif // RADARDOCKWIDGET_H
