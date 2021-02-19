#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <QObject>
#include <QThread>
#include "httpworker.h"

class  HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);
    ~HttpClient();
    void setHostUrl(const QString &strUrl);
    void stopWorker();
    void startWorker();
    User getUser();
    void save();
    User load();
    void logout();
    int status();
    QByteArray getResponseData();
    void saveToDisk(const QString &fileName);
signals:
    /* download */
    void sigDownloadFile(const QString &strUrl);
public:
    HttpWorker *pHttpWorker;
private:
    QThread worker;
};

#endif // HTTPCLIENT_H
