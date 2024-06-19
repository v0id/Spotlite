#ifndef CACHEWARMER_H
#define CACHEWARMER_H

#include <QThread>
#include <QStringList>

class CacheWarmer : public QThread
{
    Q_OBJECT
public:
    explicit CacheWarmer(QObject *parent = 0);
    void addFile(const QString &filename);

protected:
    QStringList _files;

    virtual void run();
    void _warmCache(const QString &filename);

signals:

public slots:

};

#endif // CACHEWARMER_H
