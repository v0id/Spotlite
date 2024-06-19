#include "spotnntp.h"
#include "yencmemdecoder.h"
#include "qtioprocessor/qtiocompressor.h"
#include <stdexcept>
#include <QTimer>
#include <QBuffer>
#include <QHostAddress>
#include <QDir>
#include <QApplication>
#include <QDirIterator>

const char *SpotNNTP::EOA = "\r\n.\r\n";


SpotNNTP::SpotNNTP(QString server, int port, QString username, QString password, bool ssl, bool xzver, QObject *parent) :
    QObject(parent), _server(server), _port(port), _username(username), _password(password), _ssl(ssl), _bytesReceived(0), _pendingDelete(false), _dec(NULL)
{
#ifndef QT_NO_SSL
    if (ssl)
    {
        _socket = new QSslSocket(this);
        connect(_socket, SIGNAL(encrypted()), this, SLOT(onConnected()) );
        connect(_socket, SIGNAL(peerVerifyError(QSslError)), this, SLOT(onSSLerror(QSslError)) );

        QString certdir = QApplication::applicationDirPath()+"/certificates";
        if ( QFile::exists(certdir) )
        {
            QDirIterator iter(certdir, QStringList("*.*"), QDir::NoDotAndDotDot);
            while (iter.hasNext())
            {
                iter.next();
                QString cert = iter.filePath();
                if ( static_cast<QSslSocket *>(_socket)->addCaCertificates(cert) )
                    qDebug() << "Extra certificaat toegevoegd:" << cert;
                else
                    qDebug() << "Fout bij toevoegen extra certificaat:" << cert;
            }
        }
    }
    else
    {
#endif
        _socket = new QTcpSocket(this);
        connect(_socket, SIGNAL(connected()), this, SLOT(onConnected()) );
#ifndef QT_NO_SSL
    }
#endif
    connect(_socket, SIGNAL(connected()), this, SLOT(onConnected()) );
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()) );
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)) );

    _timer.setSingleShot(true);
    _timer.setInterval(TIMEOUT * 1000);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimeout()) );

    if (xzver)
        _dec = new YencMemDecoder(this);
}

void SpotNNTP::login()
{
    _group = "";

    if (_pendingDelete) /* Action cancelled */
    {
        deleteLater();
        return;
    }

    if ( !_socket->isOpen() )
    {
        _state = waitingForWelcome;
#ifndef QT_NO_SSL
        if (_ssl)
        {
            QSslSocket *s = static_cast<QSslSocket *> (_socket);
            s->connectToHostEncrypted(_server, _port);
        }
        else
#endif
            _socket->connectToHost(_server, _port);

        _timer.start();
    }
}

void SpotNNTP::logout_and_delete()
{
    if ( _socket->isOpen() )
    {
        _state = quitSent;
        cmd("QUIT");
    }
    else
        delete this;
}

void SpotNNTP::onConnected()
{
    _socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    _socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

void SpotNNTP::onError(QAbstractSocket::SocketError socketError)
{
    if ( _state == quitSent && socketError == QAbstractSocket::RemoteHostClosedError )
    {
        deleteLater();
    }
    else
    {
        QString err = _socket->errorString();
        qDebug() << "Socket fout: (" << socketError << ") " << err;
        abort_and_emit_error(socketError, err);
    }
}

void SpotNNTP::abort()
{
    _msgid = _group = "";
    _state = none;

    if (_timer.isActive() )
        _timer.stop();
//    _socket->abort();
    _socket->close();
}

void SpotNNTP::abort_and_emit_error(int code, const QString &msg)
{
    QByteArray tmpmsgid = _msgid;
    abort();
    emit error(code, msg, tmpmsgid);
}

void SpotNNTP::onReadyRead()
{
    if ( _timer.isActive() )
        _timer.stop();

    if (_state == downloading)
    {
        QByteArray data = _socket->readAll();
        const char *c = data.constData();
        int l = data.length();
        bool eof = false;
        _bytesReceived += l;

        /* articles end with <CR><LF>.<CR><LF> */
        if ( (l > 4 && !strcmp(c+l-5, EOA)) || (l+_crlfpos == 5 && !strcmp(c, EOA+_crlfpos)) )
        {
            eof = true;
        }
        else if (l > 3 && c[l-4] == '\r' && c[l-3] == '\n' && c[l-2] == '.' && c[l-1] == '\r' )
            _crlfpos = 4; /* partial crlf <CR><LF>.<CR> */
        else if (l > 2 && c[l-3] == '\r' && c[l-2] == '\n' && c[l-1] == '.' )
            _crlfpos = 3; /* partial crlf <CR><LF>. */
        else if (l > 1 && c[l-2] == '\r' && c[l-1] == '\n')
            _crlfpos = 2;  /* partial crlf <CR><LF> */
        else if (l > 0 && c[l-1] == '\r')
            _crlfpos = 1;
        else
            _crlfpos = 0;
        /* TODO: should we handle one-byte-at-a-time responses <5 characters? */

        _buf.append(data);

        if (eof)
        {
            QByteArray tmpbuf = _buf;
            QByteArray tmpid  = _msgid;
            _state = sloggedIn; _msgid = ""; _buf.clear();
            emit articleData(tmpid, tmpbuf);
        }

        return;
    }

    if ( !_socket->canReadLine() )
        return;

    QByteArray line = _socket->readLine();
    _bytesReceived += line.count();

    if (_state != downloading && _state != headerDownloading && _state != xroverDownloading)
    {
        qDebug() << "R: " << line.trimmed();

        if ( line.length() < 3 )
            return; /* Invalid input, expecting: 123 Message */

        int code = line.left(3).toInt();
        bool ok = (code >= 200 && code <= 399);

        if (!ok)
        {
            if (code == ARTICLE_NOT_FOUND || code == ARTICLE_NUM_NOT_FOUND)
            {
                _state = sloggedIn;
                emit articleNotFound(_msgid);
                return;
            }
            else if (_state == xroverSent)
            {
                _state = sloggedIn;
                emit error(XHDR_NOT_SUPPORTED, "", "");
                return;
            }
            else if (code == 411 && _group == "alt.binaries")
            {
                //
            }
            else
            {
                abort_and_emit_error(code, line);
            }

        }
    }

    switch (_state)
    {
        case waitingForWelcome:
            if ( !_password.isEmpty() )
            {
                _state = userSent;
                cmd("AUTHINFO USER "+_username.toLatin1() );
                break;
            }
            // fallthrough

        case passSent:
            _state = sloggedIn;
            //if (_msgid.isEmpty() )
                emit loggedIn();
            /*else
            {
                _state = bodySent;

                if (_msgid.startsWith('>'))
                    cmd("BODY <"+_msgid.mid(1)+">");
                else if (_msgid.contains('@'))
                    cmd("ARTICLE <"+_msgid+">");
                else
                    cmd("ARTICLE "+_msgid);
            }*/

            break;

        case userSent:
            _state = passSent;
            cmd("AUTHINFO PASS "+_password.toLatin1() );
            break;

        case bodySent:
            _crlfpos = 0;
            _state = downloading;
            onReadyRead();
            break;

        case xoverSent:
            _state = headerDownloading;
            onReadyRead();
            break;

        case xroverSent:
            _state = xroverDownloading;
            onReadyRead();
            break;

        case headerDownloading:
            do
            {
                if (_dec)
                {
                    _dec->dataReceived(line);

                    //qDebug() << "LINEtje: " << line;

                    if (line == ".\r\n")
                    {
                        _state = sloggedIn;
                        qDebug() << "unzippen";

                        if (!_dec->ok() )
                        {
                            emit error(0, tr("Foutieve gecompreseerde headers ontvangen (yenc crc niet ok)"), "");
                            return;
                        }


                        QByteArray &data = _dec->decodedData();
                        QBuffer     buffer(&data);
                        QtIOCompressor unzip(&buffer);
                        unzip.setStreamFormat(unzip.RawZipFormat);
                        unzip.open(unzip.ReadOnly);
                        QByteArray unzipped = unzip.readAll();
                        qDebug() << "compressed:" << data.length() << "bytes, uncompressed:" << unzipped.length();
                        unzip.close();
                        buffer.setBuffer(&unzipped);
                        _dec->clear();
                        buffer.open(buffer.ReadOnly);

                        int lines = 0;
                        while (buffer.canReadLine() )
                        {
                            emit headerLine( buffer.readLine() );
                            lines++;
                        }

                        buffer.close();
                        emit headerLine(line);
                    }
                }
                else
                {
                    if (line == ".\r\n")
                    {
                        _state = sloggedIn;
                        // break?
                    }
                    emit headerLine(line);
                }

            } while ( line != ".\r\n" && _socket->canReadLine() && &(line = _socket->readLine()) );
            break;

        case xroverDownloading:
            do
            {
                if (_dec)
                {
                    _dec->dataReceived(line);

                    if (line == ".\r\n")
                    {
                        _state = sloggedIn;

                        if (!_dec->ok() )
                        {
                            emit error(0, tr("Foutieve gecompreseerde headers ontvangen (yenc crc niet ok)"), "");
                            return;
                        }

                        QByteArray &data = _dec->decodedData();
                        QBuffer     buffer(&data);
                        QtIOCompressor unzip(&buffer);
                        unzip.setStreamFormat(unzip.RawZipFormat);
                        unzip.open(unzip.ReadOnly);
                        QByteArray unzipped = unzip.readAll();
                        qDebug() << "compressed:" << data.length() << "bytes, uncompressed:" << unzipped.length();
                        unzip.close();
                        buffer.setBuffer(&unzipped);
                        _dec->clear();
                        buffer.open(buffer.ReadOnly);

                        while ( buffer.canReadLine() )
                        {
                            QByteArray bufline = buffer.readLine();
                            QList<QByteArray> items = bufline.split(' ');
                            if (items.count() == 2)
                                emit xroverLine(items[0].toLongLong(), items[1].replace("\r\n", "") );
                        }
                        emit xroverLine(-1, ".");
                    }
                }
                else
                {
                    if (line == ".\r\n")
                    {
                        _state = sloggedIn;
                        // break?
                        emit xroverLine(-1, ".");
                        break;
                    }
                    else
                    {
                        QList<QByteArray> items = line.split(' ');
                        if (items.count() == 2)
                            emit xroverLine(items[0].toLongLong(), items[1].replace("\r\n", "") );
                    }
                }

            } while ( _socket->canReadLine() && &(line = _socket->readLine()) );
            break;

        case groupSent:
            {
                qint64 min = 0;
                qint64 max = 0;
                QList<QByteArray> l = line.split(' ');
                QByteArray groupname;

                if (l.count() > 4)
                {
                    min = l[2].toLongLong();
                    max = l[3].toLongLong();
                    groupname = l[4].replace("\r\n", "");
                }

                _state = sloggedIn;
                emit groupResponse(groupname, min, max);
            }
            break;

        case postSent:
            _socket->write(_buf);
            _buf.clear();
            _state = posting;
            break;

        case posting:
            _state = sloggedIn;
            emit postingComplete();
            break;

        default:
            break;
    }
}

void SpotNNTP::cmd(QByteArray s)
{
    if (_state == passSent)
        qDebug() << "Wachtwoord verzonden";
    else
        qDebug() << "S: " << s;

    s += "\r\n";
    _socket->write(s);
    _timer.start();
}

void SpotNNTP::download(const QByteArray &msgid)
{
    _timerActive = false;
    if ( !_socket->isOpen() )
       login();

    _msgid   = msgid;

    if ( _state == sloggedIn )
    {
        _state = bodySent;
        if (msgid.startsWith('>'))
            cmd("BODY <"+msgid.mid(1)+">");
        else if (msgid.contains('@'))
            cmd("ARTICLE <"+msgid+">");
        else
            cmd("ARTICLE "+msgid);
    }
}

void SpotNNTP::retry_after_seconds(int seconds)
{
    _timerActive = true;
    QTimer::singleShot(seconds*1000, this, SLOT(login()) );
    qDebug() << "Opnieuw proberen in" << seconds << "seconden";
}

void SpotNNTP::deleteLater()
{
    if (_timerActive)
    {
        _pendingDelete = true;
    }
    else
    {
        QObject::deleteLater();
    }
}

bool SpotNNTP::idle() const
{
    return (_state == sloggedIn);
}

void SpotNNTP::xover(qint64 min, qint64 max)
{
    if ( !_socket->isOpen() )
       login();

    if ( _state == sloggedIn )
    {
        _state = xoverSent;
        if (_dec)
            cmd("XZVER "+QByteArray::number(min, 10)+"-"+QByteArray::number(max, 10));
        else
            cmd("XOVER "+QByteArray::number(min, 10)+"-"+QByteArray::number(max, 10));
    }
 }

void SpotNNTP::xrover(qint64 min, qint64 max)
{
    if ( !_socket->isOpen() )
       login();

    if ( _state == sloggedIn )
    {
        _state = xroverSent;
        if (_dec)
            cmd("XZHDR References "+QByteArray::number(min, 10)+"-"+QByteArray::number(max, 10));
        else
            cmd("XHDR References "+QByteArray::number(min, 10)+"-"+QByteArray::number(max, 10));
    }
 }


void SpotNNTP::group(const QByteArray &group)
{
    if ( !_socket->isOpen() )
       login();

    if ( _state == sloggedIn )
    {
        _state = groupSent;
        cmd("GROUP "+group);
        _group = group;
    }
}

QByteArray SpotNNTP::currentMSGid() const
{
    return _msgid;
}

QByteArray SpotNNTP::currentGroup() const
{
    return _group;
}

#ifndef QT_NO_SSL
void SpotNNTP::onSSLerror(const QSslError &error)
{
    QString msg = tr("SSL fout! ");
    int cacount = QSslSocket::systemCaCertificates().count();

    QSslError::SslError eno = error.error();
    if ( !eno || eno == QSslError::SelfSignedCertificate)
    {
        if (cacount)
            msg += tr("<p>Het SSL certificaat van de sever is niet uitgegeven door een vertrouwde instantie (zoals RapidSSL, StartSSL of Comodo).<br>Zonder betrouwbaar certificaat is het niet mogelijk een <b>VEILIGE</b> verbinding op te zetten.<p><b>Neem contact met je provider op!</b><p>Extra info: je was verbonden met server IP adres %1, je besturingssysteem kent %2 verschillende CAs, het certificaat dat de server gebruikt is te zien via DebugView.").arg( _socket->peerAddress().toString(), QString::number(cacount) );
        else
            msg += tr("Lijst met certificate authorities corrupt");
    }
    else if ( eno == QSslError::HostNameMismatch)
        msg += tr("Je hebt '%1' als server adres opgegeven, maar het SSL certificaat van de server is alleen geldig voor '%2'").arg(_server, error.certificate().subjectInfo(QSslCertificate::CommonName).join("/"));
    else
        msg += ": "+error.errorString();

    qDebug() << "Aantal door besturingssysteem vertrouwde CAs:" << cacount;
    qDebug() << "SSL fout:" << error.errorString();
    qDebug() << "Certificaat:" << error.certificate().toPem();
    qDebug() << "Certificaat ketting:" << static_cast<QSslSocket *>( _socket )->peerCertificateChain();
    //qDebug() << "Door besturingssysteem vertrouwede CAs:" << QSslSocket::systemCaCertificates();

    abort_and_emit_error(SSL_ERROR, msg);
}
#endif

void SpotNNTP::post(const QByteArray &postdata)
{
    _buf = postdata;
    _state = postSent;
    cmd("POST");
}

void SpotNNTP::onTimeout()
{
    qDebug() << "TIMEOUT!";
    abort_and_emit_error(TIMEOUT_EXCEEDED, tr("Timeout - server reageert niet binnen %1 sec.").arg(TIMEOUT) );
}
