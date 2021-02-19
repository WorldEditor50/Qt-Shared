#ifndef IPSCANNER_H
#define IPSCANNER_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QString>
#include <QAtomicInt>
#include <QHostInfo>
class IpScanner : public QThread
{
    Q_OBJECT
public:
    explicit IpScanner(QObject *parent = nullptr);
signals:
    void sigOnline(bool on);
public slots:
    void startPing(const QString& ipAddr);
    void stopPing();
protected:
    void run() override;
private:
    QAtomicInt isActive;
    QProcess *pingProcess;
};

#endif // IPSCANNER_H
