#ifndef SPOTIMAGELIST_H
#define SPOTIMAGELIST_H

#include <QWidget>
#include "spotlite.h"


class QWebView;
class QUrl;
class QWidget;
class QNetworkRequest;
class QAbstractTableModel;
class QToolButton;

class SpotImageList;
class _SpotImageListItem : public QObject
{
    Q_OBJECT
public:
    _SpotImageListItem(SpotImageList *parent, SpotLite *sl, int spotid);
    QString title();
    QByteArray imageurl();
    int id();

protected:
    SpotImageList *_parent;
    SpotLite *_sl;
    int _spotid;
    QString    _title;
    QByteArray _msgid, _imagedata;
    QList<QByteArray> _imagemsgids;

    QByteArray _decodeIMG(const QByteArray &data);
    bool _parseSpot(const QByteArray &data);
    QByteArray _unSpecial(QByteArray s);

protected slots:
    void onArticleData(const QByteArray &msgid, QByteArray data);
    void onArticleNotFound(const QByteArray &msgid);
};


class SpotImageList : public QWidget
{
    Q_OBJECT
public:
    explicit SpotImageList(QWidget *parent = 0);
    void setSpotLite(SpotLite *sl);
    void setModel(QAbstractTableModel *model);
    void setIndex(int idx);
    void updateHTML();
    void refresh();

signals:
    void spotOpened(int id, const QString &title);

public slots:

    void onLinkClick(const QUrl & url);
    void previousPage();
    void nextPage();
    void _downloadRequested(const QNetworkRequest &request);
    void _downloadFinished();

protected:
    int _index, _total, _pagesize;
    SpotLite *_sl;
    QAbstractTableModel *_model;
    QWebView *_web;
    QToolButton *_prev, *_next;
    bool _suspendUpdate;

    QByteArray _mainTemplate, _itemTemplate;
    QByteArray _toHTML(const QString &in);
    QList<_SpotImageListItem *> _itemlist;


};

#endif // SPOTIMAGELIST_H
