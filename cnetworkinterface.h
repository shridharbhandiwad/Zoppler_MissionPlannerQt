#ifndef CNETWORKINTERFACE_H
#define CNETWORKINTERFACE_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>

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

    /**
        * @brief Initialize the sender socket with proper interface binding
        * Call this before sending messages to ensure proper multicast setup
        */
    static void initializeSender();

    /**
        * @brief Refresh network interfaces (call when network changes)
        */
    static void refreshNetworkInterfaces();

private slots:
    /**
        * @brief Process incoming datagrams from the UDP socket
        */
    void _processPendingDatagrams();

private:
    /**
        * @brief Find all suitable network interfaces for multicast
        * @return List of network interfaces suitable for multicast
        */
    static QList<QNetworkInterface> findMulticastInterfaces();

    /**
        * @brief Get the preferred multicast interface (prefers Ethernet over WiFi)
        * @return The preferred network interface for multicast
        */
    static QNetworkInterface getPreferredMulticastInterface();

    static QUdpSocket _m_udpSenderSocket;
    static bool _m_senderInitialized;
    static QHostAddress _m_multicastAddress;
    static quint16 _m_multicastPort;

    QUdpSocket *m_pUdpReceiverSocket = nullptr;    //!< UDP socket for receiving data
    QThread m_workerThread;                //!< Thread in which the receiver runs
    quint16 m_nListeningPort = 0;
    QList<QNetworkInterface> m_joinedInterfaces;   //!< Interfaces we've joined multicast on

signals:
    void signalUpdateObject(QJsonDocument);
};

#endif // CNETWORKINTERFACE_H
