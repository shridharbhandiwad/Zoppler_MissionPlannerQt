#include "cupdateposition.h"
#include "ui_cupdateposition.h"

CUpdatePosition::CUpdatePosition(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CUpdatePosition)
{
    ui->setupUi(this);

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

    ui->lineEdit_latitude->setValidator(&latValidator);
    ui->lineEdit_longitude->setValidator(&lonValidator);
    ui->lineEdit_altitude->setValidator(&altValidator);

    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setFixedSize(this->size());
}

CUpdatePosition::~CUpdatePosition()
{
    delete ui;
}

void CUpdatePosition::on_pushButton_update_clicked()
{
    emit signalUpdatePosition(_m_sObjectId,ui->lineEdit_latitude->text().toDouble(),
    ui->lineEdit_longitude->text().toDouble(),ui->lineEdit_altitude->text().toDouble());
}

void CUpdatePosition::on_pushButton_close_clicked()
{
    close();
}

void CUpdatePosition::setObjectId(QString sObjectId)
{
    _m_sObjectId = sObjectId;
    setWindowTitle("Update Position "+sObjectId);
}

void CUpdatePosition::setPosition(double dLat,double dLon,double dAlt)
{
    ui->lineEdit_latitude->setText(QString::number(dLat,'f',7));
    ui->lineEdit_longitude->setText(QString::number(dLon,'f',7));
    ui->lineEdit_altitude->setText(QString::number(dAlt,'f',2));
}
