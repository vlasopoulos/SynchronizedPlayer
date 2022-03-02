#include "openurl.h"
#include "ui_openurl.h"

openURL::openURL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::openURL)
{
    ui->setupUi(this);
}

openURL::~openURL()
{
    delete ui;
}

void openURL::on_openURLbuttonBox_accepted()
{
    QString url = ui->urlInput->text();
    emit urlSet(url);
}
