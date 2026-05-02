#include "cnetworkinterface.h"
#include <QtNetwork>


QUdpSocket CNetworkInterface::_m_udpSenderSocket;

CNetworkInterface::CNetworkInterface(QObject *parent)
    : QObject{parent}
{
    // Move this object to the worker thread
    this->moveToThread(&m_workerThread);

    // Connect thread finish signal to delete this object safely
    connect(&m_workerThread, &QThread::finished, this, &QObject::deleteLater);

    // Start the thread
    m_workerThread.start();

    //startListening(8881);
}

void CNetworkInterface::PublishMessage(QJsonDocument doc) {

    QByteArray data = doc.toJson(QJsonDocument::Compact);
    QHostAddress SenderAddress("225.0.0.1");
    //QHostAddress SenderAddress("192.168.1.6");
    _m_udpSenderSocket.writeDatagram(data,data.size(), SenderAddress, 8888);
}

void CNetworkInterface::startListening(quint16 nPort)
{
    m_nListeningPort = nPort;

    // Run this in the receiver thread
    QMetaObject::invokeMethod(this, [this]() {
        m_pUdpReceiverSocket = new QUdpSocket();

        // Bind to the given port
        if (!m_pUdpReceiverSocket->bind(QHostAddress::AnyIPv4, m_nListeningPort,
            QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {

            qCritical() << "[CUdpReceiver] Failed to bind to port"
                        << m_nListeningPort << ":" << m_pUdpReceiverSocket->errorString();
            delete m_pUdpReceiverSocket;
            m_pUdpReceiverSocket = nullptr;
            return;
        }
        bool bMulticast = m_pUdpReceiverSocket->joinMulticastGroup(QHostAddress("225.0.0.1"));

        // Connect readyRead to our processing slot
        connect(m_pUdpReceiverSocket, &QUdpSocket::readyRead,
                this, &CNetworkInterface::_processPendingDatagrams);

        qDebug() << "[CUdpReceiver] Listening on port" << m_nListeningPort;
    });
}

/**
 * @brief Stops listening and cleans up the socket
 */
void CNetworkInterface::stopListening()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_pUdpReceiverSocket) {
            m_pUdpReceiverSocket->close();
            m_pUdpReceiverSocket->deleteLater();
            m_pUdpReceiverSocket = nullptr;
            qDebug() << "[CUdpReceiver] Stopped listening on port" << m_nListeningPort;
        }
    });
}

/**
 * @brief Handles incoming datagrams and emits track data signals
 */
void CNetworkInterface::_processPendingDatagrams()
{
    while (m_pUdpReceiverSocket && m_pUdpReceiverSocket->hasPendingDatagrams()) {
        QByteArray baDatagram;
        baDatagram.resize(static_cast<int>(m_pUdpReceiverSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 nSenderPort;

        if (m_pUdpReceiverSocket->readDatagram(baDatagram.data(), baDatagram.size(),
                                       &sender, &nSenderPort) == -1) {
            qWarning() << "[CUdpReceiver] Failed to read datagram:"
                       << m_pUdpReceiverSocket->errorString();
            continue;
        }


        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(baDatagram, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            emit signalUpdateObject(doc);
        }



        // // Check size of received datagram
        // if (baDatagram.size() == sizeof(stTrackRecvInfo)) {
        //     stTrackRecvInfo stTrack;
        //     memcpy(&stTrack, baDatagram.constData(), sizeof(stTrackRecvInfo));

        //     // Emit signal with parsed track
        //     emit signalUpdateTrackData(stTrack);
        // } else {
        //     qWarning() << "[CUdpReceiver] Invalid datagram size:" << baDatagram.size()
        //     << ", expected:" << sizeof(stTrackRecvInfo);
        // }
    }
}
