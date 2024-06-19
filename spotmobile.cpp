#include "spotmobile.h"
#include "spotmobilehandler.h"
#include <QtNetwork/QTcpSocket>
#include <QApplication>

SpotMobile::SpotMobile(SpotLite *sl, MainWindow *mw, QObject *parent) :
    QObject(parent), _sl(sl), _mw(mw)
{
    connect(&_serverSocket, SIGNAL(newConnection()), this, SLOT(onNewConnection()) );
    //_docroot = QApplication::applicationDirPath()+"/SpotMobile";
    _docroot = ":";
}

void SpotMobile::listen(const QHostAddress &address, quint16 port, const QByteArray &username, const QByteArray &password)
{
    if (password.isEmpty())
        _auth.clear();
    else
        _auth = QByteArray(username+":"+password).toBase64();

    _serverSocket.listen(address, port);
}

void SpotMobile::onNewConnection()
{
    QTcpSocket *socket = _serverSocket.nextPendingConnection();
    QByteArray  ip     = socket->peerAddress().toString().toLatin1();

    if ( _handlers.count(ip) > MAX_CONNECTIONS_PER_IP ) /* Anti DoS */
    {
        delete socket;
        return;
    }

    SpotMobileHandler *h = new SpotMobileHandler(socket, _docroot, _sl, _mw, this);
    connect(h, SIGNAL(destroyed(QObject*)), this, SLOT(onHandlerDone(QObject*)));
    if ( !_auth.isEmpty() )
        h->setAuth(_auth);
}

void SpotMobile::onHandlerDone(QObject *obj)
{
    SpotMobileHandler *h = static_cast<SpotMobileHandler *>(obj);
    _handlers.remove( _handlers.key(h) );
}
