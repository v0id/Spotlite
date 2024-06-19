#include "hashcash.h"
#include "hashcash/hashcash.h"

#include <QDebug>
#include <stdexcept>


HashCash::HashCash(QObject *parent) :
    QThread(parent)
{
}

void HashCash::run()
{
    char *stamp = hashcash_simple_mint(_ref, DEFAULT_BITS, 0, NULL, 1);
    _stamp = QByteArray(stamp);
    hashcash_free(stamp);

    emit computed();
}

void HashCash::calculate(const QByteArray &ref)
{
    if ( isRunning() )
        throw std::runtime_error("Already calculating");

    _ref = ref;
    start();
}

QByteArray HashCash::stamp()
{
    return _stamp;
}

QByteArray HashCash::header()
{
    /*char *hdr = hashcash_make_header(_stamp.constData(), 1000, NULL, 0, 0);
    QByteArray result(hdr);
    hashcash_free(hdr);
    return result;
    */

    return "X-Hashcash: "+_stamp+"\r\n";
}

bool HashCash::valid(const QByteArray &stamp, const QByteArray &ref)
{
    int result = hashcash_check(stamp.constData(), false, ref.constData(), NULL, NULL, TYPE_STR, 0, 0, 0, DEFAULT_BITS, NULL);

    switch (result)
    {
    case HASHCASH_INVALID:
        qDebug() << "Hashcash invalid";
        break;
    case HASHCASH_UNSUPPORTED_VERSION:
        qDebug() << "Hashcash unsupported version";
        break;
    case HASHCASH_WRONG_RESOURCE:
        qDebug() << "Hashcash wrong resource";
        break;
    case HASHCASH_REGEXP_ERROR:
        qDebug() << "Hashcash regexp error";
        break;
    case HASHCASH_INSUFFICIENT_BITS:
        qDebug() << "Hashcash insufficient bits";
        break;
    default:
        break;
    }

    return (result >= 0);
}
