#ifndef SPOTVIEW_H
#define SPOTVIEW_H

#include <QObject>
#include <QTabWidget>
#include <QStringList>
#include <QList>
#include <QSet>
#include <QHash>
#include "spotlite.h"

class QWebView;
class QAxWidget;
class QUrl;
class QWidget;
class QIcon;
class QNetworkRequest;

class SpotView : public QObject
{
    Q_OBJECT
public:
    explicit SpotView(SpotLite *sl, QTabWidget *tw, int spotid, const QString &title, const QString &skinpath, QObject *parent = 0);
    void loadSpot();
    void activate();
    int tabIndex() const;
    int spotId() const;

signals:
    void createTagFilter(int cat, const QByteArray &tag);
    void notice(int nr, const QString &msg);
    void closeRequested();

public slots:
    void onArticleData(const QByteArray &msgid, QByteArray data);
    void onArticleNotFound(const QByteArray &msgid);
    void onLinkClick(const QUrl & url);
    void onWatchlistToggle(bool add);
    void onReply();
    void onReport();
    void onNewComment(const QByteArray &msg);
    void onDeleteSpot();
    void _downloadRequested(const QNetworkRequest &request);
    void _downloadFinished();
    void contextmenu(const QPoint &p);

protected:
    SpotLite *_sl;
#ifdef XXQ_OS_WIN
    QAxWidget *_web;
#else
    QWebView *_web;
#endif

    QTabWidget *_tw;
    int _spotid, _cat, _subcat, _spotdate;
    qint64 _size;
    QString _newsgroup, _title, _image, _website;
    QByteArray _mainHTML, _commentsHTML, _spotter, _tag, _spotterkey;
    QStringList _filenames;
    QStringList _cats;
    QList<int> _commentsToDownload;
    QSet<QString> _commentsHashes;
    QWidget *_widget;
    bool _useActionLinks, _requireValidSignature;
    QHash<QByteArray,QByteArray> _useridsAndNames;

    QByteArray _mainTemplate, _commentTemplate;

    static QByteArray _glMainTemplate;
    static QByteArray _glCommentTemplate;
    static QString _skinPath;
    static QIcon _watchlistIcon;
    static QIcon _replyIcon;
    static QIcon _deleteIcon;
    static QSet<QByteArray> _smileys;

    QByteArray _spotMSGID, _nzbMSGID, _imgMSGID;
    bool _parseSpot(const QByteArray &data);
    bool _decodeNZB(const QByteArray &data);
    bool _decodeIMG(const QByteArray &data);
    QByteArray _unSpecial(QByteArray s);
    QByteArray _parseComment(int msgnr, const QByteArray &data);
    void _updateHTML();
    QByteArray _toHTML(const QString &in);
    QByteArray _normalizeUsername(QByteArray username, QByteArray userid);

    virtual void _initWebbrowser();
    virtual void _displayHTML(const QByteArray &html, bool final = true);
};

#endif // SPOTVIEW_H
