#include "cnetworkinterface.h"
#include <QtNetwork>


QUdpSocket CNetworkInterface::_m_udpSenderSocket;
bool CNetworkInterface::_m_senderInitialized = false;
QHostAddress CNetworkInterface::_m_multicastAddress("225.0.0.1");
quint16 CNetworkInterface::_m_multicastPort = 8888;

CNetworkInterface::CNetworkInterface(QObject *parent)
    : QObject{parent}
{
    // Move this object to the worker thread
    this->moveToThread(&m_workerThread);

    // Connect thread finish signal to delete this object safely
    connect(&m_workerThread, &QThread::finished, this, &QObject::deleteLater);

    // Start the thread
    m_workerThread.start();

    // Initialize sender socket on first instance
    initializeSender();

    //startListening(8881);
}

QList<QNetworkInterface> CNetworkInterface::findMulticastInterfaces()
{
    QList<QNetworkInterface> multicastInterfaces;
    
    foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
        // Skip interfaces that are not up or don't support multicast
        if (!(iface.flags() & QNetworkInterface::IsUp) ||
            !(iface.flags() & QNetworkInterface::IsRunning) ||
            !(iface.flags() & QNetworkInterface::CanMulticast)) {
            continue;
        }
        
        // Skip loopback interfaces
        if (iface.flags() & QNetworkInterface::IsLoopBack) {
            continue;
        }
        
        // Check if the interface has at least one IPv4 address
        bool hasIPv4 = false;
        foreach (const QNetworkAddressEntry &entry, iface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol &&
                !entry.ip().isLoopback()) {
                hasIPv4 = true;
                break;
            }
        }
        
        if (hasIPv4) {
            multicastInterfaces.append(iface);
            qDebug() << "[CNetworkInterface] Found multicast-capable interface:" 
                     << iface.humanReadableName() << "(" << iface.name() << ")";
        }
    }
    
    return multicastInterfaces;
}

QNetworkInterface CNetworkInterface::getPreferredMulticastInterface()
{
    QList<QNetworkInterface> interfaces = findMulticastInterfaces();
    
    if (interfaces.isEmpty()) {
        qWarning() << "[CNetworkInterface] No multicast-capable interfaces found!";
        return QNetworkInterface();
    }
    
    // Prefer Ethernet over WiFi
    // Common Ethernet interface names: eth*, en*, Ethernet*, Local Area Connection*
    // Common WiFi interface names: wlan*, wl*, Wi-Fi*, Wireless*
    
    QNetworkInterface preferredInterface;
    int priority = 0;  // Higher is better
    
    foreach (const QNetworkInterface &iface, interfaces) {
        QString name = iface.name().toLower();
        QString humanName = iface.humanReadableName().toLower();
        int ifacePriority = 1;  // Default priority
        
        // Check for Ethernet indicators (higher priority)
        if (name.startsWith("eth") || name.startsWith("en") || 
            name.startsWith("enp") || name.startsWith("eno") ||
            humanName.contains("ethernet") || humanName.contains("local area")) {
            ifacePriority = 3;
            qDebug() << "[CNetworkInterface] Identified as Ethernet:" << iface.humanReadableName();
        }
        // Check for WiFi indicators (lower priority)
        else if (name.startsWith("wlan") || name.startsWith("wl") ||
                 name.startsWith("wifi") || humanName.contains("wi-fi") ||
                 humanName.contains("wireless")) {
            ifacePriority = 2;
            qDebug() << "[CNetworkInterface] Identified as WiFi:" << iface.humanReadableName();
        }
        
        if (ifacePriority > priority) {
            priority = ifacePriority;
            preferredInterface = iface;
        }
    }
    
    qDebug() << "[CNetworkInterface] Selected preferred interface:" 
             << preferredInterface.humanReadableName();
    
    return preferredInterface;
}

void CNetworkInterface::initializeSender()
{
    if (_m_senderInitialized) {
        return;
    }
    
    qDebug() << "[CNetworkInterface] Initializing multicast sender...";
    
    // Get preferred interface for multicast
    QNetworkInterface preferredIface = getPreferredMulticastInterface();
    
    if (preferredIface.isValid()) {
        // Set the multicast interface for the sender socket
        _m_udpSenderSocket.setMulticastInterface(preferredIface);
        
        // Get the first IPv4 address of the preferred interface for binding
        foreach (const QNetworkAddressEntry &entry, preferredIface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol &&
                !entry.ip().isLoopback()) {
                // Bind sender to specific interface
                if (_m_udpSenderSocket.bind(entry.ip(), 0, 
                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
                    qDebug() << "[CNetworkInterface] Sender bound to" << entry.ip().toString()
                             << "on interface" << preferredIface.humanReadableName();
                } else {
                    qWarning() << "[CNetworkInterface] Failed to bind sender to" 
                               << entry.ip().toString() << ":" << _m_udpSenderSocket.errorString();
                    // Try binding to any address as fallback
                    _m_udpSenderSocket.bind(QHostAddress::AnyIPv4, 0,
                        QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
                }
                break;
            }
        }
        
        qDebug() << "[CNetworkInterface] Multicast interface set to:" 
                 << preferredIface.humanReadableName();
    } else {
        qWarning() << "[CNetworkInterface] No valid interface found, using system default";
    }
    
    // Set TTL for multicast (1 = local network only, increase for routing across networks)
    _m_udpSenderSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
    
    // Disable loopback (don't receive our own messages)
    _m_udpSenderSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
    
    _m_senderInitialized = true;
    qDebug() << "[CNetworkInterface] Sender initialization complete";
}

void CNetworkInterface::refreshNetworkInterfaces()
{
    qDebug() << "[CNetworkInterface] Refreshing network interfaces...";
    
    // Close and reset the sender socket
    _m_udpSenderSocket.close();
    _m_senderInitialized = false;
    
    // Re-initialize
    initializeSender();
}

void CNetworkInterface::PublishMessage(QJsonDocument doc) {
    // Ensure sender is initialized
    if (!_m_senderInitialized) {
        initializeSender();
    }

    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    qint64 bytesSent = _m_udpSenderSocket.writeDatagram(data, data.size(), 
                                                         _m_multicastAddress, _m_multicastPort);
    
    if (bytesSent == -1) {
        qWarning() << "[CNetworkInterface] Failed to send multicast message:" 
                   << _m_udpSenderSocket.errorString();
        
        // Try refreshing interfaces and retry once
        refreshNetworkInterfaces();
        bytesSent = _m_udpSenderSocket.writeDatagram(data, data.size(), 
                                                      _m_multicastAddress, _m_multicastPort);
        if (bytesSent == -1) {
            qCritical() << "[CNetworkInterface] Retry also failed:" 
                        << _m_udpSenderSocket.errorString();
        }
    }
}

void CNetworkInterface::startListening(quint16 nPort)
{
    m_nListeningPort = nPort;

    // Run this in the receiver thread
    QMetaObject::invokeMethod(this, [this]() {
        m_pUdpReceiverSocket = new QUdpSocket();

        // Bind to the given port on any IPv4 address
        if (!m_pUdpReceiverSocket->bind(QHostAddress::AnyIPv4, m_nListeningPort,
            QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {

            qCritical() << "[CUdpReceiver] Failed to bind to port"
                        << m_nListeningPort << ":" << m_pUdpReceiverSocket->errorString();
            delete m_pUdpReceiverSocket;
            m_pUdpReceiverSocket = nullptr;
            return;
        }
        
        // Join multicast group on ALL available interfaces
        // This ensures we receive multicast data regardless of which interface it comes from
        QList<QNetworkInterface> multicastInterfaces = findMulticastInterfaces();
        
        if (multicastInterfaces.isEmpty()) {
            // Fallback: try to join without specifying interface
            qWarning() << "[CUdpReceiver] No multicast interfaces found, trying default join";
            bool bMulticast = m_pUdpReceiverSocket->joinMulticastGroup(_m_multicastAddress);
            if (!bMulticast) {
                qWarning() << "[CUdpReceiver] Failed to join multicast group (default):" 
                           << m_pUdpReceiverSocket->errorString();
            } else {
                qDebug() << "[CUdpReceiver] Joined multicast group" << _m_multicastAddress.toString()
                         << "(default interface)";
            }
        } else {
            // Join multicast group on each interface
            foreach (const QNetworkInterface &iface, multicastInterfaces) {
                bool bMulticast = m_pUdpReceiverSocket->joinMulticastGroup(_m_multicastAddress, iface);
                if (bMulticast) {
                    m_joinedInterfaces.append(iface);
                    qDebug() << "[CUdpReceiver] Joined multicast group" << _m_multicastAddress.toString()
                             << "on interface" << iface.humanReadableName();
                } else {
                    qWarning() << "[CUdpReceiver] Failed to join multicast group on" 
                               << iface.humanReadableName() << ":" << m_pUdpReceiverSocket->errorString();
                }
            }
            
            if (m_joinedInterfaces.isEmpty()) {
                qCritical() << "[CUdpReceiver] Failed to join multicast group on any interface!";
            }
        }

        // Connect readyRead to our processing slot
        connect(m_pUdpReceiverSocket, &QUdpSocket::readyRead,
                this, &CNetworkInterface::_processPendingDatagrams);

        qDebug() << "[CUdpReceiver] Listening on port" << m_nListeningPort 
                 << "on" << m_joinedInterfaces.size() << "interfaces";
    });
}

/**
 * @brief Stops listening and cleans up the socket
 */
void CNetworkInterface::stopListening()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_pUdpReceiverSocket) {
            // Leave multicast group on all joined interfaces
            foreach (const QNetworkInterface &iface, m_joinedInterfaces) {
                m_pUdpReceiverSocket->leaveMulticastGroup(_m_multicastAddress, iface);
                qDebug() << "[CUdpReceiver] Left multicast group on" << iface.humanReadableName();
            }
            m_joinedInterfaces.clear();
            
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
