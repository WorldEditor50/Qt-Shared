#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <QThread>
#include <QObject>
#include "HttpClientImpl.h"

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
signals:
    /* download */
    void sigDownloadFile(const QString &strUrl);
private:
    HttpClientImpl *impl;
    QThread worker;
};

#endif // HTTPCLIENT_H
