#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QList>
#include <chrono>
#include <thread>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    void listenThreaded();
    void listen();
    void listClients();
    void appendNewConnection(QTcpSocket* client);
    void writeToClients(QString msg, QString type);
    void writeToClients(QString msg, QString type, QTcpSocket* currentClient);

signals:
    void addUdpClient(QTcpSocket* client, QHostAddress ip, quint16 udpPort);
    void removeUdpClient(QTcpSocket* client);

private slots:
    void initServer(int port);
    void newConnection();
    void clientDisconnected();
    void readFromClient();

private:
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;
    int port;
    QString source;
    bool isSourceSet = false;
};

#endif // SERVER_H
