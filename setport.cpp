#include "setport.h"
#include "ui_setport.h"

SetPort::SetPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPort)
{
    ui->setupUi(this);
}

SetPort::~SetPort()
{
    delete ui;
}

void SetPort::on_buttonBox_accepted()
{
    int port = (ui->portInput->text()).toInt();
    emit setPort(port);
    QString username = ui->usernameInput->text();
    emit connectToLocalHost(port, username);
}
