#ifndef HTTPWORKER_H
#define HTTPWORKER_H
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
class HttpWorker : public QObject
{
    Q_OBJECT
public:
    explicit HttpWorker(QObject *parent = nullptr);
    ~HttpWorker();
    static void save();
    static User load();
    static void clearUser();
    void get(const QUrl &reqUrl);
    void post(const QUrl &reqUrl, const QByteArray &data);
    void put(const QUrl &reqUrl, const QByteArray &data);
    void deleteResource(const QUrl &reqUrl);
    void setUser(const QString &userName, const QString &password);
    void setTimeout(int timeout);
    void setHostUrl(const QString &hostUrl);
    void saveToDisk(const QString &fileName);
    inline QString jointHostUrl(const QString &path);
    QString getRegisterType(const QString &username);
signals:
    void valueChanged(double value);
    void requestFinished(QByteArray responseData);
    /* time out */
    void requestFail(QString error);
    /* download */
    void downloadFileFinished(QByteArray responseData);
public slots:
    /* download */
    void downloadFile(const QString &strUrl);
private slots:
    void replyFinished();
    void read();
    void authentication(QNetworkReply *, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *, const QList<QSslError> &errors);
    void setUserAgent(QNetworkRequest &request);
    void doRequest(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &data);
public:
    QVariant statusCode;
    QByteArray responseData;
    static User user;
private:
    QUrl url;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    double replyDataSize;
    QFile httpLog;
    /* user */
    int timeout;
    QString strHostUrl;
};

#endif // HTTPWORKER_H
