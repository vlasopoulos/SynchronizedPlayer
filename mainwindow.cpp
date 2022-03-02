#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->chatWidget->setWordWrap(true);
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.1f);
    player->setVideoOutput(ui->videoDisplay);
    player->setAudioOutput(audioOutput);
    ui->videoDisplay->installEventFilter(this);
    ui->videoDisplay->show();
    ui->seekSlider->setRange(0,player->duration());
    ui->volumeSlider->setRange(0,100);
    ui->volumeSlider->setValue(10);
    connect(ui->seekSlider, &QSlider::sliderMoved, this, &MainWindow::seek);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
//    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::handleBuffering);
    connect(ui->buttonPlay, &QPushButton::clicked, this, &MainWindow::playPressed);
    connect(ui->buttonPause, &QPushButton::clicked, this, &MainWindow::pausePressed);
    connect(ui->buttonStop, &QPushButton::clicked, this, &MainWindow::stopPressed);
    connect(ui->buttonFullScreen, &QPushButton::clicked, this, &MainWindow::fullScreen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendTimestampThreaded()
{
    if (server)
    {
        QThread *sendTimestampThread = QThread::create(&MainWindow::sendTimestamp, this);
        connect(this, &MainWindow::destroyed, sendTimestampThread, &QThread::quit);
        connect(sendTimestampThread, &QThread::finished, sendTimestampThread, &QThread::deleteLater);
        sendTimestampThread->start();
    }
}

void MainWindow::sendTimestamp()
{
    while(true)
    {
//        qInfo() << player->position();
        server->writeToClients(QString::number(player->position()), "sncsync");
        QObject().thread()->usleep(1000*1000*1); //every 1 second send sync info
    }
}

void MainWindow::setVideoSource(QString url)
{
    if (client) client->writeToServer(url, "srcnet");
    else player->setSource(QUrl(url));
}

void MainWindow::on_actionOpen_URL_triggered()
{
    openURL* urlWindow = new openURL(this);
    urlWindow->setWindowTitle("Open URL");
    urlWindow->show();
    connect(urlWindow, SIGNAL(urlSet(QString)), this, SLOT(setVideoSource(QString)));
}

void MainWindow::on_actionStart_Server_triggered()
{
    SetPort* portWindow = new SetPort(this);
    portWindow->setWindowTitle("Start Server");
    portWindow->show();
    if (server == nullptr) server = new Server(this);
    else
    {
        delete server;
        server = new Server(this);
    }
    if (udpServer == nullptr) udpServer = new UdpServer(this);
    else
    {
        delete udpServer;
        udpServer = new UdpServer(this);
    }
    connect(portWindow, SIGNAL(setPort(int)), server, SLOT(initServer(int)));
    connect(portWindow, SIGNAL(connectToLocalHost(int,QString)), this, SLOT(connectToLocalHost(int,QString)));
    connect(portWindow, SIGNAL(setPort(int)), udpServer, SLOT(initServer(int)));
    connect(server, &Server::addUdpClient, udpServer, &UdpServer::addUdpClient);
    connect(server, &Server::removeUdpClient, udpServer, &UdpServer::removeUdpClient);

    sendTimestampThreaded();
}

void MainWindow::on_actionConnect_to_Server_triggered()
{
    ConnectToServer* cts = new ConnectToServer(this);
    cts->setWindowTitle("Connect to Server");
    cts->show();
    if (client == nullptr) client = new Client(this);
    else
    {
        delete client;
        client = new Client(this);
    }
    if (udpClient == nullptr) udpClient = new UdpClient(this);
    else
    {
        delete udpClient;
        udpClient = new UdpClient(this);
    }
    connect(udpClient, &UdpClient::connected, this, &MainWindow::initClientConnections);
    connect(cts, &ConnectToServer::connectToServer, client, &Client::connectToServer);
    connect(cts, &ConnectToServer::connectToUdpServer, udpClient, &UdpClient::udpConnectToServer);
    connect(cts, &ConnectToServer::setUsername, this, &MainWindow::setUsername);
    connect(cts, &ConnectToServer::setServerURL, this, &MainWindow::setServerURL);
}

void MainWindow::connectToLocalHost(int port, QString username)
{
    serverURL = "localhost";
    if (client == nullptr) client = new Client(this);
    else
    {
        delete client;
        client = new Client(this);
    }
    if (udpClient == nullptr) udpClient = new UdpClient(this);
    else
    {
        delete udpClient;
        udpClient = new UdpClient(this);
    }
    connect(udpClient, &UdpClient::connected, this, &MainWindow::initClientConnections);
    client->connectToLocalServer(port);
    udpClient -> udpConnectToLocalServer(port);
    setUsername(username);
}

void MainWindow::initClientConnections()
{
    ui->menuConnect->setEnabled(false);
    connect(udpClient, &UdpClient::setUdpPort, client, &Client::setUdpPort);
    connect(client, &Client::newChatMsg, this, &MainWindow::newChatMsg);
    connect(client, &Client::remotePlay, this, &MainWindow::remotePlay);
    connect(client, &Client::remotePause, this, &MainWindow::remotePause);
    connect(client, &Client::remoteStop, this, &MainWindow::remoteStop);
    connect(client, &Client::remoteSeek, this, &MainWindow::remoteSeek);
    connect(client, &Client::remoteSync, this, &MainWindow::remoteSync);
    connect(client, &Client::remoteSetVideoSource, this, &MainWindow::remoteSetVideoSource);
    connect(client, &Client::remoteSetLocalVideoSource, this, &MainWindow::remoteSetLocalVideoSource);
    connect(client, &Client::userConnected, this, &MainWindow::userConnected);
    connect(client, &Client::userDisconnected, this, &MainWindow::userDisconnected);

    //instantiate audioinput/audiooutput
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::UInt8);

    QAudioDevice info = QMediaDevices::defaultAudioInput();
    if (!info.isFormatSupported(format))
    {
        qWarning() << "Default format not supported, trying to use the nearest.";
    }
    QAudioDevice info2 = QMediaDevices::defaultAudioOutput();
    if (!info2.isFormatSupported(format))
    {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    udpAudioSink = new QAudioSink(format, this);
    udpAudioSink->setVolume(0.2f);
    udpAudioSource = new QAudioSource(format, this);
    udpAudioSourceDevice = udpAudioSource->start();
    udpAudioSinkDevice = udpAudioSink->start();
    connect(udpAudioSourceDevice, &QIODevice::readyRead, this, &MainWindow::readFromMicrophone);
    connect(udpClient, &UdpClient::voiceReceived, this, &MainWindow::voiceReceived);
}

bool MainWindow::eventFilter(QObject *ob, QEvent *e){
    if (e->type() == QEvent::KeyPress && ob == ui->videoDisplay)
    {
        QKeyEvent *k = static_cast<QKeyEvent *>(e);
        if (k->key() == Qt::Key_Escape)
        {
            ui->videoDisplay->setFullScreen(false);
            return true;
        }
        if (k->key() == Qt::Key_Space)
        {
            if (player->playbackState() == QMediaPlayer::StoppedState || player->playbackState() == QMediaPlayer::PausedState)
            {
                playPressed();
                return true;
            }
            if (player->playbackState() == QMediaPlayer::PlayingState)
            {
                pausePressed();
                return true;
            }
        }
    }
    if (e->type() == QEvent::MouseButtonDblClick && ob == ui->videoDisplay)
    {
        if (ui->videoDisplay->isFullScreen()) ui->videoDisplay->setFullScreen(false);
        else ui->videoDisplay->setFullScreen(true);
        return true;
    }

    return QWidget::eventFilter(ob, e);
}

void MainWindow::fullScreen()
{
    ui->videoDisplay->setFullScreen(true);
}

void MainWindow::playPressed()
{
    if (client) client->writeToServer("play" + QString::number(player->position()), "snc");
    else player->play();
}

void MainWindow::pausePressed()
{
    if (client) client->writeToServer("paus" + QString::number(player->position()), "snc");
    else player->pause();
}

void MainWindow::stopPressed()
{
    if (client) client->writeToServer("stop", "snc");
    else player->stop();
}

void MainWindow::setUsername(QString username)
{
    if (client) client->writeToServer(username, "usr");
}

void MainWindow::on_chatInput_returnPressed()
{
    if (ui->chatInput->text() != "")
    {
        if (client) client->writeToServer(ui->chatInput->text(), "msg");
        ui->chatInput->clear();
    }
}

void MainWindow::on_sendButton_clicked()
{
    on_chatInput_returnPressed();
}

void MainWindow::newChatMsg(QString msg)
{
    QListWidgetItem* item = new QListWidgetItem(ui->chatWidget);
    item->setText(msg);
    ui->chatWidget->addItem(item);
    ui->chatWidget->scrollToBottom();
}

void MainWindow::durationChanged(qint64 duration)
{
    ui->seekSlider->setMaximum(duration);
}

void MainWindow::positionChanged(qint64 progress)
{
    ui->seekSlider->setValue(progress);
}

void MainWindow::seek(int mseconds)
{
//    qInfo() << "sync " << mseconds;
    if (client) client->writeToServer("seek" + QString::number(mseconds), "snc");
    else player->setPosition(mseconds);
}

void MainWindow::remotePlay(qint64 position)
{
    player->setPosition(position);
    player->play();
}

void MainWindow::remotePause(qint64 position)
{
    player->setPosition(position);
    player->pause();
}

void MainWindow::remoteStop()
{
    player->stop();
}

void MainWindow::remoteSeek(qint64 position)
{
    player->setPosition(position);
}

void MainWindow::remoteSync(qint64 position)
{
    if (abs(player->position() - position) > 200) player->setPosition(position);
}

void MainWindow::remoteSetVideoSource(QString src)
{
    player->setSource(QUrl(src));
}

void MainWindow::userConnected(QString usrName)
{
    QListWidgetItem* item = new QListWidgetItem(ui->participantsWidget);
    item->setText("* " + usrName);
    ui->participantsWidget->addItem(item);
}

void MainWindow::userDisconnected(QString usrName)
{
    for (int i=0; i < ui->participantsWidget->count(); i++)
    {
        if (ui->participantsWidget->item(i)->text() == "* " + usrName)
        {
            QListWidgetItem *item = ui->participantsWidget->takeItem(i);
            delete item;
            break;
        }
    }
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    audioOutput->setVolume(value/100.0);
    ui->volumeLabel->setText(QString::number(value) + "%");
}

void MainWindow::readFromMicrophone()
{
    if (udpClient != nullptr)
    {
            udpClient->udpWriteToServer(udpAudioSourceDevice->readAll());
    }
//    udpAudioSinkDevice->write(udpAudioSourceDevice->readAll());
}


void MainWindow::voiceReceived(QByteArray data)
{
    if (!micMuted)
    {
        udpAudioSinkDevice->write(data);
    }
}

void MainWindow::on_muteButton_stateChanged(int arg1)
{
    if (udpAudioSource)
    {
        if (arg1 == 0)
        {
            micMuted=false;
            udpAudioSource->setVolume(0.2f);
        }
        if (arg1 == 2)
        {
            micMuted=true;
            udpAudioSource->setVolume(0.f);
        }
    }
}

//void MainWindow::handleBuffering(QMediaPlayer::MediaStatus status)
//{
//    if (buffering && player->bufferProgress() == 1.f)
//    {
//        qInfo() << "finished buffering";
//        buffering = false;
//        playPressed();
//    }
//    if (!buffering && !(player->bufferProgress() == 1.f))
//    {
//        qInfo() << "buffering";
//        buffering = true;
//        pausePressed();
//    }
//}

void MainWindow::on_actionObit_triggered()
{
    emit themeChanged("Obit");
}


void MainWindow::on_actionMyWatch_triggered()
{
     emit themeChanged("MyWatch");
}


void MainWindow::on_actionFilmovio_triggered()
{
     emit themeChanged("Filmovio");
}


void MainWindow::on_actionCombinear_triggered()
{
     emit themeChanged("Combinear");
}


void MainWindow::on_actionDevsion_triggered()
{
     emit themeChanged("Devsion");
}


void MainWindow::on_actionPerstfic_triggered()
{
     emit themeChanged("Perstfic");
}


void MainWindow::on_actionVisualScript_triggered()
{
     emit themeChanged("VisualScript");
}


void MainWindow::on_actionWstartpage_triggered()
{
     emit themeChanged("Wstartpage");
}


void MainWindow::on_actionHost_local_file_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    connect(&dialog, &QFileDialog::fileSelected, this, &MainWindow::hostLocalFile);
    dialog.exec();
}

void MainWindow::hostLocalFile(QString file)
{
    QFileInfo info(file);
    QString directory = info.path();
    QString fileName = info.fileName();

    if (client) client->writeToServer(fileName, "srclcl");
    else player->setSource(file);
}

void MainWindow::startLocalServer(QString directory)
{

}

void MainWindow::remoteSetLocalVideoSource(QString src)
{
    player->setSource(serverURL + ":8000/" + src.replace(" ", "%20"));
    qInfo() << serverURL + ":8000/" + src;
}

void MainWindow::setServerURL(QUrl url)
{
    serverURL = url.toString();
}


