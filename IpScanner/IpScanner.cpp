#include "IpScanner.h"

IpScanner::IpScanner(QObject *parent) :
    QThread(parent)
{
    isActive.store(1);
    pingProcess = new QProcess(this);
}

void IpScanner::startPing(const QString& ipAddr)
{
    if (pingProcess->state() == QProcess::NotRunning) {
        qDebug()<<"start ping";
        pingProcess->start(QString("ping %1 -n 1 -w 5000 -t").arg(ipAddr));
    }
    return;
}

void IpScanner::stopPing()
{
    isActive.store(0);
    pingProcess->kill();
    pingProcess->waitForFinished();
    return;
}

void IpScanner::run()
{
    while (isActive.load() == 1) {
        QByteArray output = pingProcess->readAllStandardOutput();
        if (QString(output).contains("TTL")) {
            emit sigOnline(true);
        } else {
            emit sigOnline(false);
        }
        QThread::msleep(2000);
    }
    return;
}
