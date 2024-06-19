#ifndef SPOTNNTP_H
#define SPOTNNTP_H

#include <QObject>
#include <QString>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslSocket>
#include <QTimer>

class YencMemDecoder;

class SpotNNTP : public QObject
{
    Q_OBJECT
public:
    explicit SpotNNTP(QString server, int port, QString username, QString password, bool ssl, bool xzver, QObject *parent = 0);

    void download(const QByteArray &msgid);
    void group(const QByteArray &group);
    void xover(qint64 min, qint64 max);
    void xrover(qint64 min, qint64 max);
    void post(const QByteArray &postdata);
    void logout_and_delete();

    void abort();
    void abort_and_emit_error(int error, const QString &msg);
    void retry_after_seconds(int seconds);

    bool idle() const;
    QByteArray currentMSGid() const;
    QByteArray currentGroup() const;

    enum {PASSWORD_NEEDED = 480, PASSWORD_INCORRECT = 483, ARTICLE_NUM_NOT_FOUND = 423, ARTICLE_NOT_FOUND = 430, ERROR_WRITING_FILE = 9999, SETTINGS_CHANGE = 9998, XHDR_NOT_SUPPORTED = 9997, SSL_ERROR = 9996, TIMEOUT_EXCEEDED = 9995 } _SpotNNTPErrors;

public slots:
    void login();
    void deleteLater();


signals:
//    void dataDecoded(int seqnr, const QString &filename, quint64 bytes, bool ok, bool yenc, Nzb *nzb);
    void articleData(const QByteArray &msgid, QByteArray data);
    void headerLine(const QByteArray &line);
    void groupResponse(const QByteArray &group, qint64 min, qint64 max);
    void xroverLine(qint64 nr, const QByteArray &references);

    void error(int error, const QString &msg, const QByteArray &msgid);
    void articleNotFound(const QByteArray &msgid);
    void loggedIn();
    void postingComplete();

protected:
    QString _server;
    int _port;
    QString _username, _password;
    bool _ssl;
    QTcpSocket *_socket;
    QByteArray _msgid, _buf, _group;
    QTimer _timer;
    quint64 _bytesReceived;
    bool _pendingDelete, _timerActive;
    YencMemDecoder *_dec;

    static const int TIMEOUT = 30;
    static const char *EOA;
    int _crlfpos;

    enum {none, waitingForWelcome, userSent, passSent, sloggedIn, bodySent, downloading, quitSent, headSent, headerDownloading, nzbBodySent, groupSent, xoverSent, xroverSent, xroverDownloading, postSent, posting} _state;

    void cmd(QByteArray s);

protected slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onTimeout();

#ifndef QT_NO_SSL
    void onSSLerror(const QSslError &errors);
#endif
};

#endif // SPOTNNTP_H
