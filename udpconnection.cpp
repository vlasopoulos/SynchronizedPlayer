#include "udpconnection.h"

UdpConnection::UdpConnection(QTcpSocket* newClient, QHostAddress &newIp, quint16 newUdpPort, QObject *parent) : QObject(parent)
{
    client = newClient;
    ip = newIp;
    udpPort = newUdpPort;
}

const QHostAddress &UdpConnection::getIp() const
{
    return ip;
}

void UdpConnection::setIp(const QHostAddress &newIp)
{
    ip = newIp;
}

quint16 UdpConnection::getUdpPort() const
{
    return udpPort;
}

void UdpConnection::setUdpPort(quint16 newUdpPort)
{
    udpPort = newUdpPort;
}

QTcpSocket *UdpConnection::getClient() const
{
    return client;
}
