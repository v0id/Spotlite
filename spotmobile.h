#ifndef SPOTMOBILE_H
#define SPOTMOBILE_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QMultiMap>

class SpotMobileHandler;
class SpotLite;
class MainWindow;

class SpotMobile : public QObject
{
    Q_OBJECT
public:
    explicit SpotMobile(SpotLite *sl, MainWindow *mw, QObject *parent = 0);
    void listen(const QHostAddress &address, quint16 port, const QByteArray &username = "", const QByteArray &password = "");

signals:

public slots:
    void onNewConnection();
    void onHandlerDone(QObject *obj);

protected:
    QTcpServer _serverSocket;
    QByteArray _auth;
    QMultiMap<QByteArray,SpotMobileHandler *> _handlers;
    QString _docroot;
    SpotLite *_sl;
    MainWindow *_mw;


    static const int MAX_CONNECTIONS_PER_IP = 100;
};

#endif // SPOTMOBILE_H
