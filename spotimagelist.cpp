#include "spotimagelist.h"
#include "spotsignature.h"
#include "mainwindow.h"
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWebView>
#include <QFileDialog>
#include <QAction>
#include <QSqlQuery>
#include <QXmlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include <QTimer>

SpotImageList::SpotImageList(QWidget *parent) :
    QWidget(parent), _index(0), _total(0), _pagesize(16), _sl(NULL), _model(NULL)
{
    /* Templates */
    QString path = ":/tpl";
    QFile qf(path+"/imagelist.tpl");
    qf.open(qf.ReadOnly);
    _mainTemplate = qf.readAll();
    qf.close();

    qf.setFileName(path+"/imagelistitem.tpl");
    qf.open(qf.ReadOnly);
    _itemTemplate = qf.readAll();
    qf.close();

    /* Webbrowser spul */
    _web = new QWebView(this);
    _web->settings()->setObjectCacheCapacities(0,0,0);
    connect(_web, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClick(QUrl)));
    _web->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    QAction *act = _web->pageAction(QWebPage::Copy);
    act->setShortcut(QKeySequence::Copy);
    _web->addAction(act);

    connect(_web->page(), SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(_downloadRequested(QNetworkRequest)) );

    /* Toolbar spul */
    QToolBar *toolbar = new QToolBar( tr("Navigatie"), this);
    _prev = new QToolButton(toolbar);
    _prev->setIcon( QIcon(":/icons/resultset_previous.png") );
    _prev->setText( tr("Vorige pagina") );
    _prev->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _next = new QToolButton(toolbar);
    _next->setIcon( QIcon(":/icons/resultset_next.png") );
    _next->setText( tr("Volgende pagina") );
    _next->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addWidget(_prev);
    toolbar->addWidget(_next);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_web);
    layout->addWidget(toolbar);
    setLayout(layout);

    connect(_prev, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(_next, SIGNAL(clicked()), this, SLOT(nextPage()));
}

void SpotImageList::onLinkClick(const QUrl &url)
{
    if (url.scheme() == "spotlite")
    {
        int spotid = url.path().toInt();

        foreach (_SpotImageListItem *sil, _itemlist)
        {
            if ( sil->id() == spotid)
            {
                emit spotOpened(spotid, sil->title() );
                QTimer::singleShot(1000, this, SLOT(refresh()));
                break;
            }
        }
    }
}

void SpotImageList::refresh()
{
    if (!_model || !_sl)
        return;

    _total = _model->rowCount();
    foreach (_SpotImageListItem *sil, _itemlist)
    {
        delete sil;
    }
    _itemlist.clear();

    int _max = qMin(_total, _index+_pagesize );
    _suspendUpdate = true;
    for (int i=_index; i<_max; i++)
    {
        int spotid = _model->data( _model->index(i, 0), Qt::UserRole).toInt();
        _itemlist.append(new _SpotImageListItem(this, _sl, spotid));
    }
    _suspendUpdate = false;

    _prev->setEnabled(_index != 0);
    _next->setEnabled(_total > _max);


    updateHTML();
}

void SpotImageList::updateHTML()
{
    if (_suspendUpdate)
        return;

    QByteArray itemshtml, item, html = _mainTemplate;

    foreach (_SpotImageListItem *sil, _itemlist)
    {
        item = _itemTemplate;
        QByteArray imgdata = _toHTML(sil->imageurl() );
        if ( imgdata.length() < 4 )
        {
            imgdata = "";
        }

        item.replace("{spotid}", QByteArray::number(sil->id() ));
        item.replace("{title}", _toHTML(sil->title()) );
        item.replace("{imagedata}", imgdata);

        itemshtml += item;
    }

    html.replace("{items}", itemshtml);
    _web->setHtml(html);
}

void SpotImageList::_downloadRequested(const QNetworkRequest &request)
{
    QString defaultFileName = QFileInfo(request.url().toString()).fileName();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Bestand opslaan"), defaultFileName);
    if (fileName.isEmpty())
        return;

    QNetworkRequest newRequest = request;
    newRequest.setAttribute(QNetworkRequest::User, fileName);
    QNetworkAccessManager *networkManager = _web->page()->networkAccessManager();
    QNetworkReply *reply = networkManager->get(newRequest);
    connect(reply, SIGNAL(finished()), this, SLOT(_downloadFinished()));
}

void SpotImageList::_downloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>( sender() );
    QString filename     = reply->request().attribute(QNetworkRequest::User).toString();
    QFile fd(filename);
    fd.open(fd.WriteOnly);
    fd.write( reply->readAll() );
    fd.close();
    reply->deleteLater();
}

void SpotImageList::setSpotLite(SpotLite *sl)
{
    _sl = sl;
}

void SpotImageList::setModel(QAbstractTableModel *model)
{
    _model = model;
}

_SpotImageListItem::_SpotImageListItem(SpotImageList *parent, SpotLite *sl, int spotid)
    : QObject(parent), _parent(parent), _sl(sl), _spotid(spotid)
{
    QSqlQuery q(*_sl->spotDatabase() );
    if (spotid < 0)
    {
        q.prepare("SELECT msgid,title FROM erospots WHERE id=?");
        q.addBindValue(0-spotid);
    }
    else
    {
        q.prepare("SELECT msgid,title FROM spots WHERE id=?");
        q.addBindValue(spotid);
    }
    q.exec();

    if ( q.next() )
    {
        connect(_sl, SIGNAL(articleData(QByteArray,QByteArray)), this, SLOT(onArticleData(QByteArray,QByteArray)));
        connect(_sl, SIGNAL(articleNotFound(QByteArray)), this, SLOT(onArticleNotFound(QByteArray)));

        _msgid = q.value(0).toByteArray();
        if (!_msgid.contains('@'))
            _msgid += "@free.pt";
        _title = q.value(1).toString();
        _sl->downloadArticle(_msgid);
    }
}

void _SpotImageListItem::onArticleData(const QByteArray &msgid, QByteArray data)
{
    if (msgid == _msgid)
    {
        _parseSpot(data);
        _msgid.clear();
    }
    else
    {
        if ( _imagemsgids.contains(msgid) )
        {
            _imagedata.append( _decodeIMG(data) );
            _imagemsgids.removeOne(msgid);

            if (_imagemsgids.empty() )
            {
                if (!_imagedata.isEmpty() )
                {
                    QByteArray _imgtype;

                    if ( _imagedata.startsWith("\x89PNG\r\n") )
                        _imgtype = "image/png";
                    else /* TODO: jpg sanity check? */
                        _imgtype = "image/jpeg";

                    _imagedata = "data:"+_imgtype+";base64,"+_imagedata.toBase64();
                }

                _parent->updateHTML();
            }
        }
    }
}

void _SpotImageListItem::onArticleNotFound(const QByteArray &)
{

}

QString _SpotImageListItem::title()
{
    return _title;
}

QByteArray  _SpotImageListItem::imageurl()
{
    if ( _imagemsgids.empty() )
        return _imagedata;
    else
        return "";
}

QByteArray SpotImageList::_toHTML(const QString &in)
{
    static const char *replacements[] =
    {
        "<", "&lt;",
        ">", "&gt;",
        "&", "&amp;",
        "\"", "&quot;",
        0
    };

    QByteArray h = in.toLatin1();
    for (const char **r = replacements; *r; r+=2)
    {
        h.replace(*r, r[1]);
    }

    return h;
}

QByteArray _SpotImageListItem::_decodeIMG(const QByteArray &data)
{
//    qDebug() << "IMG DATA" << data;

    QByteArray _buf = data;

    _buf.replace("\n..", "."); /* Todo: start of msg? */
    _buf.replace("\n", "");
    _buf.replace("\r", "");
    _buf.replace("=C", "\n");
    _buf.replace("=B", "\r");
    _buf.replace("=A", QByteArray(1, 0));
    _buf.replace("=D", "=");
    _buf.chop(1);

    return _buf;
}

inline void doXpath(QXmlQuery &q, QString &result, const char *query)
{
    q.setQuery(query);
    q.evaluateTo(&result);
    result = result.replace('\n', "").replace("&amp;", "&");
}

bool _SpotImageListItem::_parseSpot(const QByteArray &data)
{
    int pos = data.indexOf("\r\n\r\n");
    if (pos == -1)
        return false;

    QByteArray xml, rawmodulus, rawexponent, signature, msgid, xmlsignature;
    QString description, image, nzbref, sizestr, imgref;
    QXmlQuery q;
    QList<QByteArray> headerlines = data.left(pos).split('\n');
    bool has_signature = false;


    foreach (QByteArray line, headerlines)
    {
        if (line.startsWith("X-XML: "))
            xml += line.mid(7);
        else if (line.startsWith("X-User-Key: "))
        {
            QString modulus, exponent;
            QByteArray rsaxmlkey = line.mid(12);
            q.setFocus(rsaxmlkey);
            doXpath(q, modulus, "/RSAKeyValue/Modulus/string()");
            doXpath(q, exponent, "/RSAKeyValue/Exponent/string()");
            QByteArray _spotterkey = modulus.toLatin1();

            rawmodulus  = QByteArray::fromBase64(_spotterkey);
            rawexponent = QByteArray::fromBase64(exponent.toLatin1() );
        }
        else if (line.startsWith("X-User-Signature: "))
        {
            signature = QByteArray::fromBase64( _unSpecial(line.mid(18)) );
        }
        else if (line.startsWith("Message-ID: <"))
        {
            msgid = line.mid(12);
            int p = msgid.indexOf(">");
            msgid = msgid.left(p+1);
        }
        else if (line.startsWith("X-XML-Signature: "))
        {
            xmlsignature = line.mid(17).replace('\r', "");
        }
    }
    xml = xml.replace('\r', "").replace('\n', "");

    if (xml.isEmpty())
        return false;

    if ( !signature.isEmpty() && !rawmodulus.isEmpty() && !rawexponent.isEmpty() )
    {
        qDebug() << "Checken van spot RSA...";
        has_signature = SpotSignature::verify(rawmodulus, rawexponent, msgid, signature);

        if (!has_signature)
        {
            has_signature = SpotSignature::verify(rawmodulus, rawexponent, xmlsignature, signature);
        }
        qDebug() << (has_signature ? "Handtekening OK!" : "Handtekening onjuist!");
    }

    if ( !has_signature )
    {
        return false;
    }

    xml = xml.replace("SpotNet>", "Spotnet>");

    q = QXmlQuery();
    q.setFocus(xml);
    doXpath(q, description, "/Spotnet/Posting/Description[1]/string()");
    if (description.isEmpty() )
    {
        qDebug() << "Fout bij het parsen van XML";
        return false;
    }

    doXpath(q, imgref, "/Spotnet/Posting/Image[1]/Segment[1]/string()");
    if ( imgref.isEmpty() )
    {
        doXpath(q, image, "/Spotnet/Posting/Image[1]/string()");
        if ( !image.isEmpty() )
        {
            _imagedata = image.toLatin1();
            _parent->updateHTML();
        }
    }
    else
    {
        QByteArray i = ">"+imgref.toLatin1();
        _imagemsgids.append( i );
        _sl->downloadArticle( i );
    }

    return true;
}

QByteArray _SpotImageListItem::_unSpecial(QByteArray s)
{
    s.replace("-s", "/").replace("-p", "+");

    switch (s.length() % 4)
    {
    case 0:
        return s;

    case 1:
        return s+"===";

    case 2:
         return s+"==";

    case 3:
         return s+"=";
    default:
         return "";
    }
}

int _SpotImageListItem::id()
{
    return _spotid;
}

void SpotImageList::previousPage()
{
    _index -= _pagesize;
    if (_index < 0)
        _index = 0;

    refresh();
}

void SpotImageList::nextPage()
{
    _index += _pagesize;

    refresh();
}

void SpotImageList::setIndex(int idx)
{
    _index = idx;
}
