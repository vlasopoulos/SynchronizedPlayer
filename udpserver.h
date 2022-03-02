#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QNetworkDatagram>

#include "udpconnection.h"

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);
    ~UdpServer();
    void writeToClients(QByteArray data, QHostAddress senderAddress, quint16 senderPort);

signals:

public slots:
    void addUdpClient(QTcpSocket* client, QHostAddress ip, quint16 udpPort);
    void removeUdpClient(QTcpSocket* client);


private slots:
    void initServer(int port);
    void udpReadFromClient();

private:
    QUdpSocket *socket;
    int port;
    QList<UdpConnection*> clients;
};

#endif // UDPSERVER_H
