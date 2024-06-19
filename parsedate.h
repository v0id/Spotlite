#ifndef PARSEDATE_H
#define PARSEDATE_H

#include <time.h>
#include <QDateTime>
#include <QByteArray>
#include <QDebug>

/*
 * Parses a textual date string, returns the date
 */
extern "C" time_t parse_date(char *p, time_t *now);

inline QDateTime stringToQDateTime(QByteArray in)
{
    time_t out;

    out = parse_date(in.data(), NULL);
    return QDateTime::fromTime_t(out);
}

inline QByteArray normalizeDate(QByteArray in)
{
    QDateTime d = stringToQDateTime(in);
    int daysto = d.daysTo( QDateTime::currentDateTime() );

    if (daysto == 0)
        return "Vandaag, "+d.toString("hh:mm").toLatin1();
    else if (daysto == 1)
        return "Gisteren, "+d.toString("hh:mm").toLatin1();
    else
        return d.toString("d-M-yyyy hh:mm").toLatin1();
}

#endif // PARSEDATE_H
