#include "udpserver.h"

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{

}

UdpServer::~UdpServer()
{
    for (auto client : clients)
    {
        delete client;
    }
    socket->close();
    socket->deleteLater();
}

void UdpServer::addUdpClient(QTcpSocket* client, QHostAddress ip, quint16 udpPort)
{
    clients.append(new UdpConnection(client, ip, udpPort, this));
    qInfo() << clients;
}

void UdpServer::removeUdpClient(QTcpSocket *client)
{
    for (auto c : clients)
    {
        if (c->getClient() == client)
        {
            qInfo() << "UDP removed client:" << clients.removeOne(c);
            clients.squeeze();
            c->deleteLater();
        }
    }
}

void UdpServer::initServer(int port)
{
    socket = new QUdpSocket(this);
    this->port = port;
    socket->bind(port);
    connect(socket, &QUdpSocket::readyRead, this, &UdpServer::udpReadFromClient);
}

void UdpServer::udpReadFromClient()
{
    while (socket->hasPendingDatagrams())
       {
//           qInfo() << "got datagrams";
           QNetworkDatagram datagram = socket->receiveDatagram();
           if (datagram.data() == "UDP")
           {
               qInfo() << "UDP connection:" << datagram.senderAddress() << datagram.senderPort();
           }
           else
           {
               writeToClients(datagram.data(), datagram.senderAddress(), datagram.senderPort());
           }
       }
}

void UdpServer::writeToClients(QByteArray data, QHostAddress senderAddress, quint16 senderPort)
{
    for (auto client : clients)
    {
        if (client->getIp() != senderAddress && client->getUdpPort() != senderPort)
        {
            //        qInfo() << "writing to client:" << client << socket->writeDatagram(data, client->getIp(), client->getUdpPort());
//            qInfo() << "writing to:" << client->getIp() << client->getUdpPort();
            //        socket->writeDatagram(data, client->getIp(), client->getUdpPort());
            if (client->getIp() == QHostAddress("::1"))
            {
                socket->writeDatagram(data, QHostAddress("127.0.0.1"), client->getUdpPort());
            }
            else
            {
                socket->writeDatagram(data, client->getIp(), client->getUdpPort());
            }
        }

    }
}
