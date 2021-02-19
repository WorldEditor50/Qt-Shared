#include "httpclient.h"

HttpClient::HttpClient(QObject *parent):QObject(parent)
{
    pHttpWorker = new HttpWorker;
    pHttpWorker->moveToThread(&worker);
    connect(&worker, &QThread::finished, pHttpWorker, &QObject::deleteLater, Qt::QueuedConnection);
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
    pHttpWorker->setHostUrl(strUrl);
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
    return HttpWorker::user;
}

void HttpClient::save()
{
    HttpWorker::save();
    return;
}

User HttpClient::load()
{
    return HttpWorker::load();
}

void HttpClient::logout()
{
    return HttpWorker::clearUser();
}

int HttpClient::status()
{
    return pHttpWorker->statusCode.toInt();
}

QByteArray HttpClient::getResponseData()
{
    return pHttpWorker->responseData;
}

void HttpClient::saveToDisk(const QString &fileName)
{
    pHttpWorker->saveToDisk(fileName);
    return;
}
