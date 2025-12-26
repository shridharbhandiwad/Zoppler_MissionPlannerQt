#ifndef CUPDATEROUTE_H
#define CUPDATEROUTE_H

#include <QWidget>
#include "MapDisplay/qgspointxyz.h"
#include <QRegularExpressionValidator>
namespace Ui {
class CUpdateRoute;
}

class CUpdateRoute : public QWidget
{
    Q_OBJECT

public:
    explicit CUpdateRoute(QWidget *parent = nullptr);
    ~CUpdateRoute();

    void setPoints(QList<QgsPointXYZ> listPoints);

    void setObjectId(QString sObjectId);
private:
    Ui::CUpdateRoute *ui;

    QString _m_sObjectId;


    QRegularExpressionValidator latValidator;
    QRegularExpressionValidator lonValidator;
    QRegularExpressionValidator altValidator;

signals :
    void signalUpdatePoints(QString,QList<QgsPointXYZ>);
private slots:
    void on_pushButton_update_clicked();
    void on_pushButton_close_clicked();
};

#endif // CUPDATEROUTE_H
