#ifndef SPOTLITE_H
#define SPOTLITE_H

#define SPOTLITE_VERSION   "SpotLite v2.1.0"

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QList>
#include <QMap>
#include <QTime>
#include <QTimer>
#include <QSettings>
#include <QSet>

class SpotNNTP;

class SpotLite : public QObject
{
    Q_OBJECT
public:
    explicit SpotLite(QString datadir, bool portable = false, QObject *parent = 0);
    virtual ~SpotLite();
    QSqlDatabase *spotDatabase();
//    QSqlDatabase *cacheDatabase();
    void downloadArticle(const QByteArray &msgid, bool topofqueue = false);
    void downloadArticle(qint64 nr);
    int numNewPosts() const;
    int numNewEPosts() const;
    void reconnect();
    bool vacuumDB();
    void emptyCommentsList();
    QList<int> getCommentMsgNrs(const QByteArray &msgid);
    QSet<int> getCommentMsgIDnrs(const QByteArray &msgid);
    bool emptyDatabases();
    bool isOnWatchlist(int spotid);
    void addToWatchlist(int spotid);
    void removeFromWatchlist(int spotid);
    QSettings *settings();
    bool inTransaction();
    void reloadFriendsAndFoes();
    bool isFriend(const QByteArray &userid) const;
    bool isFoe(const QByteArray &userid) const;
    void addFriend(const QByteArray &userid, const QByteArray &username);
    void banUser(const QByteArray &userid, const QByteArray &username, const QByteArray &reason = "");
    void exportDatabase(const QString &filename);
    bool importDatabase(const QString &filename);
    void deleteSpot(int spotid, const QString &reason = "", const QString &moderator = "", bool emitnewSpots = true);
    void deleteSpot(const QByteArray &msgid);
    void undeleteSpot(int spotid);
    void emptyTrash();
    void processNewSpots();
    SpotNNTP *nntp();
    void deleteSpam(int minid = -1, int mineid = -1);

signals:
    void articleData(const QByteArray &msgid, QByteArray data);
    void articleNotFound(const QByteArray &msgid);
    void error(int code, const QString &msg);
    void notice(int code, const QString &msg);
    void newSpots();
    void processingQueuedSpots();

public slots:
    void onArticleData(const QByteArray &msgid, QByteArray data);
    void onHeaderLine(const QByteArray &line);
    void onGroupResponse(const QByteArray &group, qint64 min, qint64 max);
    void onXrover(qint64 nr, const QByteArray &references);
    void onXroverLine(qint64 nr, const QByteArray &references);
    void onXroverSpamLine(qint64 nr, const QByteArray &references);
    void onError(int error, const QString &msg, const QByteArray &msgid);
    void onArticleNotFound(const QByteArray &msgid);
    void onLoggedIn();
    void onRefreshTimer();
    void connectToServer();
    void onFilterDone();

protected:
    QSqlDatabase _spotDatabase, _cacheDatabase, _commlistDatabase;
    QSqlQuery    _checkDuplicatesQuery, _insertQuery, _checkCommlistQuery, _echeckDuplicatesQuery, _einsertQuery, _checkSpotter, _insertSpotter;
    bool _tableExists(QSqlDatabase &db, const char *table);
    bool _indexExists(QSqlDatabase &db, const char *index);
    void _openDatabases();
    void _downloadQueue();
    void _cleanWatchlist();
    void _initDBsettings(QSqlDatabase &db);
    void _prepareInsertQueries();
    QString _decodeUtf8(const QByteArray &s);
    QByteArray _fetchFromCache(const QByteArray &id);
    void _saveWatchlist();
    void _restoreWatchlist();
    int _getSpotterID(const QByteArray &pubkey, const QByteArray &name = "");

    QString _datadir;
    QList<QByteArray> _queue;
    SpotNNTP *_nntp;
    qint64 maxOnServer, xoverSentForMax, xoverLastReceived, startid;
    QTime _lastStatusUpdate;
    QTimer _refreshHeaderTimer;
    int _newposts, _neweposts;
    bool _transaction, _commlistTransaction, _nntpSwitch, _updateKnop, _uptodate, _filterhc, _filtermanycat;

    QMap<QByteArray, QList<int> > _commlist;
    QMap<QByteArray, int> _spamcountlist;
    QSettings _settings;
    qint64 cmaxOnServer, cxroverSentForMax, cxroverLastReceived;
    qint64 smaxOnServer, sxroverSentForMax, sxroverLastReceived;
    QString _filterHook;

    QSet<QByteArray> _friends, _foes;
    QList<QByteArray> watchlist_msgids, ewatchlist_msgids;

    bool _portable;
};

#endif // SPOTLITE_H
