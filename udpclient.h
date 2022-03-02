#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QUrl>
#include <QNetworkDatagram>

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();
    void udpConnectToLocalServer(int port);
    void udpWriteToServer(QByteArray data);

signals:
    void setUdpPort(quint16 udpPort);
    void connected();
    void voiceReceived(QByteArray data);

public slots:
    void udpConnectToServer(QUrl url, int port);

private slots:
    void udpReadFromServer();

private:
    QUdpSocket *socket;
};

#endif // UDPCLIENT_H
