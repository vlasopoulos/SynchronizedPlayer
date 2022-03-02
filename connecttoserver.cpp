#include "connecttoserver.h"
#include "ui_connecttoserver.h"

ConnectToServer::ConnectToServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToServer)
{
    ui->setupUi(this);
}

ConnectToServer::~ConnectToServer()
{
    delete ui;
}

void ConnectToServer::on_buttonBox_accepted()
{
    QUrl url = ui->ipInput->text();
    int port = ui->portInput->text().toInt();
    qInfo() << "Connecting to " << url << ":" << port;
    QString username = ui->usernameInput->text();
    emit connectToServer(url,port);
    emit connectToUdpServer(url,port);
    emit setUsername(username);
    emit setServerURL(url);
}
