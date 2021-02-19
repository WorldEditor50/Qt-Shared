#include "SingleApplication.h"

SingleApplication::SingleApplication(int &argc, char **argv):
    QApplication(argc, argv),
    isRunning(false),
    localServer(nullptr),
    w(nullptr)
{
    serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    initLocalConnection();
}

bool SingleApplication::checkRunning()
{
    return isRunning;
}

void SingleApplication::setWidget(QWidget *w)
{
    this->w = w;
    return;
}

void SingleApplication::newLocalConnection()
{
    QLocalSocket *pSocket = localServer->nextPendingConnection();
    if (pSocket != nullptr) {
        pSocket->waitForReadyRead(2 * TIME_OUT);
        delete pSocket;
        activateWindow();
    }
    return;
}

void SingleApplication::restart()
{
    QLocalServer::removeServer(serverName);
    return;
}

void SingleApplication::initLocalConnection()
{
    isRunning = false;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    QString message = serverName + tr(" is running");
    if (socket.waitForConnected(TIME_OUT)) {
        QMessageBox info(serverName, message,
                         QMessageBox::Information,
                         QMessageBox::Yes | QMessageBox::Default,
                         QMessageBox::Escape,
                         QMessageBox::NoButton);
        if (info.exec() == QMessageBox::Yes) {

        }
        isRunning = true;
        return;
    }
    newLocalServer();
    return;
}

void SingleApplication::newLocalServer()
{
    localServer = new QLocalServer(this);
    connect(localServer, &QLocalServer::newConnection, this, &SingleApplication::newLocalConnection);
    if (!localServer->listen(serverName)) {
        if (localServer->serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(serverName);
            localServer->listen(serverName);
        }
    }
    return;
}

void SingleApplication::activateWindow()
{
    if (w != nullptr) {
        w->show();
        w->raise();
        w->activateWindow();
    }
    return;
}
