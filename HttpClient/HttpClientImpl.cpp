#include "HttpClientImpl.h"
#include <QDebug>

User HttpClientImpl::user;
HttpClientImpl::HttpClientImpl(QObject *parent) :
    QObject(parent),
    timeout(30)
{
    host = "https://www.baidu.com";
}

HttpClientImpl::~HttpClientImpl()
{

}
void HttpClientImpl::save()
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

User HttpClientImpl::load()
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

void HttpClientImpl::clearUser()
{
    user.token.clear();
    user.name.clear();
    user.password.clear();
    return;
}

void HttpClientImpl::authentication(QNetworkReply *, QAuthenticator *authenticator)
{
    qDebug()<<"enter authentication";
    authenticator->setUser(user.name);
    authenticator->setPassword(user.password);
    return;
}

void HttpClientImpl::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
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

void HttpClientImpl::setUserAgent(QNetworkRequest &request)
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

QByteArray HttpClientImpl::startRequest(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &data)
{
    QNetworkAccessManager manager;
    QNetworkReply *reply = nullptr;
    switch (op) {
    case QNetworkAccessManager::HeadOperation:
        reply = manager.head(request);
        break;
    case QNetworkAccessManager::GetOperation:
        reply = manager.get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = manager.put(request, data);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = manager.post(request, data);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = manager.deleteResource(request);
        break;
    default:
        break;
    }
    connect(&manager, &QNetworkAccessManager::authenticationRequired,
            this, &HttpClientImpl::authentication);
    connect(&manager, &QNetworkAccessManager::sslErrors,
            this, &HttpClientImpl::sslErrors);
    QByteArray responseData;
    connect(reply, &QIODevice::readyRead, this, [reply, &responseData](){
        responseData += reply->readAll();
    });
    connect(reply, &QNetworkReply::finished, this, [&](){
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (reply->error() != QNetworkReply::NoError) {
            qDebug()<<"reply error: "<<reply->error();
            emit requestFail(reply->errorString());
            return;
        }
        /* redirect */
        const QVariant redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirectTarget.isNull()) {
            return;
        }
        QUrl url(host);
        QUrl redirectUrl = url.resolved(redirectTarget.toUrl());
        QNetworkRequest request;
        request.setUrl(redirectUrl);
        setUserAgent(request);
        reply = manager.get(request);
    });
    /* wait response */
    wait(reply, timeout);
    return responseData;
}

void HttpClientImpl::wait(QNetworkReply *reply, int sec)
{
    QEventLoop eventLoop;
    QTimer timer;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    /* start timer */
    timer.start(sec * 1000);
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
}

void HttpClientImpl::get(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    QByteArray data;
    startRequest(QNetworkAccessManager::GetOperation, request, data);
    return;
}

void HttpClientImpl::post(const QUrl &url, const QByteArray &data)
{
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    startRequest(QNetworkAccessManager::PostOperation, request, data);
    return;
}

void HttpClientImpl::put(const QUrl &url, const QByteArray &data)
{
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    startRequest(QNetworkAccessManager::PutOperation, request, data);
    return;
}

void HttpClientImpl::deleteResource(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    setUserAgent(request);
    QByteArray data;
    startRequest(QNetworkAccessManager::DeleteOperation, request, data);
    return;
}

void HttpClientImpl::setUser(const QString &userName, const QString &password)
{
    user.name = userName;
    user.password = password;
    return;
}

void HttpClientImpl::setTimeout(int timeout)
{
    this->timeout = timeout;
    return;
}

void HttpClientImpl::setHost(const QString &hostUrl)
{
    this->host = hostUrl;
    return;
}

QString HttpClientImpl::getRegisterType(const QString &username)
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
