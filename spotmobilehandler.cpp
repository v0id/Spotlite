#include "spotmobilehandler.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QFileInfo>
#include <QBuffer>
#include <QDateTime>
#include <QVariantMap>
#include <QVariant>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <qtioprocessor/qtiocompressor.h>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QSettings>

#include "parsedate.h"
#include "mainwindow.h"
#include "json/serializer.h"
#include "spotlite.h"
#include "customfilter.h"
#include "spotsmodel.h"
#include "spotmobileview.h"

SpotMobileHandler::SpotMobileHandler(QTcpSocket *socket, const QString &docroot, SpotLite *sl, MainWindow *mw, QObject *parent) :
    QObject(parent), _socket(socket), _sl(sl), _mw(mw)
{
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    _socket->setParent(this);

    //QFileInfo fi(docroot);
    //_docroot = fi.absoluteFilePath();
    _docroot = docroot;
    //qDebug()<<"docroot" << docroot;

    _timer.start(10000);
}

void SpotMobileHandler::onReadyRead()
{
    QByteArray buf = _socket->peek(MAX_HEADER_SIZE);
    int p1         = buf.indexOf("\r\n\r\n");
    if ( p1 != -1 )
    {
        _timer.stop();
        buf = _socket->read(p1+4).replace("\r", "");

        QMap<QByteArray,QByteArray> headers;
        QList<QByteArray> lines = buf.split('\n');

        if (lines.count() < 2 || !lines[0].contains(' ') )
        {
            generateError(500, "Invalid request");
            return;
        }

        QList<QByteArray> reqparts = lines[0].split(' ');

        for (int i=1; i<lines.count(); i++)
        {
            p1 = lines[i].indexOf(':');

            if (p1 != -1 )
            {
                headers.insert(lines[i].left(p1), lines[i].mid(p1+2));
            }
        }

//        qDebug() << "Request:" << reqparts;
//        qDebug() << "Headers" << headers;

        if ( !_auth.isEmpty() && headers.value("Authorization") != "Basic "+_auth )
        {
            _socket->write("HTTP/1.1 401 Authorization Required\r\n"
                           "WWW-Authenticate: Basic realm=\"Gebruikersnaam en wachtwoord graag\"\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: 12\r\n"
                           "\r\n"
                           "Geen toegang");
            _timer.start(KEEPALIVE_TIMEOUT);
            return;
        }

        handleRequest(reqparts[0].toUpper(), QUrl::fromEncoded("http://"+headers.value("Host", "server")+reqparts[1], QUrl::TolerantMode), headers );
    }
    else if ( _socket->bytesAvailable() > MAX_HEADER_SIZE )
    {
       _socket->close();
    }
}

void SpotMobileHandler::onTimeout()
{
    qDebug() << "Spotmobile timeout. IP: " << _socket->peerAddress();
    onError(_socket->SocketTimeoutError);
}

void SpotMobileHandler::onError(QAbstractSocket::SocketError)
{
    if (_socket->isOpen() )
        _socket->close();

    deleteLater();
}

void SpotMobileHandler::handleRequest(const QByteArray &method, const QUrl &url, const QMap<QByteArray,QByteArray> &headers)
{
    if (method == "GET")
    {
        if ( url.path().startsWith("/dynamic/"))
        {
            QByteArray script = url.path().mid(9).toLatin1();

            if (script == "cats.js")
            {
                _generateCats();
            }
            else if (script == "spots.js" && url.hasQueryItem("cat") && url.hasQueryItem("limit") )
            {
                int start = ( url.hasQueryItem("start") ? url.queryItemValue("start").toInt() : 0 );
                _generateSpotList(url.queryItemValue("cat").toLatin1(), start, url.queryItemValue("limit").toInt() );
            }
            else if (script == "spot" && url.hasQueryItem("id"))
            {
                /* TODO: fix me. this should not be here */
                QString skinpath = "/usr/local/share/SpotLite/skins/";
                if ( !QFile::exists(skinpath))
                {
                    skinpath = QApplication::applicationDirPath()+"/skins/";
                }
                skinpath += _sl->settings()->value("skin").toString();
                /* --- */

                SpotMobileView *smv = new SpotMobileView(_sl, this, url.queryItemValue("id").toInt(), skinpath, this);
                smv->loadSpot();
                /* TODO: prevent pipelining? */
            }
            else
            {
                generateError(404, "Script not found");
            }

            return;
        }

        QFileInfo fi(_docroot+url.path());
        QString filename;
        if (_docroot.startsWith("qrc:") )
            filename = _docroot+url.path();
        else
            filename = fi.canonicalFilePath();

        if (filename.endsWith('/'))
            filename += "index.html";

        if ( !filename.startsWith(_docroot+"/") )
        {
            generateError(403, "Forbidden");
            qDebug() << "403" << filename << _docroot+url.path();
            return;
        }

        if ( headers.contains("If-Modified-Since") )
            serveFile(filename, stringToQDateTime(headers.value("If-Modified-Since")));
        else
            serveFile(filename);
    }
    /*else if (method == "POST")
    {

    }*/
    else
    {
        generateError(405, "Method not allowed");
    }
}

void SpotMobileHandler::generateError(int code, const QByteArray &msg)
{
    QByteArray errstr = QByteArray::number(code)+" "+msg;
    _socket->write("HTTP/1.1 "+errstr+"\r\n"
                   "Content-Type: text/html\r\n"
                   "Content-Length: "+QByteArray::number(errstr.length()+2)+"\r\n"
                   "\r\n\r\n"
                   +errstr+"\r\n");
    _timer.start(KEEPALIVE_TIMEOUT);
}

void SpotMobileHandler::serveFile(const QString &filename, const QDateTime &iflastmod)
{
    QFile f(filename);
    QByteArray ctype;
    QDateTime lastmod = QFileInfo(f).lastModified();

//    qDebug() << "Serving" << filename;

    if (!f.exists() || QFileInfo(f).isDir() )
    {
        generateError(404, "File not found");
        return;
    }
    else if (iflastmod.isValid() && iflastmod == lastmod )
    {
//        qDebug() << "Not modified:" << filename;
        _socket->write("HTTP/1.1 304 Not modified\r\n\r\n");
        _timer.start(KEEPALIVE_TIMEOUT);
        return;
    }

    if (filename.endsWith(".html"))
        ctype = "text/html";
    else if (filename.endsWith(".js"))
        ctype = "text/javascript";
    else if (filename.endsWith(".css"))
        ctype = "text/css";
    else
        ctype = "application/octet-stream";

    f.open(f.ReadOnly);
    sendResponse(ctype, f.readAll(), lastmod );
    f.close();
}

inline QString toRFC822Date(const QDateTime &localTime)
{
   QLocale locale( QLocale::c() );

   return locale.toString( localTime.date(), "ddd, dd MMM yyyy" ) + ' ' +
          locale.toString( localTime.time(), "hh:mm:ss" );
}

void SpotMobileHandler::sendResponse(const QByteArray &ctype, const QByteArray &data, const QDateTime &lastmod)
{
    if (_forceGzip)
    {
        QByteArray cdata;

        if (data.startsWith("\x1f\x8b"))
        {
            cdata = data;
        }
        else
        {
            QBuffer buf(&cdata);
            QtIOCompressor comp(&buf);
            comp.setStreamFormat(comp.GzipFormat);
            comp.open(comp.WriteOnly);
            comp.write(data);
            comp.close();
        }

        QByteArray headers = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: "+ctype+"\r\n"
                               "Content-Encoding: gzip\r\n"
                               "Content-Length: "+QByteArray::number(cdata.size() )+"\r\n";
        if ( lastmod.isValid() )
        {
            headers += "Last-Modified: "+toRFC822Date(lastmod.toUTC() )+" GMT\r\n";
        }

        _socket->write(headers+"\r\n"+cdata);
    }
    else
    {
        _socket->write("HTTP/1.1 200 OK\r\n"
                       "Content-Type: "+ctype+"\r\n"
                       "Content-Length: "+QByteArray::number(data.size() )+"\r\n"
                       "\r\n"+
                       data
                       );
    }

    _timer.start(KEEPALIVE_TIMEOUT);
}

void SpotMobileHandler::_generateCats()
{
    QVariantMap root;
    QVariantList cats;
    QJson::Serializer s;
    QTreeWidget *tree = _mw->catTree();

    root.insert("text", "cats");

    for (int i=0; i < tree->topLevelItemCount(); i++)
    {
        QVariantMap m;
        QTreeWidgetItem *topwi = tree->topLevelItem(i);

        if ( topwi->isHidden() )
            continue;

        m.insert("text", topwi->text(0));
        if ( topwi->childCount() )
        {
            QVariantList l;
            QVariantMap alles;
            alles.insert("id", i);
            alles.insert("text", "Alles in "+topwi->text(0));
            alles.insert("leaf", true);
            l.append(alles);

            for (int j=0; j < topwi->childCount(); j++)
            {
                QVariantMap subitem;
                QTreeWidgetItem *subwi = topwi->child(j);
                subitem.insert("id", QByteArray::number(i)+"-"+QByteArray::number(j));
                subitem.insert("text", subwi->text(0));
                subitem.insert("leaf", true);

                if ( subwi->data(0, Qt::UserRole).type() == QVariant::UserType )
                    l.append(subitem);
            }

            m.insert("items", l);
        }
        else
        {
            if ( topwi->data(0, Qt::UserRole).type() != QVariant::UserType && topwi->data(0, Qt::UserRole).toInt() < 1 )
                continue;

            m.insert("id", QByteArray::number(i));
            m.insert("leaf", true);
        }

        cats.append(m);
    }

    root.insert("items", cats);

    sendResponse("text/javascript", s.serialize(root), QDateTime::currentDateTime() );
}

void SpotMobileHandler::_generateSpotList(const QByteArray &catid, int start, int limit)
{
  //  qDebug() << catid << start;

    if (catid.isEmpty() || start < 0)
    {
        generateError();
        return;
    }

    /* Opzoeken om welke categorie er gevraagd wordt */
    int i;
    QTreeWidget *tree = _mw->catTree();
    QTreeWidgetItem *wi;
    QList<QByteArray> catparts = catid.split('-');

    i = catparts[0].toInt();
    if (i < 0 || i > tree->topLevelItemCount() )
    {
        generateError();
        return;
    }
    wi = tree->topLevelItem(i);

    if ( catparts.count() == 2)
    {
        i = catparts[1].toInt();
        if (i < 0 || i > wi->childCount() )
        {
            generateError();
            return;
        }

        wi = wi->child(i);
    }

    /* Filterinformatie opvragen, en database raadplegen */
    QSqlQuery q(*_sl->spotDatabase());
    CustomFilter f;
    QVariant v = wi->data(0, Qt::UserRole);

    if ( v.type() == v.UserType)
    {
        f = v.value<CustomFilter>();
    }
    else
    {
        int c = v.toInt();
        if (c > 0)
        {
            f.setCat(c);
        }
    }

    QByteArray table = "spots", customquery = f.query();
    if ( f.cat() == 9 )
    {
        table = "erospots";
        q.prepare("SELECT 0-id,subcat,spotdate,title,spotter FROM erospots WHERE "+customquery+" LIMIT ? OFFSET ?");
    }
    else
    {
        q.prepare("SELECT id,subcat,spotdate,title,spotter FROM spots WHERE "+customquery+" LIMIT ? OFFSET ?");
    }

    q.addBindValue(limit);
    q.addBindValue(start);
    if (!q.exec() )
    {
        qDebug() << "Fout bij uitvoeren query" << q.lastError();
        generateError();
        return;
    }

    QVariantList spots;
    while ( q.next() )
    {
        QVariantMap m;
        m.insert("id", q.value(0).toInt() );
        m.insert("subcat", SpotsModel::subcategoryname( q.value(1).toInt() ) );
        m.insert("spotdate", q.value(2).toInt());
        m.insert("title", _filterHTML(q.value(3).toByteArray() ));
        m.insert("spotter", _filterHTML(q.value(4).toByteArray() ));

        spots.append(m);
    }

    int total = spots.count();
    if (total == limit)
    {
        q.prepare("SELECT count(*) FROM "+table+" WHERE "+customquery.left(customquery.lastIndexOf(" ORDER BY")) );
        q.exec();
        q.next();
        total = q.value(0).toInt();
    }
    else
        total += start;

    QVariantMap root;
    QJson::Serializer s;
    root.insert("total", total);
    root.insert("items", spots);

    sendResponse("text/javascript", s.serialize(root), QDateTime::currentDateTime() );
}

QByteArray SpotMobileHandler::_filterHTML(const QByteArray &in)
{
    QByteArray out = in;
    return out.replace('<', "&lt;").replace('>', "&gt;");
}

void SpotMobileHandler::setAuth(const QByteArray &auth)
{
    _auth = auth;
}
