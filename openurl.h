#ifndef OPENURL_H
#define OPENURL_H

#include <QDialog>

namespace Ui {
class openURL;
}

class openURL : public QDialog
{
    Q_OBJECT

public:
    explicit openURL(QWidget *parent = nullptr);
    ~openURL();

signals:
    void urlSet(QString url);

private slots:
    void on_openURLbuttonBox_accepted();

private:
    Ui::openURL *ui;
};

#endif // OPENURL_H
