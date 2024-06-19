#ifndef YENCMEMDECODER_H
#define YENCMEMDECODER_H

#include <QObject>
#include <QFile>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtDebug>

#include "crc.h"

class YencMemDecoder : public QObject
{
    Q_OBJECT
public:
    explicit YencMemDecoder(QObject *parent = 0);
    void clear();
    void dataReceived(QByteArray &data);
    QByteArray &decodedData();
    inline bool ok() const
    {
        return _ok;
    }

protected:
    QByteArray buf, linebuf;

    bool _esc, _lf;
    enum {none, prolog, maindata, epilog } _state;
    QFile *_file;
    uint32_t _crc;
    QByteArray _output;
    quint64 _bytes;
    bool _ok;

signals:

public slots:

};

#endif // YENCMEMDECODER_H
