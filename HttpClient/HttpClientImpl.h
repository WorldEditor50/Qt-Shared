#ifndef HTTP_CLIENT_IMPL_H
#define HTTP_CLIENT_IMPL_H
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QEventLoop>
#include <QTimer>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QFile>
#include <QCryptographicHash>
#include <QMutex>
#include <QThread>
#include <QProcess>
#include <QWaitCondition>
#include <QRegularExpression>
#include <QHostInfo>

class User
{
public:
    User(){}
    ~User(){}
    QString name;
    QString password;
    QString token;
    QString language;
};
class HttpClientImpl : public QObject
{
    Q_OBJECT
public:
    explicit HttpClientImpl(QObject *parent = nullptr);
    ~HttpClientImpl();
    static void save();
    static User load();
    static void clearUser();
    void get(const QUrl &url);
    void post(const QUrl &url, const QByteArray &data);
    void put(const QUrl &url, const QByteArray &data);
    void deleteResource(const QUrl &url);
    void setUser(const QString &userName, const QString &password);
    void setTimeout(int timeout);
    void setHost(const QString &url);
    QString getRegisterType(const QString &username);
signals:
    void valueChanged(double value);
    void requestFinished(QByteArray responseData);
    /* time out */
    void requestFail(QString error);
private slots:
    void authentication(QNetworkReply *, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void setUserAgent(QNetworkRequest &request);
private:
    QByteArray startRequest(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &data);
    void wait(QNetworkReply *reply, int sec);
public:
    static User user;
private:
    int timeout;
    QString host;
};

#endif // HTTP_CLIENT_IMPL_H
