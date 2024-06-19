#include "yencmemdecoder.h"

// #include <zlib.h> /* zlib has CRC32 */

#include <stdexcept>


YencMemDecoder::YencMemDecoder(QObject *parent) :
    QObject(parent)
{
    clear();
}

void YencMemDecoder::clear()
{
    _output.clear();
    linebuf.clear();
    buf.clear();

    _esc = false;
    _lf  = true;
    _state = prolog;
    _crc = crc32_begin();
    linebuf = "\n";
    _bytes = 0;
    _ok = false;
}

void YencMemDecoder::dataReceived(QByteArray &data)
{
    if ( !linebuf.isEmpty() )
    {
        data.prepend(linebuf);
        linebuf.clear();
    }

    int outoff = 0;
    const char *c   = data.constData();
    int len = data.length();
    if (len > buf.capacity() )
    {
        buf.clear();
        buf.reserve(len);
    }
    char *out = buf.data();

    for (int i=0; i<len; i++)
    {
        register char ch = c[i];
        switch (ch)
        {
            case '\n':
            /* BEGIN LF handling */
            if (i+3 > len)
            {
                linebuf = c+i;
                i = len;
                break;
            }

            ch = c[i+1];
            if (ch == '.' && c[i+2] == '.') // .. => .
            {
                i++;
                break;
            }
            else if (ch == '=' && c[i+2] == 'y')
            {
                i++;
                char *poslf = (char *) strchr(c+i, '\n');

                if ( !poslf )
                {
                    linebuf = data.mid(i-1);
                    i = len;
                    break;
                }

                *poslf = 0; // Null-terminate string

                if ( !strncmp(c+i, "=ybegin ", 8) )
                {
/*
                    const char *namestr = strstr(c+i, " name=");
                    const char *sizestr = strstr(c+i, " size=");
                    char *sizeendstr = 0;

                    if (sizestr)
                        sizeendstr = (char *) strchr(sizestr+1, ' ');

                    if (!namestr)
                        break; */

                    _state = maindata; outoff = 0;
                }
                else if ( !strncmp(c+i, "=ypart ", 7))
                {
                }
                else if ( !strncmp(c+i, "=yend", 5))
                {
                    const char *pcrcstr = strstr(c+i, "pcrc32=");
                    if (!pcrcstr)
                        pcrcstr = strstr(c+i, " crc32=");
                    const char *sizestr = strstr(c+i, " size=");
                    char *pcrcendstr = 0;
                    char *sizeendstr = 0;

                    if (pcrcstr)
                        pcrcendstr = (char *) strchr(pcrcstr+1, ' ');
                    if (sizestr)
                        sizeendstr = (char *) strchr(sizestr+1, ' ');

                    if (pcrcendstr)
                        *pcrcendstr = 0;
                    if (sizeendstr)
                        *sizeendstr = 0;

                    if (outoff)
                    {
                        _output.append(out, (qint64) outoff);
                        _bytes += outoff;
                        //_crc = crc32( _crc, (Bytef *) out, outoff );
                        _crc = crc32_data(_crc, out, outoff);
                    }
                    _crc = crc32_end(_crc);

                    if (/*sizestr && pcrcstr && _bytes == QString(sizestr+6).toULongLong() &&*/ QString(pcrcstr+7).toULong(0,16) == _crc )
                    {
//                            qDebug() << "Size & CRC matches!";
                        _ok = true;
                    }
                    else
                    {
                        qDebug() << "CRC onjuist :-(";
                        qDebug() << _bytes << _output.length();
                        if (pcrcstr)
                            qDebug() << "CRC volgens header" << QString(pcrcstr+7).toULong(0,16) << "Berekende CRC" << _crc;
                    }

                    _state = epilog;

                    return;
                }

                i = poslf - c - 2;
                *poslf = '\n';
                //qDebug() << "X: " << c[i] << "Y: " << c[i+1];
            }
            /* END */
            break;

            case '\r':
            break;

            case '=':
                ch = c[++i];
                if (!ch)
                {
                    linebuf = "=";
                    break;
                }
                ch = ((ch - 64) & 255);

            // fall through

            default:
                out[outoff++] = ((ch - 42) & 255);
        }
    }

    if (outoff && _state == maindata )
    {
        _output.append(out, (qint64) outoff);

        _bytes += outoff;
//        _crc = crc32( _crc, (Bytef *) out, outoff );
        _crc = crc32_data(_crc, out, outoff);
    }
}

QByteArray &YencMemDecoder::decodedData()
{
    return _output;
}

