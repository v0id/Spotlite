#ifndef SPOTMOBILEHANDLER_H
#define SPOTMOBILEHANDLER_H

#include <QObject>
#include <QUrl>
#include <QTimer>
#include <QTcpSocket>
#include <QDateTime>

class SpotLite;
class MainWindow;

class SpotMobileHandler : public QObject
{
    Q_OBJECT
public:
    explicit SpotMobileHandler(QTcpSocket *socket, const QString &docroot, SpotLite *sl, MainWindow *mw, QObject *parent = 0);
    void setAuth(const QByteArray &_auth);
    void sendResponse(const QByteArray &ctype, const QByteArray &data, const QDateTime &lastmod);

signals:

public slots:
    void onReadyRead();
    void onTimeout();
    void onError(QAbstractSocket::SocketError socketError);

protected:
    QByteArray _auth;
    QTcpSocket *_socket;
    QTimer _timer;
    QString _docroot;
    SpotLite *_sl;
    MainWindow *_mw;

    static const int MAX_HEADER_SIZE = 1024;
    static const int KEEPALIVE_TIMEOUT = 121000;
    static const bool _forceGzip = true;

    void handleRequest(const QByteArray &method, const QUrl &url, const QMap<QByteArray,QByteArray> &headers);
    void generateError(int code = 500, const QByteArray &msg = "Internal server error");
    void serveFile(const QString &filename, const QDateTime &iflastmod = QDateTime() );

    void _generateCats();
    void _generateSpotList(const QByteArray &catid, int start, int limit);
    QByteArray _filterHTML(const QByteArray &in);
};

#endif // SPOTMOBILEHANDLER_H
