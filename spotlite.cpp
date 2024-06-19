#include "spotlite.h"
#include "spotnntp.h"
#include "spotsignature.h"
#include "customfilter.h"
#include "cachewarmer.h"
#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QVariant>
#include <QSettings>
#include <QDateTime>
#include <QApplication>
#include <QBuffer>
#include <QDataStream>
#include <QSqlError>
#include <QProcess>
#include "qtioprocessor/qtiocompressor.h"

#include <stdexcept>

#define MAX_PER_CYCLE           50000
#define MAX_PER_CYCLE_XHDR      (MAX_PER_CYCLE*4)
#define HEADER_REFRESH_INTERVAL 119000

SpotLite::SpotLite(QString datadir, bool portable, QObject *parent) :
    QObject(parent), _datadir(datadir), _nntp(NULL), startid(0), _newposts(0), _neweposts(0), _transaction(false), _commlistTransaction(false), _nntpSwitch(false), _updateKnop(false), _uptodate(false),
    _settings(datadir+"/SpotLite.conf", QSettings::IniFormat), _portable(portable)
{
    qsrand( QDateTime::currentDateTime().toTime_t() );
    qRegisterMetaTypeStreamOperators<CustomFilter>("CustomFilter");
    _openDatabases();
    qDebug() << "Watchlist opschonen";
    _cleanWatchlist();
    _refreshHeaderTimer.setSingleShot(true);
    connect(&_refreshHeaderTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimer()) );
    reloadFriendsAndFoes();
    _filterHook = _settings.value("filterhook").toString();
    qDebug() << "Spotlite init klaar";
}

void SpotLite::_openDatabases()
{
    QDir dir;
    QString dirname = /*QDesktopServices::storageLocation(QDesktopServices::DataLocation)*/ _datadir;
    qDebug() << "Database map:" << dirname;

    if ( !dir.exists(dirname) )
    {
        if ( !dir.mkpath(dirname) )
        {
            throw std::runtime_error("Fout bij aanmaken database map");
        }
    }

    if ( _settings.allKeys().isEmpty() )
    {
        QSettings oldsettings;
        QStringList keys = oldsettings.allKeys();
        if ( !_portable && !keys.isEmpty() )
        {
            foreach (QString key, keys)
            {
                _settings.setValue(key, oldsettings.value(key));
            }
            _settings.sync();
            if ( _settings.status() == _settings.NoError )
            {
                foreach (QString key, keys)
                {
                    oldsettings.remove(key);
                }
                oldsettings.remove("");
                oldsettings.sync();
            }
        }

        // Changeme
        _settings.setValue("skin", "RoyaleBlueFull");
    }

    if ( !_settings.value("coldcache").toBool() )
    {
        CacheWarmer *cw = new CacheWarmer(this);
        connect(cw, SIGNAL(finished()), cw, SLOT(deleteLater()));

        cw->addFile(dirname+"/reactielijst.db");
        cw->addFile(dirname+"/spots.db");
        cw->start();
    }

    _spotDatabase  = QSqlDatabase::addDatabase("QSQLITE", "spots");
    _spotDatabase.setDatabaseName(dirname+"/spots.db");
    if ( ! _spotDatabase.open() )
    {
        throw std::runtime_error("Fout bij openen spots database");
    }
    _initDBsettings(_spotDatabase);

    _cacheDatabase = QSqlDatabase::addDatabase("QSQLITE", "cache");
    _cacheDatabase.setDatabaseName(dirname+"/cache.db");
    if ( !_cacheDatabase.open() )
    {
        throw std::runtime_error("Fout bij openen cache database");
    }
    _initDBsettings(_cacheDatabase);

    _commlistDatabase = QSqlDatabase::addDatabase("QSQLITE", "commentlist");
    _commlistDatabase.setDatabaseName(dirname+"/reactielijst.db");
    if ( !_commlistDatabase.open() )
    {
        throw std::runtime_error("Fout bij openen cache database");
    }
    _initDBsettings(_commlistDatabase);

    if ( !_tableExists(_spotDatabase, "spots") )
    {

        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE spots(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "cat INT,"
                    "subcat INT,"
                    "spotdate INT,"
                    "title VARCHAR(255),"
                    "spotter VARCHAR(255),"
                    "tag VARCHAR(255),"
                    "othercats VARCHAR(255),"
                    "msgid VARCHAR(255) )"
               );

        q.exec("CREATE UNIQUE INDEX catidx ON spots(cat,id)");
        q.exec("CREATE UNIQUE INDEX subcatidx ON spots(subcat,id)");
        q.exec("CREATE INDEX tagidx ON spots(tag)");
        q.exec("CREATE INDEX dateidx ON spots(spotdate)");
    }
    if ( !_tableExists(_spotDatabase, "erospots") )
    {
        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE erospots(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "cat INT,"
                    "subcat INT,"
                    "spotdate INT,"
                    "title VARCHAR(255),"
                    "spotter VARCHAR(255),"
                    "tag VARCHAR(255),"
                    "othercats VARCHAR(255),"
                    "msgid VARCHAR(255) )"
               );
        q.exec("CREATE UNIQUE INDEX ecatidx ON erospots(cat,id)");
        q.exec("CREATE UNIQUE INDEX esubcatidx ON erospots(subcat,id)");
        q.exec("CREATE INDEX etagidx ON erospots(tag)");
        q.exec("CREATE INDEX edateidx ON erospots(spotdate)");

        /* Upgrade from older versions */
        q.exec("ALTER TABLE spots ADD COLUMN othercats VARCHAR(255)");

        q.exec("INSERT INTO erospots(id,cat,subcat,spotdate,title,spotter,tag,msgid) SELECT id,cat,subcat,spotdate,title,spotter,tag,msgid FROM spots WHERE cat=9 ORDER BY id");
        q.exec("DELETE FROM spots WHERE cat=9");
    }

    if ( !_tableExists(_spotDatabase, "trash_spots") )
    {

        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE trash_spots(id INTEGER PRIMARY KEY,"
                    "cat INT,"
                    "subcat INT,"
                    "spotdate INT,"
                    "title VARCHAR(255),"
                    "spotter VARCHAR(255),"
                    "tag VARCHAR(255),"
                    "othercats VARCHAR(255),"
                    "msgid VARCHAR(255),"
                    "moderator VARCHAR(255),"
                    "updated INT,"
                    "reason VARCHAR(255) )"
               );
    }

    /*
        q.exec("CREATE TABLE mod_spotted(teamid INT,"
                    "modid INT,"
                    "teamname VARCHAR(255),"
                    "modname VARCHAR(255),"
                    "firstactiontime INT,"
                    "actioncount INT,"
                    "lastmsgid VARCHAR(255),"
                    "PRIMARY KEY(teamid,modid)"
                );

        q.exec("CREATE TABLE mod_trusted_teams(teamid INT PRIMARY KEY,"
                    "teamname VARCHAR(255),"
                    "pubkey VARCHAR(255),"
                    "teamleader VARCHAR(255)"
                );
        q.exec("CREATE TABLE mod_revokelist(teamid INT, modid INT, pubkey VARCHAR(255), starttime INT");
        q.exec("CREATE TABLE mod_trusted_mods(teamid INT,"
               "modid INT,"
               "modname VARCHAR(255),"
               "lastactionid INT",
               "PRIMARY KEY(teamid,modid)"
               );
    }
    */


    if ( !_tableExists(_spotDatabase, "friends") )
    {
        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE friends(userid VARCHAR(255) PRIMARY KEY, username VARCHAR(255))");
    }
    if ( !_tableExists(_spotDatabase, "foes") )
    {
        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE foes(userid VARCHAR(255) PRIMARY KEY, username VARCHAR(255), reason VARCHAR(255))");
    }

    if ( !_tableExists(_cacheDatabase, "cache") )
    {

        QSqlQuery q(_cacheDatabase);
        q.exec("CREATE TABLE cache(id VARCHAR(255) PRIMARY KEY,"
                    "dateadded INT,"
                    "data BLOB )"
               );
        q.exec("CREATE INDEX dateidx ON cache(dateadded)");
    }
    if ( !_tableExists(_commlistDatabase, "msgnrs") )
    {
        QSqlQuery q(_commlistDatabase);
        q.exec("CREATE TABLE msgnrs(post VARCHAR(255) PRIMARY KEY,"
               "nrs BLOB)");
    }

    if ( !_tableExists(_spotDatabase, "spotters") )
    {
        QSqlQuery q(_spotDatabase);
        q.exec("CREATE TABLE spotters(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "pubkey VARCHAR(255) NOT NULL UNIQUE,"
                    "name VARCHAR(255))"
                );
        q.exec("ALTER TABLE spots ADD COLUMN spotterid INT DEFAULT 0");
        q.exec("ALTER TABLE erospots ADD COLUMN spotterid INT DEFAULT 0");
        q.exec("ALTER TABLE trash_spots ADD COLUMN spotterid INT DEFAULT 0");
        if ( _tableExists(_spotDatabase, "queued_spots") )
            q.exec("ALTER TABLE queued_spots ADD COLUMN spotterid INT DEFAULT 0");
    }
    if ( !_tableExists(_spotDatabase, "queued_spots") )
    {
        if ( _settings.value("ver", 0).toInt() )
        {
            QMessageBox::information(NULL, tr("Even bezig"), tr("Het upgraden van de database kan enkele minuten duren..."), QMessageBox::Ok);
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }

        QSqlQuery q(_spotDatabase);
        q.exec("ALTER TABLE spots ADD COLUMN spamcount INT DEFAULT 0");
        q.exec("ALTER TABLE erospots ADD COLUMN spamcount INT DEFAULT 0");
        q.exec("ALTER TABLE trash_spots ADD COLUMN spamcount INT DEFAULT 0");

        qDebug() << "vacuum";
        vacuumDB();
        qDebug() << "vacuum klaar";

        q.exec("CREATE UNIQUE INDEX mididx ON spots(msgid)");
        q.exec("CREATE UNIQUE INDEX emididx ON erospots(msgid)");
        q.exec("CREATE TABLE queued_spots(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "cat INT,"
                    "subcat INT,"
                    "spotdate INT,"
                    "title VARCHAR(255),"
                    "spotter VARCHAR(255),"
                    "tag VARCHAR(255),"
                    "othercats VARCHAR(255),"
                    "msgid VARCHAR(255),"
                    "spamcount INT DEFAULT 0,"
                    "spotterid INT DEFAULT 0)"
               );
        q.exec("CREATE UNIQUE INDEX qmididx ON queued_spots(msgid)");


        if ( _settings.value("ver", 0).toInt() )
        {
            QApplication::restoreOverrideCursor();
        }
    }

    if ( !_indexExists(_spotDatabase, "mididx") )
    {
        qDebug() << "mididx mist, opnieuw aanmaken";
        QSqlQuery q(_spotDatabase);
        q.exec("CREATE UNIQUE INDEX mididx ON spots(msgid)");
        q.exec("CREATE UNIQUE INDEX emididx ON erospots(msgid)");
        q.exec("CREATE UNIQUE INDEX qmididx ON queued_spots(msgid)");
    }

    if ( _settings.value("ver", 0).toInt() < 8 )
    {
        QSqlQuery q(_spotDatabase);
        q.exec("DELETE from spots where length(othercats)>30");

        _settings.setValue("ver", 8);
        _settings.setValue("nntpImages", true);
    }

    _checkDuplicatesQuery = QSqlQuery(_spotDatabase);
    _checkDuplicatesQuery.prepare("SELECT cat FROM spots WHERE spotdate=? AND title=?");
    //_checkDuplicatesQuery.prepare("SELECT cat FROM spots WHERE spotdate=? AND msgid=?");


    _echeckDuplicatesQuery = QSqlQuery(_spotDatabase);
    _echeckDuplicatesQuery.prepare("SELECT cat FROM erospots WHERE spotdate=? AND title=?");
    //_echeckDuplicatesQuery.prepare("SELECT cat FROM erospots WHERE spotdate=? AND msgid=?");

    _prepareInsertQueries();
    _checkCommlistQuery = QSqlQuery(_commlistDatabase);
    _checkCommlistQuery.prepare("SELECT nrs FROM msgnrs WHERE post=?");

    _checkSpotter = QSqlQuery(_spotDatabase);
    _checkSpotter.prepare("SELECT id FROM spotters WHERE pubkey=?");

    _insertSpotter = QSqlQuery(_spotDatabase);
    _insertSpotter.prepare("INSERT INTO spotters(pubkey,name) VALUES(?,?)");
}

void SpotLite::_prepareInsertQueries()
{
    _insertQuery = QSqlQuery(_spotDatabase);
    _einsertQuery = QSqlQuery(_spotDatabase);

    _updateKnop = _settings.value("updateknop").toBool();
    if ( _updateKnop )
    {
        _insertQuery.prepare("INSERT INTO queued_spots(cat,subcat,spotdate,title,spotter,tag,msgid,othercats,spotterid) VALUES (?,?,?,?,?,?,?,?,?)");
        _einsertQuery.prepare("INSERT INTO queued_spots(cat,subcat,spotdate,title,spotter,tag,msgid,othercats,spotterid) VALUES (?,?,?,?,?,?,?,?,?)");
    }
    else
    {
        _insertQuery.prepare("INSERT INTO spots(cat,subcat,spotdate,title,spotter,tag,msgid,othercats,spotterid) VALUES (?,?,?,?,?,?,?,?,?)");
        _einsertQuery.prepare("INSERT INTO erospots(cat,subcat,spotdate,title,spotter,tag,msgid,othercats,spotterid) VALUES (?,?,?,?,?,?,?,?,?)");
    }
}

SpotLite::~SpotLite()
{
    if (_nntp)
        _nntp->logout_and_delete();

    _spotDatabase.close();
    _cacheDatabase.close();
    _commlistDatabase.close();
}

QSqlDatabase *SpotLite::spotDatabase()
{
    return &_spotDatabase;
}

/*QSqlDatabase *SpotLite::cacheDatabase()
{
    return &_cacheDatabase;
}*/

bool SpotLite::_tableExists(QSqlDatabase &db, const char *table)
{
    QSqlQuery q(db);
    q.prepare("SELECT name FROM sqlite_master WHERE name=?");
    q.addBindValue(table);
    q.exec();

    return q.next();
}

bool SpotLite::_indexExists(QSqlDatabase &db, const char *index)
{
    return _tableExists(db, index);
}

void SpotLite::connectToServer()
{
    _filterhc      = _settings.value("filterhc", true).toBool();
    _filtermanycat = _settings.value("filtermanycat", true).toBool();

    if (!_nntp)
    {
        if ( _settings.value("port").isValid() )
        {
            QString server  = _settings.value("server").toString();
            QByteArray user = _settings.value("user").toByteArray();
            QByteArray pass = QByteArray::fromBase64(_settings.value("pass").toByteArray() );
            int port        = _settings.value("port").toInt();
            bool ssl        = _settings.value("ssl").toBool();
            bool xzver      = _settings.value("xzver").toBool();
            _nntpSwitch     = _settings.value("nntpswitch").toBool();

            _nntp = new SpotNNTP(server, port, user, pass, ssl, xzver, this);
            connect(_nntp, SIGNAL(articleData(QByteArray,QByteArray)), this, SLOT(onArticleData(QByteArray,QByteArray)));
            connect(_nntp, SIGNAL(articleNotFound(QByteArray)), this, SLOT(onArticleNotFound(QByteArray)));
            connect(_nntp, SIGNAL(error(int,QString,QByteArray)), this, SLOT(onError(int,QString,QByteArray)));
            connect(_nntp, SIGNAL(groupResponse(QByteArray,qint64,qint64)), this, SLOT(onGroupResponse(QByteArray,qint64,qint64)));
            connect(_nntp, SIGNAL(headerLine(QByteArray)), this, SLOT(onHeaderLine(QByteArray)));
            connect(_nntp, SIGNAL(xroverLine(qint64,QByteArray)), this, SLOT(onXrover(qint64,QByteArray)));
            connect(_nntp, SIGNAL(loggedIn()), this, SLOT(onLoggedIn()));
            qDebug() << "Verbinden...";
            _nntp->login();
        }
    }
}

QByteArray SpotLite::_fetchFromCache(const QByteArray &msgid)
{
    QSqlQuery q(_cacheDatabase);
    q.prepare("SELECT data FROM cache WHERE id=?");
    q.addBindValue(QString(msgid));
    q.exec();

    if ( q.next() )
    {
        QByteArray cdata = q.value(0).toByteArray();

        QBuffer b(&cdata);
        QtIOCompressor io(&b);
        io.open(io.ReadOnly);
        QByteArray data = io.readAll();
        io.close();
        b.close();

        return data;
    }
    else
        return QByteArray();
}

void SpotLite::downloadArticle(const QByteArray &msgid, bool topofqueue)
{
    QByteArray data = _fetchFromCache(msgid);

    if ( !data.isEmpty() )
    {
        emit articleData(msgid, data);
    }
    else
    {
        if (!_nntp)
            connectToServer();

        if ( !_queue.contains(msgid) && (_nntp && _nntp->currentMSGid() != msgid) )
        {
            if (topofqueue)
                _queue.prepend(msgid);
            else
                _queue.append(msgid);
            _downloadQueue();
        }
    }
}

void SpotLite::downloadArticle(qint64 nr)
{
    downloadArticle(QByteArray::number(nr));
}

void SpotLite::onLoggedIn()
{
    if ( _queue.empty() && !_refreshHeaderTimer.isActive() )
    {
//        _refreshHeaderTimer.stop();
        _nntp->group("free.pt");
    }
    else
    {
        _downloadQueue();
    }
}

void SpotLite::_downloadQueue()
{
    if ( !_queue.isEmpty() && _nntp && _nntp->idle() )
    {
        QByteArray msgid = _queue.takeFirst();

        if (!msgid.contains("@") && _nntp->currentGroup() != "free.usenet")
        {
            _queue.prepend(msgid);
            _nntp->group("free.usenet");
        }
        else if (_nntpSwitch && msgid.startsWith(">") && _nntp->currentGroup() != "alt.binaries")
        {
            _queue.prepend(msgid);
            _nntp->group("alt.binaries");
        }
        else
            _nntp->download(msgid);
    }
}

void SpotLite::onArticleData(const QByteArray &msgid, QByteArray data)
{
    QSqlQuery q(_cacheDatabase);
    q.prepare("INSERT INTO cache(id,dateadded,data) VALUES (?,?,?)");

    QBuffer b;
    QtIOCompressor io(&b);
    io.open(io.WriteOnly);
    io.write(data);
    io.close();

    q.addBindValue(QString(msgid));
    q.addBindValue( QDateTime::currentDateTime().toTime_t() );
    q.addBindValue( /*data*/ b.buffer() );
    q.exec();

    emit articleData(msgid, data);
    onLoggedIn();
}

void SpotLite::onHeaderLine(const QByteArray &line)
{
    //qDebug() << "HR:" << line;

    if (line == ".\r\n")
    {
        qDebug() << "S: [einde van XOVER data]";

        int maxdays = _settings.value("maxdays").toInt();

        if (maxdays && maxdays < 9999)
        {
            int d = QDateTime::currentDateTime().toTime_t();
            QSqlQuery q(_spotDatabase);
            q.prepare("DELETE FROM spots WHERE spotdate<?");
            q.addBindValue( d-maxdays*86400 );
            q.exec();
            q.prepare("DELETE FROM erospots WHERE spotdate<?");
            q.addBindValue( d-maxdays*86400 );
            q.exec();
        }

        _spotDatabase.commit();
        _transaction = false;

        if (xoverLastReceived)
        {
            _settings.setValue("lastheader", xoverLastReceived);
            _settings.sync();

            if (!_filterHook.isEmpty() )
            {
                if (xoverSentForMax == maxOnServer)
                {
                    QProcess *proc = new QProcess(this);
                    connect(proc, SIGNAL(finished(int)), this, SLOT(onFilterDone()) );
                    connect(proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onFilterDone()) );
                    qDebug() << "Uitvoeren:" << _filterHook;
                    proc->start(_filterHook);
                }
            }
            else
                emit newSpots();
            QApplication::processEvents();
        }
        else if ( _settings.value("lastheader").toInt() == 0 )
        {
            _nntp->logout_and_delete();
            _nntp = NULL;
            emit error(0, tr("Je Usenet provider ondersteund het downloaden van headers niet!"));
            return;
        }

        if ( _queue.isEmpty() )
        {
            if (xoverSentForMax < maxOnServer) /* more on server */
            {
                qint64 min = xoverSentForMax + 1;
                xoverSentForMax = qMin(maxOnServer, min+MAX_PER_CYCLE);
                xoverLastReceived = 0;

                _transaction = true; _spotDatabase.transaction();
                emit notice(0, tr("Bezig met ophalen van nog meer headers"));
                _nntp->xover(min, xoverSentForMax);
            }
            else
            {
                // all done
                QSqlQuery q(_spotDatabase);
                q.exec("SELECT count(*) FROM spots");
                q.next();
                int total = q.value(0).toInt();

                q.exec("SELECT count(*) FROM erospots");
                q.next();
                total += q.value(0).toInt();

                emit notice(0, QString("Klaar met headeren... %1 nieuwe spots, %2 totaal.").arg(QString::number(_newposts+_neweposts), QString::number(total) ) );

                //_refreshHeaderTimer.start(119000);

                if ( _settings.value("spamreports", true).toBool() )
                    _nntp->group("free.willey");
                else
                    _nntp->group("free.usenet");
            }
        }
        else
            onLoggedIn();
    }
    else
    {
        QList<QByteArray> l = line.split('\t');
        if (l.count() < 8)
        {
            qDebug() << "Foutief XOVER formaat";
            return;
        }

        xoverLastReceived = l[0].toLongLong();

        if ( xoverLastReceived % 1000 == 0 && _lastStatusUpdate.elapsed() > 25 )
        {
            int xtot = maxOnServer-startid;
            if (xtot == 0)
                xtot = 1;
            emit notice(0, QString("Headeren %1 van %2 (%3% voltooid)").arg( QString::number(xoverLastReceived), QString::number(maxOnServer), QString::number( (int) 100.0*(xoverLastReceived-startid)/xtot)) );
            _lastStatusUpdate.restart();
//            QApplication::processEvents();
        }

        if (l[1].contains("?=") && l[1].contains("=?"))
            l[1] = _decodeUtf8(l[1].trimmed().replace("?= =?", "?==?")).toLatin1();
        QList<QByteArray> subj_tag = l[1].split('|');
        if (subj_tag[0].startsWith("X5O!P%@"))
            return;

        int p = l[2].indexOf(" <");
        if (p == -1)
            return;

        QList<QByteArray> emailparts = l[2].mid(p+2).split('@');
        if (emailparts.count() < 2)
            return;

        QList<QByteArray> sninfo = emailparts[1].split('.');
        if (sninfo.count() < 6 || sninfo[0].length() < 4 )
            return;

        int cat    = sninfo[0].left(1).toInt();
        int key    = sninfo[0].mid(1,1).toInt();
        int subcat = 0;
        int spotterid = 0;
        QByteArray othercats, modulus;
        QByteArray spotter = l[2].left(p).trimmed();

        if (emailparts[0].contains("."))
        {
            modulus = emailparts[0].split('.').first().replace("-s", "/").replace("-p", "+");
            if (isFoe(modulus))
                return; /* gebanned */

            spotterid = _getSpotterID(modulus, spotter);
        }

        if (key == 1)
        {
            //subcat    = sninfo[0].mid(2,2).toInt();
            //othercats = sninfo[0].mid(4);

            QByteArray tmp;
            for (int i=2; i< sninfo[0].length(); i++)
            {
                if (!tmp.isEmpty() && isalpha(sninfo[0][i]) )
                {
                    if (tmp.length() == 2)
                        othercats += tmp.left(1)+"0"+tmp.right(1);
                    else if (tmp.length() == 3)
                        othercats += tmp;

                    tmp = "";
                }

                tmp += sninfo[0][i];
            }

            if (tmp.length() == 2)
                othercats += tmp.left(1)+"0"+tmp.right(1);
            else if (tmp.length() == 3)
                othercats += tmp;

            if ( !othercats.isEmpty() )
            {
                subcat = othercats.mid(1,2).toInt();
                if (othercats.length() > 3)
                    othercats = othercats.mid(3);
                else
                    othercats = "";
            }
        }
        else if (key == 2)
        {
            qDebug() << "keyid=2 overgeslagen" << line;
            return;
        }
        else
        {
            subcat    = sninfo[0].mid(3,2).toInt();
            othercats = sninfo[0].mid(5);
        }
        subcat += (cat*100);

        if (cat == 1)
        {
            const QByteArray c = sninfo[0];

            if (c.contains("d23") || c.contains("d75") ) /* XXX */
            {
                cat = 9; subcat = 900;
            }
            if (c.contains("d24") || c.contains("d74") )
            {
                cat = 9; subcat = 901;
            }
            if (c.contains("d25") || c.contains("d73") )
            {
                cat = 9; subcat = 902;
            }
            if (c.contains("d26") || c.contains("d72") )
            {
                cat = 9; subcat = 903;
            }
            if (c.contains("z03") && cat != 9)
            {
                cat = 9; subcat = 900;
            }
        }

        bool ero = (cat == 9);
        int spotdate = sninfo[3].toInt();
        QByteArray msgid = l[4].mid(1, l[4].size()-2 );
        if ( msgid.endsWith("@free.pt"))
            msgid = msgid.left( msgid.length() - 8);

        //qDebug() << xoverLastReceived << "Title:" << subj_tag[0] << "Poster:" << l[2].left(p) << "Cat:" << cat << "Subcat:" << subcat << "Date:" << spotdate << "Msgid:" << msgid;
        if (!spotdate)
            return;

        if (_filtermanycat && sninfo[0].length() > 45)
        {
            qDebug() << "> 15 " << msgid;
            return; // meer dan 15 categorien
        }

        //qDebug() << "<"+msgid+">" << SpotSignature::verifyHeaderHashCash("<"+msgid+">");
        if (_filterhc && spotdate > 1316037600 && !SpotSignature::verifyHeaderHashCash("<"+msgid+">"))
        {
            qDebug() << "hc mismatch" << msgid;
            return;
        }

        QByteArray title = subj_tag[0].trimmed().replace("''", "'").replace("\"\"", "\"");

        if (title.isEmpty() )
        {
            qDebug() << "empty subject" << msgid;
            return;
        }

        if (ero)
        {
            _echeckDuplicatesQuery.bindValue(0, spotdate);
            _echeckDuplicatesQuery.bindValue(1, QString(title));
//            _echeckDuplicatesQuery.bindValue(1, msgid);
            if (!_echeckDuplicatesQuery.exec() )
                qDebug() << "Fout bij ontdubbelen";

            bool dubbel = _echeckDuplicatesQuery.next();
            _echeckDuplicatesQuery.finish();

            if ( dubbel )
            {
                //qDebug() << "Dubbele espot genegeerd";
                return;
            }
        }
        else
        {
            _checkDuplicatesQuery.bindValue(0, spotdate);
            _checkDuplicatesQuery.bindValue(1, QString(title));
//            _checkDuplicatesQuery.bindValue(1, msgid);
            if (!_checkDuplicatesQuery.exec() )
                qDebug() << "Fout bij ontdubbelen";

            bool dubbel = _checkDuplicatesQuery.next();
            _checkDuplicatesQuery.finish();

            if (dubbel)
            {
                //qDebug() << "Dubbele spot genegeerd";
                return;
            }
        }

        // INSERT INTO spots(cat,subcat,spotdate,title,spotter,tag,msgid) VALUES (?,?,?,?,?,?,?)

        if (ero)
        {
            _einsertQuery.bindValue(0, cat);
            _einsertQuery.bindValue(1, subcat);
            _einsertQuery.bindValue(2, spotdate);
            /*if (subj_tag[0].contains("?=") && subj_tag[0].contains("=?"))
                _einsertQuery.bindValue(3, _decodeUtf8(subj_tag[0].trimmed().replace("?= =?", "?==?")).toLatin1() );
            else*/
                _einsertQuery.bindValue(3, QString(title));
            _einsertQuery.bindValue(4, QString(l[2].left(p).trimmed()) );
            if ( subj_tag.count() > 1)
                _einsertQuery.bindValue(5, QString(subj_tag.last().trimmed()) );
            else
                _einsertQuery.bindValue(5, QString(""));
            _einsertQuery.bindValue(6, QString(msgid));
            _einsertQuery.bindValue(7, QString(othercats));
            _einsertQuery.bindValue(8, spotterid);

            if (!_einsertQuery.exec() )
                qDebug() << "Fout bij toevoegen aan database: " << _einsertQuery.lastError();
            _einsertQuery.finish();

            if (!_updateKnop)
                _neweposts++;
        }
        else
        {
            _insertQuery.bindValue(0, cat);
            _insertQuery.bindValue(1, subcat);
            _insertQuery.bindValue(2, spotdate);
            /*if (subj_tag[0].contains("?=") && subj_tag[0].contains("=?"))
                _insertQuery.bindValue(3, _decodeUtf8(subj_tag[0].trimmed().replace("?= =?", "?==?")).toLatin1() );
            else*/
                _insertQuery.bindValue(3, QString(title) );
            _insertQuery.bindValue(4, QString(spotter));
            if ( subj_tag.count() > 1)
                _insertQuery.bindValue(5, QString(subj_tag.last().trimmed()) );
            else
                _insertQuery.bindValue(5, QString(""));
            _insertQuery.bindValue(6, QString(msgid));
            _insertQuery.bindValue(7, QString(othercats));
            _insertQuery.bindValue(8, spotterid);

            if (!_insertQuery.exec() )
                qDebug() << "Fout bij toevoegen aan database:" << _insertQuery.lastError();
            _insertQuery.finish();

            if (!_updateKnop)
                _newposts++;
        }
    }
}

void SpotLite::onGroupResponse(const QByteArray &group, qint64 min, qint64 max)
{
    if ( _queue.empty() )
    {
        //disconnect(this, SLOT(onXroverLine(qint64,QByteArray)));
        //disconnect(this, SLOT(onXroverSpamLine(qint64,QByteArray)));

        if (group == "free.pt")
        {
            qint64 lastReceived = _settings.value("lastheader").toLongLong();
            int    maxSpots     = _settings.value("maxspots").toInt()-1;

            if (max > lastReceived)
            {
                maxOnServer = max;
                if (lastReceived)
                    min = lastReceived + 1;

                min = qMax(min, max-maxSpots);
                xoverSentForMax   = qMin(max, min+MAX_PER_CYCLE);
                xoverLastReceived = 0;

                if (!startid)
                    startid = min;

                _transaction = true; _spotDatabase.transaction();
                emit notice(0, tr("Bezig met binnenhalen van nieuwe headers"));
                _lastStatusUpdate.start();
                _nntp->xover(min, xoverSentForMax);
            }
            else
            {
//                _refreshHeaderTimer.start(119000);

                if ( _settings.value("spamreports", true).toBool() )
                    _nntp->group("free.willey");
                else
                    _nntp->group("free.usenet");
            }
        }
        else if (group == "free.willey")
        {
            qint64 lastReceived = _settings.value("lastspamheader").toLongLong();
            int    maxSpots     = _settings.value("maxspots").toInt()-1;

            if (max > lastReceived)
            {
                smaxOnServer = max;
                if (lastReceived)
                    min = lastReceived + 1;

                min = qMax(min, max-maxSpots);
                sxroverSentForMax   = qMin(max, min+MAX_PER_CYCLE_XHDR);
                sxroverLastReceived = 0;

                emit notice(0, tr("Bezig met binnenhalen van nieuwe spam meldingen"));
                //connect(_nntp, SIGNAL(xroverLine(qint64,QByteArray)), this, SLOT(onXroverSpamLine(qint64,QByteArray)));
                _nntp->xrover(min, sxroverSentForMax);
            }
            else
            {
                _nntp->group("free.usenet");
            }
        }
        else if (group == "free.usenet")
        {
            qint64 lastReceived = _settings.value("lastcommentsheader").toLongLong();
            int    maxSpots     = _settings.value("maxspots").toInt()-1;

            if (max > lastReceived)
            {
                cmaxOnServer = max;
                if (lastReceived)
                    min = lastReceived + 1;

                min = qMax(min, max-maxSpots);
                cxroverSentForMax   = qMin(max, min+MAX_PER_CYCLE_XHDR);
                cxroverLastReceived = 0;

                _commlistTransaction = true;
                emit notice(0, tr("Bezig met binnenhalen van nieuwe reactielijst"));
                //connect(_nntp, SIGNAL(xroverLine(qint64,QByteArray)), this, SLOT(onXroverLine(qint64,QByteArray)));
                _nntp->xrover(min, cxroverSentForMax);
            }
            else
            {
                if (_updateKnop && !_uptodate && _settings.value("updateonstart").toBool() )
                {
                    _uptodate = true;
                    processNewSpots();
                }

                // nothing new, try again in 119 seconds
                _refreshHeaderTimer.start(HEADER_REFRESH_INTERVAL);
            }

        }
    }
    else
        onLoggedIn();
}

void SpotLite::onXroverLine(qint64 nr, const QByteArray &references)
{
    if (nr == -1)
    {
        qDebug() << "S: [einde van XHDR data]";

        if ( !_commlist.isEmpty() )
        {
            emit notice(0, tr("Bezig met opslaan van lijst met reacties"));
            QApplication::processEvents();

            QSqlQuery q(_commlistDatabase);
            _commlistDatabase.transaction();
            q.prepare("REPLACE INTO msgnrs(post,nrs) VALUES (?,?)");

            QMap<QByteArray, QList<int> >::const_iterator iter;
            for (iter = _commlist.constBegin(); iter != _commlist.constEnd(); iter++)
            {
                QByteArray  d;
                QDataStream ds(&d, QIODevice::WriteOnly);

                foreach (int i, iter.value() )
                {
                    ds << i;
                }

                q.bindValue(0, (QString) iter.key() );
                q.bindValue(1, d);
                q.exec();
            }

            q.finish();
            _commlistDatabase.commit();
            _commlist.clear();
        }
        _commlistTransaction = false;

        if (cxroverLastReceived)
        {
            _settings.setValue("lastcommentsheader", cxroverLastReceived);
            _settings.sync();
        }

//        if ( _queue.isEmpty() )
//        {
            if (cxroverSentForMax < cmaxOnServer) /* more on server */
            {
                qint64 min = cxroverSentForMax + 1;
                cxroverSentForMax = qMin(cmaxOnServer, min+MAX_PER_CYCLE_XHDR);
                cxroverLastReceived = 0;

                _commlistTransaction = true; //_commlistDatabase.transaction();
                emit notice(0, tr("Bezig met ophalen van reacties lijst"));
                _nntp->xrover(min, cxroverSentForMax);
            }
            else
            {
                // all done
                emit notice(0, QString("Klaar met binnenhalen van lijst met reacties..."));

                //_commlistDatabase.exec("PRAGMA wal_checkpoint");

                if (_updateKnop && !_uptodate && _settings.value("updateonstart").toBool() )
                {
                    _uptodate = true;
                    QApplication::setOverrideCursor( Qt::BusyCursor );
                    QApplication::processEvents();
                    processNewSpots();
                    QApplication::restoreOverrideCursor();
                }
                if ( !watchlist_msgids.isEmpty() || !ewatchlist_msgids.isEmpty() )
                {
                    QApplication::setOverrideCursor( Qt::BusyCursor );
                    QApplication::processEvents();
                    processNewSpots();
                    _restoreWatchlist();
                    QApplication::restoreOverrideCursor();
                }

                _refreshHeaderTimer.start(HEADER_REFRESH_INTERVAL);
            }
//        }
//        else
//           onLoggedIn();
    }
    else
    {
        if ( cxroverLastReceived % 1000 == 0 )
        {
            emit notice(0, QString("Headeren van reacties %1 van %2").arg( QString::number(cxroverLastReceived), QString::number(cmaxOnServer)) );
            QApplication::processEvents();
        }

        //if ( references.length() < 12 || !references.contains("@free.pt") )
        //    return;

        cxroverLastReceived = nr;
        //QByteArray msgid = references.mid(1, references.size()-10 );
        QByteArray msgid;
        if ( references.contains("@free.pt") )
            msgid = references.mid(1, references.size()-10 );
        else
            msgid = references.mid(1, references.size()-2 );

        if ( !_commlist.contains(msgid) )
        {
            QList<int> l;
            _checkCommlistQuery.bindValue(0, QString( msgid ));
            _checkCommlistQuery.exec();

            if (_checkCommlistQuery.next() )
            {
                QByteArray d = _checkCommlistQuery.value(0).toByteArray();
                QDataStream ds(&d, QIODevice::ReadOnly);

                while (!ds.atEnd() )
                {
                    int i;
                    ds >> i;
                    l.append(i);
                }
                _commlist[msgid] = l;
            }
            else
            {
                _commlist[msgid] = l;
            }
            _checkCommlistQuery.finish();
        }
        QList<int> l = _commlist[msgid];
        l.append( (int) nr);
        _commlist[msgid] = l;

        //qDebug() << xoverLastReceived << "Title:" << subj_tag[0] << "Poster:" << l[2].left(p) << "Cat:" << cat << "Subcat:" << subcat << "Date:" << spotdate << "Msgid:" << msgid;
    }
}

void SpotLite::onXroverSpamLine(qint64 nr, const QByteArray &references)
{
    if (nr == -1)
    {
        qDebug() << "S: [einde van spam XHDR data]";
        qDebug() << _spamcountlist.count();

        if ( !_spamcountlist.isEmpty() )
        {
            emit notice(0, tr("Bezig met opslaan van spamlijst (%1) stuks #1").arg( QString::number(_spamcountlist.count() )));
            QApplication::processEvents();

            _spotDatabase.transaction();
            emit notice(0, tr("Bezig met opslaan van spamlijst (%1) stuks #2").arg( QString::number(_spamcountlist.count() )));
            QApplication::processEvents();
            _transaction = true;
            QSqlQuery q(_spotDatabase), q2(_spotDatabase), q3(_spotDatabase);
            QSqlQuery sq(_spotDatabase), sq2(_spotDatabase);
            q.prepare("UPDATE spots SET spamcount=spamcount+? WHERE msgid=?");
            q2.prepare("UPDATE erospots SET spamcount=spamcount+? WHERE msgid=?");
            q3.prepare("UPDATE queued_spots SET spamcount=spamcount+? WHERE msgid=?");

            int highSpamThreshold = 0;
            if ( _settings.value("deletespam").toBool() )
            {
                highSpamThreshold = _settings.value("highspam", 10).toInt();
                sq.prepare("SELECT spamcount,id FROM spots WHERE msgid=?");
                sq2.prepare("SELECT spamcount,id FROM erospots WHERE msgid=?");
            }

            QMap<QByteArray, int>::const_iterator iter;
            for (iter = _spamcountlist.constBegin(); iter != _spamcountlist.constEnd(); iter++)
            {
                const QByteArray &id = iter.key();
                int c = iter.value();

                q.bindValue(0, c );
                q.bindValue(1, id );
                q.exec();

                if ( q.numRowsAffected() == 0)
                {
                    q2.bindValue(0, c );
                    q2.bindValue(1, id );
                    q2.exec();

                    if ( q2.numRowsAffected() == 0)
                    {
                        q3.bindValue(0, c );
                        q3.bindValue(1, id );
                        q3.exec();
                    }
                    else if (highSpamThreshold)
                    {
                        sq2.bindValue(0, id);
                        sq2.exec();
                        if (sq2.next() && sq2.value(0).toInt() > highSpamThreshold)
                            deleteSpot( 0-sq2.value(1).toInt() );
                    }
                }
                else if (highSpamThreshold)
                {
                    sq.bindValue(0, id);
                    sq.exec();
                    if (sq.next() && sq.value(0).toInt() > highSpamThreshold)
                        deleteSpot( sq.value(1).toInt() );
                }
            }

            emit notice(0, tr("Bezig met opslaan van spamlijst (%1) stuks #3").arg( QString::number(_spamcountlist.count() )));
            QApplication::processEvents();
            _spotDatabase.commit();
            _transaction = false;
            _spamcountlist.clear();
        }

        if (sxroverLastReceived)
        {
            _settings.setValue("lastspamheader", sxroverLastReceived);
            _settings.sync();
        }

        if ( _queue.isEmpty() )
        {
            if (sxroverSentForMax < smaxOnServer) /* more on server */
            {
                qint64 min = sxroverSentForMax + 1;
                sxroverSentForMax = qMin(smaxOnServer, min+MAX_PER_CYCLE_XHDR);
                sxroverLastReceived = 0;

                emit notice(0, tr("Bezig met ophalen van spam melding lijst"));
                _nntp->xrover(min, sxroverSentForMax);
            }
            else
            {
                // all done
                emit newSpots();
                emit notice(0, tr("Klaar met spam meldingen, ga nu de reacties ophalen"));
                _nntp->group("free.usenet");
            }
        }
        else
        {
            emit notice(0, tr("Headers ophalen onderbroken, wegens binnenhalen spotinfo"));
            onLoggedIn();
        }
    }
    else
    {
        if ( sxroverLastReceived % 1000 == 0 )
        {
            emit notice(0, QString("Headeren van spam meldingen %1 van %2").arg( QString::number(sxroverLastReceived), QString::number(smaxOnServer)) );
            QApplication::processEvents();
        }

        sxroverLastReceived = nr;
        QByteArray msgid;
        if ( references.contains("@free.pt") )
            msgid = references.mid(1, references.size()-10 );
        else
            msgid = references.mid(1, references.size()-2 );

        if ( !_spamcountlist.contains(msgid) )
        {
            _spamcountlist.insert(msgid, 1);
        }
        else
        {
            _spamcountlist[msgid]++;
        }
    }
}

void SpotLite::onError(int code, const QString &msg, const QByteArray &msgid)
{
    SpotNNTP *nntp = static_cast<SpotNNTP *> ( sender() );
    qDebug() << "FOUT:" << msg;

    if (_transaction)
    {
        _spotDatabase.rollback();
        _transaction = false;
    }
    /*else*/ if (_commlistTransaction)
    {
        _commlist.clear();
        _commlistTransaction = false;
    }

    if (code == SpotNNTP::PASSWORD_NEEDED)
    {
        emit error(0, tr("Vul het wachtwoord van je Usenet provider in bij de instellingen"));

        _nntp->deleteLater();
        _nntp = NULL;
    }
    else if (code == SpotNNTP::PASSWORD_INCORRECT)
    {
        emit error(0, tr("Wachtwoord onjuist. Controleer de instellingen"));

        _nntp->deleteLater();
        _nntp = NULL;
    }
    else if (code == SpotNNTP::SETTINGS_CHANGE)
    {
        emit notice(0, tr("Instellingen gewijzigd") );
        _nntp->deleteLater();
        _nntp = NULL;
    }
    else if (code == QAbstractSocket::HostNotFoundError)
    {
        emit error(0, tr("Server adres van Usenet provider onjuist. Controlleer instellingen"));

        _nntp->deleteLater();
        _nntp = NULL;
    }
    else if (code == SpotNNTP::XHDR_NOT_SUPPORTED)
    {
        emit notice(0, tr("Usenet provider ondersteund geen XHDR, kan geen reacties ophalen"));
        _refreshHeaderTimer.start(HEADER_REFRESH_INTERVAL);
    }
    else if (code == SpotNNTP::SSL_ERROR)
    {
        emit error(0, msg);

        _nntp->deleteLater();
        _nntp = NULL;
    }
    else
    {
        if ( !msgid.isEmpty() )
        {
            _queue.prepend(msgid);
        }

        emit notice(0, tr("Fout: %1").arg(msg) );
        nntp->retry_after_seconds(msg.contains("timeout", Qt::CaseInsensitive) ? 5 : 60 );
    }
}

void SpotLite::onArticleNotFound(const QByteArray &msgid)
{
    emit articleNotFound(msgid);
    onLoggedIn();
}

void SpotLite::onRefreshTimer()
{
    if ( !_nntp )
    {
        connectToServer();
    }
    else if ( _nntp->idle() )
    {
        _nntp->group("free.pt");
    }
    else
    {
        _refreshHeaderTimer.start(1000);
    }
}

int SpotLite::numNewPosts() const
{
    return _newposts;
}

int SpotLite::numNewEPosts() const
{
    return _neweposts;
}

void SpotLite::reconnect()
{
    if (_nntp)
    {
        _nntp->abort_and_emit_error(SpotNNTP::SETTINGS_CHANGE, "");
    }

    _prepareInsertQueries();
    connectToServer();
}

QString SpotLite::_decodeUtf8(const QByteArray &s)
{
    int startpos;
    QString r = s;

    while ((startpos = r.indexOf("=?UTF-8?B?")) != -1)
    {
        int endpos = r.indexOf("?=", startpos+10);
        if (endpos == -1)
            break;

        r = r.left(startpos)+QString::fromUtf8( QByteArray::fromBase64(r.mid(startpos+10, endpos-startpos-10).toLatin1() ) ) + r.mid(endpos+2);
    }

    return r.toLatin1();
}

bool SpotLite::vacuumDB()
{
    if (_transaction)
        return false;

    QSqlQuery q(_spotDatabase);
    //_spotDatabase.transaction();
    q.exec("VACUUM");
   // _spotDatabase.commit();
    q = QSqlQuery(_cacheDatabase);
   // _cacheDatabase.transaction();
    q.exec("VACUUM");
   // _cacheDatabase.commit();
    q = QSqlQuery(_commlistDatabase);
   // _commlistDatabase.transaction();
    q.exec("VACUUM");
   // _commlistDatabase.commit();

    return true;
}

void SpotLite::emptyCommentsList()
{
    QSqlQuery q(_commlistDatabase);
    q.exec("DELETE FROM msgnrs");
    q.exec("VACUUM");

    q = QSqlQuery(_cacheDatabase);
    q.exec("DELETE FROM cache WHERE id NOT LIKE '%@%'");

    startid = 0;
}

QList<int> SpotLite::getCommentMsgNrs(const QByteArray &msgid)
{
    QList<int> l;

    _checkCommlistQuery.bindValue(0, QString( msgid ));
    _checkCommlistQuery.exec();

    if (_checkCommlistQuery.next() )
    {
//        qDebug() << "yoyo" << msgid;

        QByteArray d = _checkCommlistQuery.value(0).toByteArray();
        QDataStream ds(&d, QIODevice::ReadOnly);

        while (!ds.atEnd() )
        {
            int i;
            ds >> i;
            if (!l.contains(i))
                l.append(i);
        }
    }
    _checkCommlistQuery.finish();
//    else
//        qDebug() << "nono" << msgid << msgid.length();

    return l;
}

QSet<int> SpotLite::getCommentMsgIDnrs(const QByteArray &msgid)
{
    QSet<int> r;
    QList<int> msgnrs = getCommentMsgNrs(msgid);

    foreach (int msgnr, msgnrs)
    {
        QByteArray data = _fetchFromCache( QByteArray::number(msgnr) );

        int p1 = data.indexOf("Message-ID: <");
        if (p1 != -1)
        {
            int p2 = data.indexOf("@", p1);
            QByteArray m = data.mid(p1, p2-p1);
            if ( m.count('.') == 1)
            {
                int i = m.mid(m.indexOf('.')+1).toInt();
                if (i)
                    r.insert(i);
            }
        }
    }

    return r;
}

bool SpotLite::emptyDatabases()
{
    if (_transaction)
        return false;

    /*
    _spotDatabase.close();
    _cacheDatabase.close();
    _commlistDatabase.close();
    QString dirname = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QFile::remove(dirname+"/spots.db");
    QFile::remove(dirname+"/cache.db");
    QFile::remove(dirname+"/reactielijst.db");
    _openDatabases(); */
    _saveWatchlist();

    if ( _refreshHeaderTimer.isActive() )
        _refreshHeaderTimer.stop();

    QSqlQuery q(_spotDatabase);
    q.exec("DELETE FROM spots");
    q.exec("DELETE FROM erospots");
    q.exec("DELETE FROM queued_spots");
    q.exec("DELETE FROM trash_spots");
    q.exec("DELETE FROM spotters");
    q.exec("VACUUM");

    q = QSqlQuery(_cacheDatabase);
    q.exec("DELETE FROM cache");
    q.exec("VACUUM");

    q = QSqlQuery(_commlistDatabase);
    q.exec("DELETE FROM msgnrs");
    q.exec("VACUUM");

    _settings.setValue("lastheader", (int) 0);
    _settings.setValue("lastcommentsheader", (int) 0);
    _settings.setValue("lastspamheader", (int) 0);
    _settings.setValue("watchlist", "");
    _settings.setValue("ewatchlist", "");
    _settings.sync();

    _newposts = _neweposts = startid = 0;
    _uptodate = false;
    emit newSpots();
    return true;
}

bool SpotLite::isOnWatchlist(int spotid)
{
    const char *wlname;
    if (spotid < 0)
    {
        spotid = 0-spotid;
        wlname = "ewatchlist";
    }
    else
        wlname = "watchlist";

    QList<QByteArray> wl = _settings.value(wlname).toByteArray().split(',');
    return wl.contains(QByteArray::number(spotid));
}

void SpotLite::addToWatchlist(int spotid)
{
    if (!isOnWatchlist(spotid))
    {
        const char *wlname;
        if (spotid < 0)
        {
            spotid = 0-spotid;
            wlname = "ewatchlist";
        }
        else
            wlname = "watchlist";

        QByteArray wl = _settings.value(wlname).toByteArray();
        if (wl.isEmpty())
            wl = QByteArray::number(spotid);
        else
            wl += "," + QByteArray::number(spotid);

        _settings.setValue(wlname, wl);
        emit newSpots();
    }
}

void SpotLite::removeFromWatchlist(int spotid)
{
    const char *wlname;
    if (spotid < 0)
    {
        spotid = 0-spotid;
        wlname = "ewatchlist";
    }
    else
        wlname = "watchlist";

    QList<QByteArray> wl = _settings.value(wlname).toByteArray().split(',');
    wl.removeAll( QByteArray::number(spotid) );

    QByteArray newList;

    foreach (QByteArray item, wl)
    {
        if (newList.isEmpty())
            newList = item;
        else
            newList += "," + item;
    }

    _settings.setValue(wlname, newList);
    emit newSpots();
}

void SpotLite::_cleanWatchlist()
{
    /* Normal spots */
    QByteArray wl = _settings.value("watchlist").toByteArray();
    if (!wl.isEmpty())
    {
        QList<QByteArray> wll = wl.split(',');
        QSqlQuery q(_spotDatabase);
        q.prepare("SELECT cat FROM spots WHERE id=?");

        foreach (QByteArray wlentry, wll)
        {
            int w = wlentry.toInt();

            q.bindValue(0, w);
            q.exec();

            if (!q.next() )
                removeFromWatchlist(w);
        }
    }

    /* eSpots */
    wl = _settings.value("ewatchlist").toByteArray();
    if (!wl.isEmpty())
    {
        QList<QByteArray> wll = wl.split(',');
        QSqlQuery q(_spotDatabase);
        q.prepare("SELECT cat FROM erospots WHERE id=?");

        foreach (QByteArray wlentry, wll)
        {
            int w = wlentry.toInt();

            q.bindValue(0, w);
            q.exec();

            if (!q.next() )
                removeFromWatchlist(-w);
        }
    }
}

QSettings *SpotLite::settings()
{
    return &_settings;
}

bool SpotLite::inTransaction()
{
    return _transaction;
}

void SpotLite::reloadFriendsAndFoes()
{
    QSqlQuery q(_spotDatabase);
    _friends.clear();
    _foes.clear();

    q.exec("SELECT userid FROM friends");
    while ( q.next() )
    {
        _friends.insert( q.value(0).toByteArray() );
    }
    q.exec("SELECT userid FROM foes");
    while ( q.next() )
    {
        _foes.insert( q.value(0).toByteArray() );
    }
}

bool SpotLite::isFriend(const QByteArray &userid) const
{
    return _friends.contains(userid);
}

bool SpotLite::isFoe(const QByteArray &userid) const
{
    return _foes.contains(userid);
}

void SpotLite::addFriend(const QByteArray &userid, const QByteArray &username)
{
    bool oldtrans = _transaction;
    if (!_transaction)
    {
        _spotDatabase.transaction();
        _transaction = true;
    }

    _friends.insert(userid);
    QSqlQuery q(_spotDatabase);
    q.prepare("INSERT INTO friends(userid,username) VALUES (?,?)");
    q.addBindValue(QString(userid));
    q.addBindValue(QString(username));
    q.exec();

    if (!oldtrans)
    {
        _spotDatabase.commit();
        _transaction = false;
    }
}

void SpotLite::banUser(const QByteArray &userid, const QByteArray &username, const QByteArray &reason)
{
    bool oldtrans = _transaction;
    if (!_transaction)
    {
        _spotDatabase.transaction();
        _transaction = true;
    }

    _foes.insert(userid);
    QSqlQuery q(_spotDatabase);
    q.prepare("INSERT INTO foes(userid,username,reason) VALUES (?,?,?)");
    q.addBindValue(QString(userid));
    q.addBindValue(QString(username));
    q.addBindValue(QString(reason));
    q.exec();

    q.prepare("SELECT id FROM spotters WHERE pubkey=?");
    q.addBindValue(QString(userid));
    q.exec();

    if (q.next() )
    {
        int spotterid = q.value(0).toInt();

        q.prepare("SELECT id FROM spots WHERE spotterid=?");
        q.addBindValue(QString(spotterid));
        q.exec();
        while ( q.next() )
        {
            deleteSpot(q.value(0).toInt(), "", "", false);
        }
        q.prepare("SELECT id FROM erospots WHERE spotterid=?");
        q.addBindValue(QString(spotterid));
        q.exec();
        while ( q.next() )
        {
            deleteSpot(0-q.value(0).toInt(), "", "", false);
        }

        emit newSpots();
    }

    if (!oldtrans)
    {
        _spotDatabase.commit();
        _transaction = false;
    }
}

void SpotLite::_initDBsettings(QSqlDatabase &db)
{
    QSqlQuery q(db);

    //q.exec("PRAGMA synchronous=1");
    q.exec("PRAGMA synchronous=0");
    q.exec("PRAGMA automatic_index=0");
    q.exec("PRAGMA legacy_file_format=0");

    if ( !_settings.value("sharedb", false).toBool() )
        q.exec("PRAGMA locking_mode=EXCLUSIVE");
    q.exec("PRAGMA journal_mode=WAL");
    q.exec("PRAGMA journal_size_limit=16000000");
}

void SpotLite::exportDatabase(const QString &filename)
{
    QSqlQuery q(_spotDatabase);

    q.prepare("ATTACH DATABASE ? AS exportfile");
    q.addBindValue( QDir::toNativeSeparators(filename) );
    q.exec();
    _spotDatabase.transaction();
    q.exec("CREATE TABLE exportfile.exspots AS SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount FROM spots WHERE spamcount<10 ORDER BY spotdate");
    q.exec("CREATE TABLE exportfile.exespots AS SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount FROM erospots WHERE spamcount<10 ORDER BY spotdate");
    _spotDatabase.commit();
    q.exec("DETACH DATABASE exportfile");
}

bool SpotLite::importDatabase(const QString &filename)
{
    QSqlQuery q(_spotDatabase);
    int oldnum = -1, newnum = -1;

    if (_transaction)
        return false;

    q.prepare("ATTACH DATABASE ? AS importfile");
    q.addBindValue( QDir::toNativeSeparators(filename) );
    q.exec();
    _spotDatabase.transaction();

    _saveWatchlist();

    q.exec("CREATE TABLE spots2(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "cat INT,"
                "subcat INT,"
                "spotdate INT,"
                "title VARCHAR(255),"
                "spotter VARCHAR(255),"
                "tag VARCHAR(255),"
                "othercats VARCHAR(255),"
                "msgid VARCHAR(255), spamcount INT DEFAULT 0, spotterid INT DEFAULT 0)"
           );
    q.exec("INSERT INTO spots2(cat,subcat,spotdate,title,spotter,tag,othercats,msgid) SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid FROM importfile.exspots UNION SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid FROM spots WHERE spamcount<10 ORDER BY spotdate");

    // TODO spamcounts

    q.exec("SELECT count(*) FROM spots");
    q.next();
    oldnum = q.value(0).toInt();
    q.exec("SELECT count(*) FROM spots2");
    q.next();
    newnum = q.value(0).toInt();

    if (newnum < oldnum)
    {
        qDebug() << oldnum << newnum;
        _spotDatabase.rollback();
        q.exec("DETACH DATABASE importfile");
        return false;
    }
    q.exec("DROP TABLE spots");
    q.exec("ALTER TABLE spots2 RENAME TO spots");

    q.exec("CREATE TABLE erospots2(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "cat INT,"
                "subcat INT,"
                "spotdate INT,"
                "title VARCHAR(255),"
                "spotter VARCHAR(255),"
                "tag VARCHAR(255),"
                "othercats VARCHAR(255),"
                "msgid VARCHAR(255), spamcount INT DEFAULT 0, spotterid INT DEFAULT 0)"
           );
    q.exec("INSERT INTO erospots2(cat,subcat,spotdate,title,spotter,tag,othercats,msgid) SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid FROM importfile.exespots UNION SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid FROM erospots WHERE spamcount<10 ORDER BY spotdate");

    // TODO spamcounts

    q.exec("DROP TABLE erospots");
    q.exec("ALTER TABLE erospots2 RENAME TO erospots");
    _spotDatabase.commit();
    q.exec("VACUUM");

    _spotDatabase.transaction();
    q.exec("CREATE UNIQUE INDEX catidx ON spots(cat,id)");
    q.exec("CREATE UNIQUE INDEX subcatidx ON spots(subcat,id)");
    q.exec("CREATE INDEX tagidx ON spots(tag)");
    q.exec("CREATE INDEX dateidx ON spots(spotdate)");
    q.exec("CREATE UNIQUE INDEX mididx ON spots(msgid)");

    q.exec("CREATE UNIQUE INDEX ecatidx ON erospots(cat,id)");
    q.exec("CREATE UNIQUE INDEX esubcatidx ON erospots(subcat,id)");
    q.exec("CREATE INDEX etagidx ON erospots(tag)");
    q.exec("CREATE INDEX edateidx ON erospots(spotdate)");
    q.exec("CREATE UNIQUE INDEX meididx ON erospots(msgid)");

    _spotDatabase.commit();
    q.exec("DETACH DATABASE importfile");

    _restoreWatchlist();

    return true;
}

void SpotLite::_saveWatchlist()
{
    QSqlQuery q(_spotDatabase);
    QByteArray wl = _settings.value("watchlist").toByteArray();
    if (!wl.isEmpty())
    {
        QList<QByteArray> wll = wl.split(',');
        q.prepare("SELECT msgid FROM spots WHERE id=?");

        foreach (QByteArray wlentry, wll)
        {
            int w = wlentry.toInt();

            q.bindValue(0, w);
            q.exec();

            if (q.next() )
                watchlist_msgids.append( q.value(0).toByteArray() );
        }
    }
    wl = _settings.value("ewatchlist").toByteArray();
    if (!wl.isEmpty())
    {
        QList<QByteArray> wll = wl.split(',');
        q.prepare("SELECT msgid FROM erospots WHERE id=?");

        foreach (QByteArray wlentry, wll)
        {
            int w = wlentry.toInt();

            q.bindValue(0, w);
            q.exec();

            if (q.next() )
                ewatchlist_msgids.append( q.value(0).toByteArray() );
        }
    }
}

void SpotLite::_restoreWatchlist()
{
    QSqlQuery q(_spotDatabase);
    if ( !watchlist_msgids.isEmpty() || !ewatchlist_msgids.isEmpty() )
    {
        _settings.setValue("watchlist", "");
        _settings.setValue("ewachlist", "");

        q.prepare("SELECT id FROM spots WHERE msgid=?");
        foreach (QByteArray msgid, watchlist_msgids)
        {
            q.bindValue(0, msgid);
            q.exec();

            if (q.next() )
                addToWatchlist( q.value(0).toInt() );
        }

        q.prepare("SELECT id FROM erospots WHERE msgid=?");
        foreach (QByteArray msgid, ewatchlist_msgids)
        {
            q.bindValue(0, msgid);
            q.exec();

            if (q.next() )
                addToWatchlist( 0-q.value(0).toInt() );
        }

        watchlist_msgids.clear(); ewatchlist_msgids.clear();
    }
}

void SpotLite::deleteSpot(int spotid, const QString &reason, const QString &moderator, bool emitnewSpots)
{
    if (!_transaction)
        _spotDatabase.transaction();

    QSqlQuery q(_spotDatabase);
    int updated = QDateTime::currentDateTime().toTime_t();

    if ( isOnWatchlist(spotid) )
        removeFromWatchlist(spotid);

    if (spotid > 0)
    {
        q.prepare("INSERT INTO trash_spots(id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,moderator,updated,reason,spamcount,spotterid) SELECT id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,? AS moderator,? AS updated,? AS reason,spamcount,spotterid FROM spots WHERE id=?");
        q.addBindValue(moderator);
        q.addBindValue(updated);
        q.addBindValue(reason);
        q.addBindValue(spotid);
        q.exec();
        q.prepare("DELETE FROM spots WHERE id=?");
        q.addBindValue(spotid);
        q.exec();
    }
    else
    {
        q.prepare("INSERT INTO trash_spots(id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,moderator,updated,reason,spamcount,spotterid) SELECT 0-id AS id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,? AS moderator,? AS updated,? AS reason,spamcount,spotterid FROM erospots WHERE id=0-?");
        q.addBindValue(moderator);
        q.addBindValue(updated);
        q.addBindValue(reason);
        q.addBindValue(spotid);
        q.exec();
        q.prepare("DELETE FROM erospots WHERE id=0-?");
        q.addBindValue(spotid);
        q.exec();
    }

    if (!_transaction)
        _spotDatabase.commit();

    if (emitnewSpots)
        emit newSpots();
}

void SpotLite::processNewSpots()
{
    emit processingQueuedSpots();

    /*QFileInfo fi;
    fi.setFile("/home/max/.local/share/data/SpotLite/SpotLite/spots.db-wal");
    qDebug() << "procnewspots begin" << _transaction << fi.size();*/

    bool oldtransaction = _transaction;
    _transaction = true;
    if (!oldtransaction)
    {
        _spotDatabase.transaction();
    }

    bool delspam = _settings.value("deletespam").toBool();
    int minid = 0, mineid = 0;

    if (delspam)
    {
        QSqlQuery q(_spotDatabase);
        q.exec("SELECT max(id) FROM spots");
        q.next();
        minid = q.value(0).toInt();

        q.exec("SELECT max(id) FROM erospots");
        q.next();
        mineid = q.value(0).toInt();
    }


    _newposts  = _spotDatabase.exec("INSERT INTO spots(cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid) SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid FROM queued_spots WHERE cat<>9 ORDER BY spotdate").numRowsAffected();
    _neweposts = _spotDatabase.exec("INSERT INTO erospots(cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid) SELECT cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid FROM queued_spots WHERE cat=9 ORDER BY spotdate").numRowsAffected();
    _spotDatabase.exec("PRAGMA wal_checkpoint");
    _spotDatabase.commit();
    _spotDatabase.transaction();
    _spotDatabase.exec("DELETE FROM queued_spots");

    if (delspam)
        deleteSpam(minid, mineid);

    _spotDatabase.commit();
/*    _insertQuery.finish();
    _einsertQuery.finish();*/

    if (oldtransaction)
        _spotDatabase.transaction();
    _transaction = oldtransaction;

    //fi.refresh();
    //qDebug() << "end of processnewspots" << fi.size();

    emit newSpots();
}

void SpotLite::undeleteSpot(int spotid)
{
    if (!_transaction)
        _spotDatabase.transaction();

    QSqlQuery q(_spotDatabase);

    if (spotid > 0)
    {
        q.prepare("INSERT INTO spots(id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid) SELECT id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid FROM trash_spots WHERE id=?");
    }
    else
    {
        q.prepare("INSERT INTO erospots(id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid) SELECT 0-id AS id,cat,subcat,spotdate,title,spotter,tag,othercats,msgid,spamcount,spotterid FROM trash_spots WHERE id=?");
    }

    q.addBindValue(spotid);
    if ( q.exec() )
    {
        q.prepare("DELETE FROM trash_spots WHERE id=?");
        q.addBindValue(spotid);
        q.exec();
    }

    if (!_transaction)
        _spotDatabase.commit();

    emit newSpots();
}

void SpotLite::onFilterDone()
{
    QProcess *proc = qobject_cast<QProcess *>( sender() );
    qDebug() << "Extern programma klaar, exit code:" << proc->exitCode();
    proc->deleteLater();
    emit newSpots();
}

void SpotLite::emptyTrash()
{
    QSqlQuery q(_spotDatabase);
    q.exec("DELETE FROM trash_spots");
    emit newSpots();
}

void SpotLite::onXrover(qint64 nr, const QByteArray &references)
{
    if (_commlistTransaction)
        onXroverLine(nr, references);
    else
        onXroverSpamLine(nr, references);
}

SpotNNTP *SpotLite::nntp()
{
    return _nntp;
}

void SpotLite::deleteSpam(int minid, int mineid)
{
    int highSpamThreshold = _settings.value("highspam", 10).toInt();

    bool oldtransaction = _transaction;
    _transaction = true;

    if (!oldtransaction)
        _spotDatabase.transaction();

    QSqlQuery q(_spotDatabase);
    if (minid == -1)
    {
        q.prepare("SELECT id FROM spots WHERE spamcount>?");
    }
    else
    {
        q.prepare("SELECT id FROM spots WHERE spamcount>? AND id>?");
        q.bindValue(1, minid);
    }
    q.bindValue(0, highSpamThreshold);
    q.exec();

    while ( q.next() )
    {
        deleteSpot( q.value(0).toInt(), "", "", false);
    }

    if (mineid == -1)
    {
        q.prepare("SELECT id FROM erospots WHERE spamcount>?");
    }
    else
    {
        q.prepare("SELECT id FROM erospots WHERE spamcount>? AND id>?");
        q.bindValue(1, mineid);
    }

    q.bindValue(0, highSpamThreshold);
    q.exec();

    while ( q.next() )
    {
        deleteSpot( 0-q.value(0).toInt(), "", "", false);
    }

    if (!oldtransaction)
        _spotDatabase.commit();
    _transaction = oldtransaction;

    emit newSpots();
}

int SpotLite::_getSpotterID(const QByteArray &pubkey, const QByteArray &name)
{
    if (pubkey.isEmpty())
        return 0;

    int result;

    _checkSpotter.bindValue(0, pubkey);
    _checkSpotter.exec();

    if ( _checkSpotter.next() )
    {
        result = _checkSpotter.value(0).toInt();
    }
    else
    {
        _insertSpotter.bindValue(0, pubkey);
        _insertSpotter.bindValue(1, name);
        _insertSpotter.exec();

        result = _insertSpotter.lastInsertId().toInt();
        _insertSpotter.finish();
    }

    _checkSpotter.finish();
    return result;
}
