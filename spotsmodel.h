#ifndef SPOTSMODEL_H
#define SPOTSMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QHash>
#include <QPixmap>
#include "customfilter.h"


struct cached_spot
{
    int id, subcat, spotdate, spamcount;
    QByteArray subject, poster;
};
class SpotLite;
class QSqlDatabase;

class SpotsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SpotsModel(SpotLite *sl, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void setQuery(QByteArray query, int cat, int subcat);
    void setQuery(const CustomFilter &f);
    static QString subcategoryname(int subcat);
    void changeTheme(const QString &themedir);
    int iconWidth() const;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

signals:

public slots:
    void onNewSpots();
    void resetNewcount();

protected:
    static SpotLite *_sl;
    static int _oldid, _oldeid;
    static int _rows;
    static int _iconWidth;
    static int _mediumSpamThreshold, _highSpamThreshold;

    static QByteArray _query;
    static int _cat, _subcat;
    static QSqlDatabase *_db;
    static QList<cached_spot> _cache;
    static int _cachestart, _cacheend;
    static CustomFilter _filter;

    static bool _useIcons;
    static QHash<int,QPixmap> _icons;

    static bool _loadFromDb(int neededRow);
    void invalidateCache();
    static QByteArray  _orderBy;

    const static int CACHE_ITEMS;
};

#endif // SPOTSMODEL_H
