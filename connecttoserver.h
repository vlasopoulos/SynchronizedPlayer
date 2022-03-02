#ifndef CONNECTTOSERVER_H
#define CONNECTTOSERVER_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class ConnectToServer;
}

class ConnectToServer : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectToServer(QWidget *parent = nullptr);
    ~ConnectToServer();

signals:
    void connectToServer(QUrl url, int port);
    void connectToUdpServer(QUrl url, int port);
    void setUsername(QString username);
    void setServerURL(QUrl url);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ConnectToServer *ui;
};

#endif // CONNECTTOSERVER_H
