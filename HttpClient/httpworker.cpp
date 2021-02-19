#include "httpworker.h"
#include <QDebug>

User HttpWorker::user;
HttpWorker::HttpWorker(QObject *parent) :
    QObject(parent),
    timeout(30)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::authenticationRequired, this, &HttpWorker::authentication, Qt::QueuedConnection);
    connect(manager, &QNetworkAccessManager::sslErrors, this, &HttpWorker::sslErrors, Qt::QueuedConnection);
    strHostUrl = "https://www.baidu.com";
}

HttpWorker::~HttpWorker()
{

}
void HttpWorker::save()
{
    if (user.name.isEmpty() || user.password.isEmpty()) {
        return;
    }
    QJsonObject obj;
    obj.insert("name", user.name);
    obj.insert("password", user.password);
    QJsonDocument document(obj);
    QFile file("user");
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    file.write(document.toJson());
    file.close();
    return;
}

User HttpWorker::load()
{
    QFile file("user");
    if (!file.open(QIODevice::ReadOnly)) {
        return user;
    }
    QByteArray data = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject obj = document.object();
    user.name = obj.take("name").toString();
    user.password = obj.take("password").toString();
    file.close();
    return user;
}

void HttpWorker::clearUser()
{
    user.token.clear();
    user.name.clear();
    user.password.clear();
    return;
}

void HttpWorker::replyFinished()
{
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (reply->error() != QNetworkReply::NoError) {
        qDebug()<<"reply error: "<<reply->error();
        emit requestFail(reply->errorString());
        reply->deleteLater();
        reply = nullptr;
        return;
    }
    /* redirect */
    const QVariant redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    reply->deleteLater();
    reply = nullptr;
    if (redirectTarget.isNull()) {
        return;
    }
    const QUrl redirectUrl = url.resolved(redirectTarget.toUrl());
    QNetworkRequest request;
    request.setUrl(redirectUrl);
    setUserAgent(request);
    reply = manager->get(request);
    return;
}

void HttpWorker::read()
{
    QByteArray tmp = reply->readAll();
    replyDataSize += tmp.size();
    responseData += tmp;
    emit valueChanged(replyDataSize);
    return;
}

void HttpWorker::authentication(QNetworkReply *, QAuthenticator *authenticator)
{
    qDebug()<<"enter authentication";
    authenticator->setUser(user.name);
    authenticator->setPassword(user.password);
    return;
}

void HttpWorker::sslErrors(QNetworkReply *, const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty()) {
            errorString += '\n';
        }
        errorString += error.errorString();
    }
    //httpLog.write(errorString.toUtf8());
    reply->ignoreSslErrors();
    return;
}

void HttpWorker::setUserAgent(QNetworkRequest &request)
{
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
                                    " AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3100.0 Safari/537.36");
    request.setRawHeader("Accept-Language", "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
    request.setRawHeader("Content-Type", "text/html; charset=UTF-8");
    request.setRawHeader("Vary", "Accept-Encoding");
    request.setRawHeader("Accept-Encoding", "deflate");
    request.setRawHeader("Cache-Control", "max-age=0");
    return;
}

void HttpWorker::doRequest(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &data)
{
    switch (op) {
    case QNetworkAccessManager::HeadOperation:
        reply = manager->head(request);
        break;
    case QNetworkAccessManager::GetOperation:
        reply = manager->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = manager->put(request, data);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = manager->post(request, data);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = manager->deleteResource(request);
        break;
    default:
        return;
    }
    responseData.clear();
    replyDataSize = 0;
    connect(reply, &QNetworkReply::finished, this, &HttpWorker::replyFinished, Qt::QueuedConnection);
    connect(reply, &QIODevice::readyRead, this, &HttpWorker::read, Qt::QueuedConnection);
    /* block */
    QEventLoop eventLoop;
    QTimer timer;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    /* start timer */
    timer.start(timeout * 1000);
    /* block until replication finished */
    eventLoop.exec(QEventLoop::WaitForMoreEvents);

    if (timer.isActive()) {
        timer.stop();
    } else {
        /* time out */
        disconnect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        reply->abort();
        reply->deleteLater();
        emit requestFail("Time Out");
    }
    return;
}

void HttpWorker::get(const QUrl &reqUrl)
{
    url = reqUrl;
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    QByteArray data;
    doRequest(QNetworkAccessManager::GetOperation, request, data);
    return;
}

void HttpWorker::post(const QUrl &reqUrl, const QByteArray &data)
{
    url = reqUrl;
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    doRequest(QNetworkAccessManager::PostOperation, request, data);
    return;
}

void HttpWorker::put(const QUrl &reqUrl, const QByteArray &data)
{
    url = reqUrl;
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    doRequest(QNetworkAccessManager::PutOperation, request, data);
    return;
}

void HttpWorker::deleteResource(const QUrl &reqUrl)
{
    url = reqUrl;
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    QByteArray data;
    doRequest(QNetworkAccessManager::DeleteOperation, request, data);
    return;
}

void HttpWorker::setUser(const QString &userName, const QString &password)
{
    user.name = userName;
    user.password = password;
    return;
}

void HttpWorker::setTimeout(int timeout)
{
    this->timeout = timeout;
    return;
}

void HttpWorker::setHostUrl(const QString &hostUrl)
{
    this->strHostUrl = hostUrl;
    return;
}

void HttpWorker::saveToDisk(const QString &fileName)
{
    if (responseData.isNull()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    file.write(responseData);
    file.close();
    return;
}

QString HttpWorker::jointHostUrl(const QString &path)
{
    return strHostUrl + path;
}

QString HttpWorker::getRegisterType(const QString &username)
{
    /* match registering type */
    QString type = "";
    /* ([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?) */
    QRegularExpression emailRegularExp("^([a-zA-Z0-9]+[_|\\_|\\.]?)*[a-zA-Z0-9]+@([a-zA-Z0-9]+[_|\\_|\\.]?)*[a-zA-Z0-9]+\\.[a-zA-Z]{2,3}$");
    QRegularExpressionMatch emailMatch = emailRegularExp.match(username);
    if (emailMatch.hasMatch()) {
       type = "email";
    }
    return type;
}

void HttpWorker::downloadFile(const QString &strUrl)
{
    url = QUrl::fromUserInput(strUrl);
    QNetworkRequest request;
    request.setUrl(url);
    QByteArray data;
    doRequest(QNetworkAccessManager::GetOperation, request, data);
    emit downloadFileFinished(responseData);
    return;
}
