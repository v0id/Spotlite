#include "spotmobileview.h"
#include "spotmobilehandler.h"
#include <QDateTime>
#include <QFile>

SpotMobileView::SpotMobileView(SpotLite *sl, SpotMobileHandler *sh, int spotid, const QString &skinpath, QObject *parent) :
    SpotView(sl, NULL, spotid, "", skinpath, parent), _sh(sh)
{
    _useActionLinks = false;

    QString path = skinpath;
    if (!QFile::exists(path+"/mobilespot.tpl"))
        path = ":/tpl";

    QFile qf(path+"/mobilespot.tpl");
    qf.open(qf.ReadOnly);
    _mainTemplate = qf.readAll();
    qf.close();

    if (QFile::exists(path+"/mobilecomment.tpl"))
        qf.setFileName(path+"/mobilecomment.tpl");
    else
        qf.setFileName(":/tpl/comment.tpl");

    qf.open(qf.ReadOnly);
    _commentTemplate = qf.readAll();
    qf.close();
}

void SpotMobileView::_initWebbrowser()
{

}

void SpotMobileView::_displayHTML(const QByteArray &html, bool final)
{
    if (final)
    {
        QByteArray html2 = html;
        html2.replace("qrc:/", "");
        _sh->sendResponse("text/html", html2, QDateTime::currentDateTime() );
        deleteLater();
    }
}
