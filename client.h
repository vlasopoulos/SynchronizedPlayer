#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUrl>
#include <QDebug>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    void waitForConnection();
    void writeToServer(QString msg, QString type);
    void connectToLocalServer(int port);
    void disconnect();

signals:
    void newChatMsg(QString msg);
    void remotePlay(qint64 position);
    void remotePause(qint64 position);
    void remoteStop();
    void remoteSeek(qint64 position);
    void remoteSync(qint64 position);
    void remoteSetVideoSource(QString src);
    void remoteSetLocalVideoSource(QString src);
    void userConnected(QString usrName);
    void userDisconnected(QString usrName);

public slots:
    void setUdpPort(quint16 udpPort);
    void connectToServer(QUrl url, int port);

private slots:
    void readFromServer();

private:
    QTcpSocket *socket;
};

#endif // CLIENT_H
