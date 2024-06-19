#include "customfilter.h"
#include <QDebug>

CustomFilter::CustomFilter(int cat)
    : _cat(cat), _sort(0)
{
}

CustomFilter::CustomFilter(const CustomFilter &f)
    : _name(f._name), _title(f._title),  _spotter(f._spotter), _tag(f._tag), _options(f._options), _cat(f._cat), _sort(f._sort)
{
}

CustomFilter::CustomFilter(const QString &name, const QString &title, const QString &spotter, const QString &tag, QSet<QByteArray> options, int cat, int sort)
    : _name(name), _title(title), _spotter(spotter), _tag(tag), _options(options), _cat(cat), _sort(sort)
{

}

CustomFilter& CustomFilter::operator=(const CustomFilter &f)
{
    _name = f._name;
    _title = f._title;
    _spotter = f._spotter;
    _tag = f._tag;
    _options = f._options;
    _cat = f._cat;
    _sort = f._sort;
    return *this;
}

CustomFilter::CustomFilter(int cat, int subcat, const QString &name)
    : _name(name), _cat(cat), _sort(0)
{
    _options.insert("a"+(subcat < 10 ? "0"+QByteArray::number(subcat) : QByteArray::number(subcat)) );
}

QDataStream& operator<<(QDataStream& out, const CustomFilter &f)
{
    out << f._name << f._cat << f._sort << f._options << f._title << f._spotter << f._tag;
    return out;
}

QDataStream& operator>>(QDataStream& in, CustomFilter &f)
{
    in >> f._name;
    in >> f._cat;
    in >> f._sort;
    in >> f._options;
    in >> f._title;
    in >> f._spotter;
    in >> f._tag;
    return in;
}

inline QByteArray _esc(QString in)
{
    return in.toLatin1().replace("'", "''");
}

QByteArray CustomFilter::query(const QByteArray &extraCondition, const QByteArray &orderBy)
{
    QByteArray q;
    QList<int> subcats;
    QList<QByteArray> othercats;
    QList<QByteArray> cond;

    foreach (QByteArray c, _options)
    {
        if (c.startsWith('a'))
        {
            subcats.append( _cat*100+c.mid(1).toInt() );
        }
        else
        {
            othercats.append(c);
        }
    }

    /* Categories and sub categories */
    if ( subcats.isEmpty() )
    {
        cond.append("cat="+QByteArray::number(_cat));
    }
    else
    {
        if ( subcats.count() == 1 )
            cond.append("subcat="+QByteArray::number(subcats.first() ));
        else
        {
            QByteArray s;
            foreach (int subcat, subcats)
            {
                if ( !s.isEmpty() )
                    s += ",";
                s += QByteArray::number(subcat);
            }
            cond.append("subcat IN ("+s+")");
        }
    }

    /* Other categories */
    if ( !othercats.isEmpty() )
    {
        QByteArray s;
        char last = 0;
        qSort(othercats);

        foreach (QByteArray othercat, othercats)
        {
            if ( !s.isEmpty() )
            {
                if (last != othercat[0]) /* different category */
                {
                    if (s.contains("OR"))
                        s = "("+s+")";
                    cond.append(s);
                    s = "";
                }
                else
                    s += " OR ";
            }
            //s += "othercats LIKE '%"+othercat+"%'";
            s += "othercats LIKE '"+othercat+"'";
            last = othercat[0];
        }
        if (s.contains("OR"))
            s = "("+s+")";

        cond.append(s);
    }

    /* Title */
    if ( !_title.isEmpty() )
    {
        cond.append("title LIKE '%"+_esc(_title)+"%'");
    }

    /* Spotter */
    if ( !_spotter.isEmpty() )
    {
        cond.append("spotter LIKE '"+_esc(_spotter)+"'");
    }

    /* Tag */
    if ( !_tag.isEmpty() )
    {
        cond.append("tag LIKE '"+_esc(_tag)+"'");
    }

    if ( !extraCondition.isEmpty() )
    {
        cond.append(extraCondition);
    }

    foreach (QByteArray part, cond)
    {
        if ( !q.isEmpty() )
            q += " AND ";
        q += part;
    }

    q += " ORDER BY ";

    if (!orderBy.isEmpty() )
    {
        q+= orderBy;
    }
    else
    {
        switch (_sort)
        {
        case 1:
            q += "spotdate DESC";
            break;
        case 2:
            q += "subcat";
            break;

        case 3:
            q += "title";
            break;

        case 4:
            q += "spotter";
            break;

        default:
            q += "id DESC";
        }
    }

    return q;
}
