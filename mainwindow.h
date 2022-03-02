#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioSink>
#include <QAudioSource>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QVideoWidget>
#include <QPushButton>
#include <QDebug>
#include <QThread>
#include <QKeyEvent>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include <QFileDialog>

#include "openurl.h"
#include "setport.h"
#include "server.h"
#include "udpserver.h"
#include "client.h"
#include "udpclient.h"
#include "connecttoserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sendTimestampThreaded();
    void sendTimestamp();
    bool eventFilter(QObject *ob, QEvent *e);

signals:
    void themeChanged(QString theme);

private slots:
    void on_actionOpen_URL_triggered();
    void setVideoSource(QString url);
    void on_actionStart_Server_triggered();
    void on_actionConnect_to_Server_triggered();
    void fullScreen();
    void playPressed();
    void pausePressed();
    void stopPressed();
    void setUsername(QString username);
    void on_chatInput_returnPressed();
    void on_sendButton_clicked();
    void newChatMsg(QString msg);
    void connectToLocalHost(int port, QString username);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int mseconds);
    void remotePlay(qint64 position);
    void remotePause(qint64 position);
    void remoteStop();
    void remoteSeek(qint64 position);
    void remoteSync(qint64 position);
    void remoteSetVideoSource(QString src);
    void userConnected(QString usrName);
    void userDisconnected(QString usrName);
    void on_volumeSlider_valueChanged(int value);
    void readFromMicrophone();
    void initClientConnections();
    void voiceReceived(QByteArray data);
    void on_muteButton_stateChanged(int arg1);
//    void handleBuffering(QMediaPlayer::MediaStatus status);
    void hostLocalFile(QString file);
    void on_actionObit_triggered();
    void on_actionMyWatch_triggered();
    void on_actionFilmovio_triggered();
    void on_actionCombinear_triggered();
    void on_actionDevsion_triggered();
    void on_actionPerstfic_triggered();
    void on_actionVisualScript_triggered();
    void on_actionWstartpage_triggered();
    void on_actionHost_local_file_triggered();
    void remoteSetLocalVideoSource(QString src);
    void setServerURL(QUrl url);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput* audioOutput;
    QAudioSink* udpAudioSink;
    QAudioSource* udpAudioSource = nullptr;
    QIODevice* udpAudioSinkDevice;
    QIODevice* udpAudioSourceDevice;
    Server *server = nullptr;
    Client *client = nullptr;
    UdpServer *udpServer = nullptr;
    UdpClient *udpClient = nullptr;
    bool micMuted = false;
//    bool buffering = false;
    QString serverURL;
    void startLocalServer(QString directory);
};
#endif // MAINWINDOW_H
