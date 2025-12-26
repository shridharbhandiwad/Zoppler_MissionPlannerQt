#include "cupdateroute.h"
#include "ui_cupdateroute.h"
#include <QLineEdit>
#include <QDebug>

CUpdateRoute::CUpdateRoute(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CUpdateRoute)
{
    ui->setupUi(this);
    _m_sObjectId = "";

    QRegularExpression reLat(
        R"(^([+-]?([0-8]?\d(\.\d+)?|90(\.0+)?))$)"
        );

    QRegularExpression reLon(
        R"(^([+-]?((1[0-7]\d|0?\d?\d)(\.\d+)?|180(\.0+)?))$)"
        );

    QRegularExpression reAlt(
        R"(^(?:[+-]?\d+(?:\.\d+)?)$)"
        );

    latValidator.setRegularExpression(reLat);
    lonValidator.setRegularExpression(reLon);
    altValidator.setRegularExpression(reAlt);

    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ ) {
        QLineEdit *leLat = new QLineEdit(this);
        leLat->setValidator(&latValidator);
        QLineEdit *leLon = new QLineEdit(this);
        leLon->setValidator(&lonValidator);
        QLineEdit *leAlt = new QLineEdit(this);
        leAlt->setValidator(&altValidator);

        ui->tableWidget->setCellWidget(i,0,leLat);
        ui->tableWidget->setCellWidget(i,1,leLon);
        ui->tableWidget->setCellWidget(i,2,leAlt);
    }

    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setFixedSize(this->size());
}

CUpdateRoute::~CUpdateRoute()
{
    delete ui;
}

void CUpdateRoute::setPoints(QList<QgsPointXYZ> listPoints)
{
    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ ) {

        QLineEdit *leLat = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,0));
        QLineEdit *leLon = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,1));
        QLineEdit *leAlt = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,2));

        if ( i < listPoints.size() ) {
            QgsPointXYZ pt = listPoints.at(i);

            if (leLat) {
                leLat->setText(QString::number(pt.y(),'f',7));
            }
            if (leLon) {
                leLon->setText(QString::number(pt.x(),'f',7));
            }
            if (leAlt) {
                leAlt->setText(QString::number(pt.z(),'f',2));
            }
        }
        else {
            if (leLat) {
                leLat->clear();
            }
            if (leLon) {
                leLon->clear();
            }
            if (leAlt) {
                leAlt->clear();
            }
        }
    }
}

void CUpdateRoute::setObjectId(QString sObjectId)
{
    _m_sObjectId = sObjectId;
    setWindowTitle("Update Route "+sObjectId);
}

void CUpdateRoute::on_pushButton_update_clicked()
{
    QList<QgsPointXYZ> points;

    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ ) {
        QLineEdit *leLat = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,0));
        QLineEdit *leLon = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,1));
        QLineEdit *leAlt = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,2));
        if (leLat && leLon && leAlt ) {
            if ( leLat->text().isEmpty() && leLon->text().isEmpty() &&
                leLon->text().isEmpty() ) {

            }
            else if ( leLat->text().isEmpty() || leLon->text().isEmpty() ||
                leLon->text().isEmpty() ) {
                return;
            }
            else {
                points<<QgsPointXYZ(leLon->text().toDouble(),leLat->text().toDouble(),leAlt->text().toDouble());
            }
        }
        else {
            break;
        }
    }


    if ( points.size() > 0 ) {
        qDebug()<<"New Route Points "<<points.size();
        emit signalUpdatePoints(_m_sObjectId,points);
    }
}


void CUpdateRoute::on_pushButton_close_clicked()
{
    close();
}

