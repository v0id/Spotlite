#ifndef CUSTOMFILTER_H
#define CUSTOMFILTER_H

#include <QSet>
#include <QMetaType>
#include <QVariant>
#include <QDataStream>

class CustomFilter
{

public:
    explicit CustomFilter(int cat = 1);
    CustomFilter(const QString &name, const QString &title, const QString &spotter, const QString &tag, QSet<QByteArray> options, int cat, int sort);
    CustomFilter(const CustomFilter &f);
    CustomFilter& operator=(const CustomFilter &f);
    CustomFilter(int cat, int subcat, const QString &name);
    QByteArray query(const QByteArray &extraCondition = "", const QByteArray &orderBy = "");

    inline QString name() const
    {
         return _name;
    }

     inline int cat() const
     {
         return _cat;
     }

     inline QSet<QByteArray> &options()
     {
         return _options;
     }

     inline QString title() const
     {
         return _title;
     }

     inline QString spotter() const
     {
         return _spotter;
     }

     inline QString tag() const
     {
         return _tag;
     }

     inline void setName(const QString &name)
     {
         _name = name;
     }

     inline void setCat(int cat)
     {
         _cat = cat;
     }

     inline void setOptions(const QSet<QByteArray> options)
     {
         _options = options;
     }

     inline void setTitle(const QString &title)
     {
         _title = title;
     }

     inline void setTag(const QString &tag)
     {
         _tag = tag;
     }

     inline void setSpotter(const QString &spotter)
     {
         _spotter = spotter;
     }

     inline int sort() const
     {
         return _sort;
     }

     inline bool isStandardFilter() const
     {
         return _title.isEmpty() && _tag.isEmpty() && _spotter.isEmpty() && (_options.isEmpty() || (_options.count() == 1 && _options.begin()->startsWith("a") ));
     }

    inline operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

    friend QDataStream& operator<<(QDataStream& out, const CustomFilter &f);
    friend QDataStream& operator>>(QDataStream& in, CustomFilter &f);

protected:
    QString _name, _title, _spotter, _tag;
    QSet<QByteArray> _options;
    int _cat, _sort;

signals:

public slots:

};
Q_DECLARE_METATYPE(CustomFilter)

#endif // CUSTOMFILTER_H
