#ifndef CNETWORKINTERFACE_H
#define CNETWORKINTERFACE_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

class CNetworkInterface : public QObject
{
    Q_OBJECT
public:
    explicit CNetworkInterface(QObject *parent = nullptr);

    static void PublishMessage(QJsonDocument doc);

    void startListening(quint16 nPort);

    /**
        * @brief Stop listening and clean up resources
        */
    void stopListening();


private slots:
    /**
        * @brief Process incoming datagrams from the UDP socket
        */
    void _processPendingDatagrams();

private :
    static QUdpSocket _m_udpSenderSocket;
private:
    QUdpSocket *m_pUdpReceiverSocket = nullptr;    //!< UDP socket for receiving data
    QThread m_workerThread;                //!< Thread in which the receiver runs
    quint16 m_nListeningPort = 0;
signals:
    void signalUpdateObject(QJsonDocument);
};

#endif // CNETWORKINTERFACE_H
