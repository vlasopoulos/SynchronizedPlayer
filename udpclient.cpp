#include "udpclient.h"

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{

}

UdpClient::~UdpClient()
{
        socket->close();
        socket->deleteLater();
}

void UdpClient::udpConnectToLocalServer(int port)
{
    socket = new QUdpSocket(this);
//    qInfo() << socket->bind(QHostAddress::LocalHost);
    socket->connectToHost(QHostAddress("127.0.0.1")/*QHostAddress::LocalHost*/, port);
    QByteArray data;
    data.append("UDP");
    socket->write(data);
    emit connected();
    emit setUdpPort(socket->localPort());
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::udpReadFromServer);
}

void UdpClient::udpConnectToServer(QUrl url, int port)
{
    socket = new QUdpSocket(this);
//    qInfo() << socket->bind(QHostAddress::Any);
    if (url.toString() == "localhost") socket->connectToHost(QHostAddress("127.0.0.1")/*QHostAddress::LocalHost*/, port);
    else socket->connectToHost(url.toString(), port);
    QByteArray data;
    data.append("UDP");
    socket->write(data);
    emit connected();
    emit setUdpPort(socket->localPort());
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::udpReadFromServer);
}

void UdpClient::udpReadFromServer()
{
    while (socket->hasPendingDatagrams())
       {
//           qInfo() << "got datagrams";
           QNetworkDatagram datagram = socket->receiveDatagram();
//           qInfo() << datagram.data();
           emit voiceReceived(datagram.data());

       }
}

void UdpClient::udpWriteToServer(QByteArray data)
{
//    qInfo() << "sending to server" << socket->write(data);
    if (socket) socket->write(data);
}
