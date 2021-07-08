#include "HttpClient.h"

HttpClient::HttpClient(QObject *parent):QObject(parent)
{
    impl = new HttpClientImpl;
    impl->moveToThread(&worker);
    connect(&worker, &QThread::finished, impl, &QObject::deleteLater, Qt::QueuedConnection);
    worker.start();
}

HttpClient::~HttpClient()
{
    if (!worker.isFinished()) {
        stopWorker();
    }
}

void HttpClient::setHostUrl(const QString &strUrl)
{
    impl->setHost(strUrl);
    return;
}

void HttpClient::stopWorker()
{
    worker.quit();
    worker.wait();
    return;
}

void HttpClient::startWorker()
{
    worker.start();
    return;
}

User HttpClient::getUser()
{
    return HttpClientImpl::user;
}

void HttpClient::save()
{
    HttpClientImpl::save();
    return;
}

User HttpClient::load()
{
    return HttpClientImpl::load();
}

void HttpClient::logout()
{
    return HttpClientImpl::clearUser();
}
