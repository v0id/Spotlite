#include "cachewarmer.h"
#include <QFile>
#include <QTime>
#include <QDebug>

CacheWarmer::CacheWarmer(QObject *parent) :
    QThread(parent)
{
}

void CacheWarmer::run()
{
    msleep(100);

    qDebug() << "Cache opwarmen (async)";
    QTime t;
    t.start();

    foreach (QString file, _files)
    {
        qDebug() << "Warming" << file;
        _warmCache(file);
    }
    qDebug() << "Warm in" << t.elapsed() << "milisec";
}

void CacheWarmer::addFile(const QString &filename)
{
    _files.append(filename);
}

void CacheWarmer::_warmCache(const QString &filename)
{
    QFile f(filename);
    if ( f.open(f.ReadOnly) )
    {
        char buf[8192];

        while ( f.read(buf, sizeof(buf)) )
        { }

        f.close();
    }
}
