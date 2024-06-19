#include "spotsmodel.h"
#include "spotlite.h"
#include <QDebug>
#include <QDateTime>
#include <QBrush>
#include <QColor>
#include <QSettings>
#include <QSqlError>
#include <QDirIterator>

/* Static stuff. Ugly workaround against "const" qualifier of data() */
const int SpotsModel::CACHE_ITEMS = 200;
SpotLite *SpotsModel::_sl = NULL;
int SpotsModel::_cachestart = -1;
int SpotsModel::_cacheend   = -1;
int SpotsModel::_cat        = -1;
int SpotsModel::_subcat     = 0;
int SpotsModel::_rows       = 0;
int SpotsModel::_oldid      = 0;
int SpotsModel::_oldeid     = 0;
int SpotsModel::_iconWidth  = 0;
int SpotsModel::_mediumSpamThreshold = 0;
int SpotsModel::_highSpamThreshold = 0;
QList<cached_spot> SpotsModel::_cache;
QSqlDatabase *SpotsModel::_db = NULL;
QByteArray SpotsModel::_query;
CustomFilter SpotsModel::_filter;
QByteArray SpotsModel::_orderBy = "id DESC";

bool SpotsModel::_useIcons = false;
QHash<int,QPixmap> SpotsModel::_icons;


SpotsModel::SpotsModel(SpotLite *sl, QObject *parent) :
    QAbstractTableModel(parent)
{
    _sl     = sl;
    _db     = sl->spotDatabase();
    _mediumSpamThreshold = sl->settings()->value("mediumspam", 5).toInt();
    _highSpamThreshold   = sl->settings()->value("highspam", 10).toInt();

    connect(sl, SIGNAL(newSpots()), this, SLOT(onNewSpots()));
    connect(sl, SIGNAL(processingQueuedSpots()), this, SLOT(resetNewcount()));

    resetNewcount();
}

void SpotsModel::resetNewcount()
{
    QSqlQuery q(*_db);
    q.exec("SELECT max(id) FROM spots");
    q.next();
    _oldid = q.value(0).toInt();
//    qDebug() << "oldid:" << _oldid;

    q.exec("SELECT max(id) FROM erospots");
    q.next();
    _oldeid = q.value(0).toInt();

    beginResetModel();
    invalidateCache();
    endResetModel();
}

QVariant SpotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Datum");
            break;

        case 1:
            return tr("Cat");
            break;

        case 2:
            return tr("Titel");
            break;

        case 3:
            return tr("Spotter");
            break;
        }
    }

    return QVariant();
}

int SpotsModel::rowCount(const QModelIndex &) const
{
    return _rows;
}

int SpotsModel::columnCount(const QModelIndex &) const
{
    return 4;
}

void SpotsModel::invalidateCache()
{
    _cache.clear();
    _rows = 0;
    _cachestart = _cacheend = -1;
}

void SpotsModel::setQuery(QByteArray query, int cat, int subcat)
{
    if (query != _query || cat != _cat || subcat != _subcat)
    {
        _query = query;
        _cat = cat;
        _subcat = subcat;
        if ( !query.isEmpty() )
            _orderBy = "spotdate DESC";
        else
            _orderBy = "id DESC";

        beginResetModel();
        invalidateCache();
        _loadFromDb(0);
        endResetModel();
    }
}

void SpotsModel::setQuery(const CustomFilter &f)
{
    _query.clear();
    _filter = f;
    _cat = -3;
    _orderBy.clear();

    beginResetModel();
    invalidateCache();
    _loadFromDb(0);
    endResetModel();
}

QString SpotsModel::subcategoryname(int subcat)
{
    switch (subcat)
    {
        case 100:
            return "DivX";
        case 101:
            return "WMV";
        case 102:
            return "MPG";
        case 103:
            return "DVD5";
        case 104:
            return "HD";
        case 105:
            return "IMG";
        case 106:
            return "BRD";
        case 107:
            return "HD";
        case 108:
            return "HD";
        case 109:
            return "x264";
        case 110:
            return "DVD9";

        case 200:
            return "MP3";
        case 201:
            return "WMA";
        case 202:
            return "WAV";
        case 203:
            return "OGG";
        case 204:
            return "LL";
        case 205:
            return "DTS";
        case 206:
            return "AAC";
        case 207:
            return "APE";
        case 208:
            return "FLAC";

        case 300:
            return "Win";
        case 301:
            return "Mac";
        case 302:
            return "Linux";
        case 303:
            return "PSX";
        case 304:
            return "PS2";
        case 305:
            return "PSP";
        case 306:
            return "XBox";
        case 307:
            return "360";
        case 308:
            return "GBA";
        case 309:
            return "GC";
        case 310:
            return "NDS";
        case 311:
            return "Wii";
        case 312:
            return "PS3";
        case 313:
            return "WP7";
        case 314:
            return "iOs";
        case 315:
            return "Android";

        case 400:
            return "Win";
        case 401:
            return "Mac";
        case 402:
            return "Linux";
        case 403:
            return "OS2";
        case 404:
            return "WP7";
        case 405:
            return "Nav";
        case 406:
            return "Ios";
        case 407:
            return "Android";

        case 900:
            return "XXX";
        case 901:
            return "Gay";
        case 902:
            return "Lesbo";
        case 903:
            return "Bi XXX";


        default:
            return "???";
    }
}

QVariant SpotsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() )
        return QVariant();


    int row = index.row();
    if (_cachestart == -1 || row < _cachestart || row > _cacheend)
    {
        if (!_loadFromDb(row))
            return QVariant();
//        qDebug() << _cachestart << _cacheend;
    }

    struct cached_spot data = _cache[row-_cachestart];

    if (role == Qt::DisplayRole)
    {
//        qDebug() << "row" << row << "cachestart" << _cachestart << "count" << _cache.count();

        switch (index.column() )
        {
        case 0:
            {
                QDateTime t;
                t.setTime_t(data.spotdate);
                return t.toString("dd-MM-yy (hh:mm)");
            }
        case 1:
            if ( _useIcons && _icons.contains(data.subcat) )
                return "";
            else
                return subcategoryname(data.subcat);

        case 2:
            return data.subject;

        case 3:
            return data.poster;
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (data.id < 0)
        {
            if (-data.id > _oldeid)
                return QBrush( QColor(0,100,0) );
        }
        else if (_cat == 9 || _subcat > 899 || _cat < -8)
        {
            if (data.id > _oldeid)
                return QBrush( QColor(0,100,0) );
        }
        else if ( data.id > _oldid ) /* nieuwe spot */
            return QBrush( QColor(0,100,0) );
    }
    else if (role == Qt::BackgroundColorRole)
    {
        if (_highSpamThreshold && data.spamcount > _highSpamThreshold)
            return QBrush( QColor(0xFF,0xBB, 0xBB) );
        else if (_mediumSpamThreshold && data.spamcount > _mediumSpamThreshold)
            return QBrush( QColor(0xF4, 0xDC, 0xFE) );
        else if (data.id < 0)
        {
            if (-data.id > _oldeid)
                return QBrush( QColor(0xEF,0xFF,0xEF) );
        }
        else if (_cat == 9 || _subcat > 899 || _cat < -8 )
        {
            if (data.id > _oldeid)
                return QBrush( QColor(0xEF,0xFF,0xEF) );
        }
        else if ( data.id > _oldid )
            return QBrush( QColor(0xEF,0xFF,0xEF) );
    }
    else if (role == Qt::UserRole)
    {
        if (_cat == 9 || _subcat > 899 || _cat < -8)
            return 0-data.id;
        else
            return data.id;
    }
    else if (role == Qt::DecorationRole)
    {
        if ( _useIcons && index.column() == 1 && _icons.contains(data.subcat) )
            return _icons.value(data.subcat);
    }

    return QVariant();
}

bool SpotsModel::_loadFromDb(int neededRow)
{
    qDebug() << "Raadplegen database, row:" << neededRow;
    QTime timer;
    timer.start();

    int lastid = 0;
    if (neededRow && neededRow == _cacheend+1)
    {
        lastid = _cache.last().id;
    }

    _cache.clear();
    _cachestart = qMax(0, neededRow-CACHE_ITEMS/2);
    _cacheend   = _cachestart-1;

    QSqlQuery q(*_db);
    QByteArray table, customquery;
    bool e = false;

    if (_cat == 9 || _subcat > 899)
        table = "erospots";
    else
        table = "spots";

    if ( !_query.isEmpty() )
    {
        e = _sl->settings()->value("ero").toBool();
        if (e)
        {
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM spots WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%'"
                      " UNION SELECT (0-id) AS id,subcat,spotdate,title,spotter,spamcount FROM erospots WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%'"
                      " ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        }
        else
        {
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%' ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        }
        for (int i=0; i< (e ? 6 : 3); i++)
            q.addBindValue(QString(_query));

        lastid = 0;
    }
    else if (_cat == -1)
    {
        if (!_rows)
        {
            _rows = _sl->numNewPosts();
        }

        if (lastid)
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE id>? AND id<? ORDER BY "+_orderBy+" LIMIT ?");
        else
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE id>? ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        q.addBindValue(_oldid);
    }
    else if (_cat == 0 && _subcat == 0)
    {
        if (!_rows)
        {
            q.exec("SELECT count(*) FROM spots");
            q.next();
            _rows = q.value(0).toInt();
        }

        if (lastid)
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE id<? ORDER BY "+_orderBy+" LIMIT ?");
        else
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");

    }
    else if (_cat == -9)
    {
        if (!_rows)
        {
            _rows = _sl->numNewEPosts();
        }

        if (lastid)
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM erospots WHERE id>? AND id<? ORDER BY "+_orderBy+" LIMIT ?");
        else
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM erospots WHERE id>? ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        q.addBindValue(_oldeid);
    }
    else if (_cat == -2) /* Watchlist */
    {
        QByteArray wl = _sl->settings()->value("watchlist").toByteArray();
        if (wl.isEmpty() )
        {
            _rows = 0;
            return false;
        }
        _rows = wl.split(',').size();
        lastid = 0;
        q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM spots WHERE id IN ("+wl+") ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
    }
    else if (_cat == -902) /* eWatchlist */
    {
        QByteArray wl = _sl->settings()->value("ewatchlist").toByteArray();
        if (wl.isEmpty() )
        {
            _rows = 0;
            return false;
        }
        _rows = wl.split(',').size();
        lastid = 0;
        q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM erospots WHERE id IN ("+wl+") ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
    }
    else if (_cat == -3) /* Custom filter */
    {
        customquery = _filter.query("", _orderBy);
        if ( _filter.cat() == 9 )
        {
            table = "erospots";
            q.prepare("SELECT 0-id,subcat,spotdate,title,spotter,spamcount FROM erospots WHERE "+customquery+" LIMIT ? OFFSET ?");
        }
        else
        {
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM spots WHERE "+customquery+" LIMIT ? OFFSET ?");
        }
        lastid = 0;
    }
    else if (_cat == -4) /* Prullenbak */
    {
        if (!_rows)
        {
            q.exec("SELECT count(*) FROM trash_spots");
            q.next();
            _rows = q.value(0).toInt();
        }

        table = "trash_spots";
        if (lastid)
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE id<? ORDER BY "+_orderBy+" LIMIT ?");
        else
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
    }
    else if (_subcat)
    {
        if (!_rows)
        {
            q.prepare("SELECT count(*) FROM "+table+" WHERE subcat=?");
            q.addBindValue(_subcat);
            q.exec();
            q.next();
            _rows = q.value(0).toInt();
        }

        if (lastid)
            lastid = 0;
            //q.prepare("SELECT id,subcat,spotdate,title,spotter FROM "+table+" WHERE subcat=? AND id<? ORDER BY id DESC LIMIT ?");
        else
            q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE subcat=? ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        q.addBindValue(_subcat);
    }
    else if (_cat)
    {
        if (!_rows)
        {
            q.prepare("SELECT count(*) FROM "+table+" WHERE cat=?");
            q.addBindValue(_cat);
            q.exec();
            q.next();
            _rows = q.value(0).toInt();
        }

        if (lastid)
            lastid = 0;
        q.prepare("SELECT id,subcat,spotdate,title,spotter,spamcount FROM "+table+" WHERE cat=? ORDER BY "+_orderBy+" LIMIT ? OFFSET ?");
        q.addBindValue(_cat);
    }
    else
        return false;

    if (lastid)
    {
        q.addBindValue(lastid);
        q.addBindValue(CACHE_ITEMS);
    }
    else
    {
        q.addBindValue(CACHE_ITEMS);
        q.addBindValue(_cachestart);
    }

    bool trans = false;
    if ( !_sl->inTransaction() )
    {
        trans = true;
        _db->transaction();
    }

    if (!q.exec() )
    {
        qDebug() << "Fout bij raadplegen database";
        if (trans)
            _db->rollback();
        return false;
    }

    qDebug() << "Query voltooid in" << (timer.elapsed() / 1000.0) << "secs";

    while ( q.next() )
    {
        struct cached_spot c = {
            q.value(0).toInt(), q.value(1).toInt(), q.value(2).toInt(), q.value(5).toInt(), q.value(3).toByteArray(), q.value(4).toByteArray()
        };
        _cache.append(c);
        _cacheend++;
    }
    qDebug() << "Resultaten opgehaald in" << (timer.elapsed() / 1000.0) << "secs";

    if (!_query.isEmpty() && !_rows)
    {
        _rows = _cacheend+1;

        if (_rows == CACHE_ITEMS)
        {
            if (e)
            {
                q.prepare("SELECT sum(t) FROM (SELECT count(*) AS t FROM spots WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%'"
                          " UNION SELECT count(*) AS t FROM erospots WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%'"
                          ")");
            }
            else
            {
                q.prepare("SELECT count(*) FROM "+table+" WHERE +title LIKE '%'||?||'%' OR +spotter LIKE '%'||?||'%' OR +tag LIKE '%'||?||'%'");
            }
            for (int i=0; i< (e ? 6 : 3); i++)
            {
                q.addBindValue(QString(_query));
            }
            q.exec();
            q.next();
            _rows = q.value(0).toInt();

            qDebug() << "Totaal opgehaald in" << (timer.elapsed() / 1000.0) << "secs";
        }
    }
    else if (_cat == -3 && !_rows) /* Custom filter */
    {
        _rows = _cacheend+1;

        if (_rows == CACHE_ITEMS)
        {
            q.prepare("SELECT count(*) FROM "+table+" WHERE "+customquery.left(customquery.lastIndexOf(" ORDER BY")) );
            q.exec();
            q.next();
            _rows = q.value(0).toInt();
            qDebug() << "Totaal opgehaald in" << (timer.elapsed() / 1000.0) << "secs";
        }
    }

    if (trans)
        _db->commit();
//        _db->rollback();

    return (neededRow >= _cachestart && neededRow <= _cacheend);
}

void SpotsModel::onNewSpots()
{
//    beginResetModel();
    int oldrows = _rows;
    invalidateCache();
    _loadFromDb(0);
//    endResetModel();

    if (_cat == -2 || _rows < oldrows)
    {
        beginResetModel();
        endResetModel();
    }
    else if (_rows > oldrows)
    {
        int newrows = _rows;
        int numnewrows = _rows-oldrows;
        _rows = oldrows;
        beginInsertRows(QModelIndex(), 0, numnewrows-1);
        _rows = newrows;
        endInsertRows();
    }
}

void SpotsModel::changeTheme(const QString &themedir)
{
    _icons.clear();
    _iconWidth = 0;

    QDirIterator iter(themedir+"/overzicht", QStringList("*.png"));
    while (iter.hasNext())
    {
        iter.next();
        int nr = iter.fileName().left(iter.fileName().length()-4).toInt();
        if (nr)
        {
            QPixmap icon = QPixmap( iter.filePath() );
            _icons.insert(nr, icon);
            _iconWidth = qMax(_iconWidth, icon.width() );
        }
    }

    _useIcons = !_icons.isEmpty();
}

int SpotsModel::iconWidth() const
{
    return _iconWidth;
}

void SpotsModel::sort(int column, Qt::SortOrder order)
{
    qDebug() << " sort " << column;

    switch (column)
    {
    case 0:
        _orderBy = "spotdate";
        break;

    case 1:
        _orderBy = "subcat";
        break;

    case 2:
        _orderBy = "title";
        break;

    case 3:
        _orderBy = "spotter";
        break;

    default:
        _orderBy = "id";
        order = Qt::DescendingOrder;
    }

    _orderBy += (order == Qt::AscendingOrder ? "" : " DESC");

    beginResetModel();
    invalidateCache();
    _loadFromDb(0);
    endResetModel();
}
