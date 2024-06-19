#include "zip.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QBuffer>
#include <QTextStream>
#include <QDebug>
#include <QDataStream>
#include <qtioprocessor/qtiocompressor.h>
#include <stdexcept>

Zip::Zip(QString filename, QObject *parent) :
    QObject(parent), _file(filename)
{
    if (!_file.open(QIODevice::ReadOnly))
        throw std::runtime_error("Error opening file");
}

Zip::~Zip()
{
    _file.close();
}

QStringList Zip::filenames()
{
    QStringList files;
    _file.seek(0);

    forever {
         // Zip format "local file header" fields:
         quint32 signature, crc, compSize, unCompSize;
         quint16 extractVersion, bitFlag, compMethod, modTime, modDate;
         quint16 nameLen, extraLen;

         QDataStream s(&_file);
         s.setByteOrder(QDataStream::LittleEndian);
         s >> signature;
         if (signature != 0x04034b50)   // zip local file header magic number
             break;
         s >> extractVersion >> bitFlag >> compMethod;
         s >> modTime >> modDate >> crc >> compSize >> unCompSize;
         s >> nameLen >> extraLen;

         files.append(_file.read(nameLen).replace('\\', "/"));
         _file.read(extraLen);

         _file.seek( _file.pos() + compSize);

         /*QByteArray compData = file.read(compSize);
         QByteArray unCompData;
         if (compMethod == 0) {
             unCompData = compData;
         }
         else {
             QBuffer compBuf(&compData);
             QtIOCompressor compressor(&compBuf);
             compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
             compressor.open(QIODevice::ReadOnly);
             unCompData = compressor.readAll();
         }

         // unCompData now contains the uncompressed file from the zip archive
         sout << QString("%1 %2 ").arg(1+item++, 3).arg(unCompData.size(), 6)
              << fileName << endl;

         if (fileName.toLower().endsWith(".txt"))
             sout << "   Preview: \""
                  << unCompData.mid(0, unCompData.indexOf('\n')).replace('\r', "")
                  << "\"..." << endl;
        */
     }

    return files;
}

bool Zip::hasCorrectHeader()
{
    _file.seek(0);
    return (_file.peek(4) == "PK\3\4");
}

void Zip::extract(const QString &folder)
{
    QFileInfo fi(folder);
    if ( !fi.exists() )
        throw std::runtime_error("Destination folder does not exist");
    QString absfolder = fi.canonicalFilePath()+"/";
    _file.seek(0);

    forever {
         // Zip format "local file header" fields:
         quint32 signature, crc, compSize, unCompSize;
         quint16 extractVersion, bitFlag, compMethod, modTime, modDate;
         quint16 nameLen, extraLen;

         QDataStream s(&_file);
         s.setByteOrder(QDataStream::LittleEndian);
         s >> signature;
         if (signature != 0x04034b50)   // zip local file header magic number
             break;
         s >> extractVersion >> bitFlag >> compMethod;
         s >> modTime >> modDate >> crc >> compSize >> unCompSize;
         s >> nameLen >> extraLen;

         QByteArray filename = _file.read(nameLen).replace('\\', "/");
         if (filename.contains("..") || filename.startsWith("/"))
             throw std::runtime_error("Invalid characters in filename");
         _file.read(extraLen);

         if (filename.endsWith('/'))
         {
            /* folder */
            filename.chop(1);
            QDir dir;
            dir.mkdir(absfolder+filename);
            //QFile::setPermissions(absfolder+filename, QFile::Permissions(0755));
            continue;
         }

         QFile outputfile(absfolder+filename);
         if (!outputfile.open(outputfile.WriteOnly))
             throw std::runtime_error("Error opening output file");
         //outputfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);

         /* TODO: do not read entire file into memory? */
         QByteArray compData = _file.read(compSize);
         QByteArray unCompData;
         if (compMethod == 0) {
             unCompData = compData;
         }
         else {
             QBuffer compBuf(&compData);
             QtIOCompressor compressor(&compBuf);
             compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
             compressor.open(QIODevice::ReadOnly);
             unCompData = compressor.readAll();
         }
         outputfile.write(unCompData);
         outputfile.close();
     }
}
