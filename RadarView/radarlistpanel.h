#ifndef RADARLISTPANEL_H
#define RADARLISTPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "radarmanager.h"

class RadarListPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RadarListPanel(RadarManager *manager, QWidget *parent = nullptr);

    void refresh();

signals:
    void radarSelected(int radarId);

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);
    void onOpenClicked();

private:
    void buildUI();
    void populateList();

    RadarManager  *m_manager;
    QListWidget   *m_listWidget;
    QPushButton   *m_openButton;
};

#endif // RADARLISTPANEL_H
