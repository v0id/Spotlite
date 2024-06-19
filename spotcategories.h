#ifndef SPOTCATEGORIES_H
#define SPOTCATEGORIES_H

#include <QByteArray>
#include <QObject>

class SpotCategories : public QObject
{
public:
    static QString cat2name(int cat, const QByteArray &subcat);
};

#endif // SPOTCATEGORIES_H
