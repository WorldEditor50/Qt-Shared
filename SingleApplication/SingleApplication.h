#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H
#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#define TIME_OUT 500

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    explicit SingleApplication(int &argc, char **argv);
    bool checkRunning();
    void setWidget(QWidget *w);
signals:
public slots:
    void restart();
private slots:
    void newLocalConnection();
private:
    void initLocalConnection();
    void newLocalServer();
    void activateWindow();
private:
    bool isRunning;
    QLocalServer *localServer;
    QWidget *w;
    QString serverName;
    QProcess restartProcess;
};

#endif // SINGLEAPPLICATION_H
