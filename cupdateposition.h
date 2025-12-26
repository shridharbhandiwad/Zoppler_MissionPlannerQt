#ifndef CUPDATEPOSITION_H
#define CUPDATEPOSITION_H

#include <QWidget>
#include <QRegularExpressionValidator>
namespace Ui {
class CUpdatePosition;
}

class CUpdatePosition : public QWidget
{
    Q_OBJECT

public:
    explicit CUpdatePosition(QWidget *parent = nullptr);
    ~CUpdatePosition();

    void setObjectId(QString sObjectId);

    void setPosition(double dLat, double dLon, double dAlt);
private slots:
    void on_pushButton_update_clicked();

    void on_pushButton_close_clicked();

private:
    Ui::CUpdatePosition *ui;
    QString _m_sObjectId;

    QRegularExpressionValidator latValidator;
    QRegularExpressionValidator lonValidator;
    QRegularExpressionValidator altValidator;

signals :
    void signalUpdatePosition(QString,double,double,double);
};

#endif // CUPDATEPOSITION_H
