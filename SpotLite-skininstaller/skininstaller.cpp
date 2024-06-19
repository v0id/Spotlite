#include "skininstaller.h"
#include "zip.h"
#include <stdexcept>
#include <QProcess>
#include <QCoreApplication>

SkinInstaller::SkinInstaller(const QString &filename, QObject *parent) :
    QObject(parent), _filename(filename)
{
    Zip zip(filename);
    bool has_screenshot = false;
    QString skinfolder;
    QStringList allowedfiletypes;
    allowedfiletypes << "tpl" << "png" << "jpg" << "jpeg" << "gif" << "html" << "txt" << "css" << "js" << "ico" << "ini";

    QStringList filenames = zip.filenames();
    if ( filenames.isEmpty() )
        throw std::runtime_error("Skin bestand corrupt/leeg");

    foreach (QString filename, filenames)
    {
        if (filename.endsWith('/'))
        {
            if (skinfolder.isEmpty() )
            {
                skinfolder = filename;
                continue;
            }
        }
        else
        {
            int pos = filename.lastIndexOf('.');
            if (pos != -1)
            {
                QString ext = filename.mid(pos+1).toLower();
                if ( !allowedfiletypes.contains(ext) )
                    throw std::runtime_error( QString("Bestandstype '%1' niet toegestaan. (bestand '%2')").arg(ext, filename).toLatin1().constData() );
            }
        }

        if (!filename.contains('/'))
        {
            throw std::runtime_error("Alle bestanden moeten in de submap 'naamvanskin' staan");
        }
        if ( !filename.startsWith(skinfolder) )
        {
            throw std::runtime_error("Maximaal 1 skin/directory in bestand toegestaan");
        }

        if ( filename == skinfolder+"screenshot.png")
            has_screenshot = true;
    }

    if (!has_screenshot)
        throw std::runtime_error("Skin mist screenshot.png");

    _skinname = skinfolder.left( skinfolder.length()-1 );

    if (_skinname.toLower() == "standaard" )
        throw std::runtime_error("Skin mag niet de naam 'standaard' hebben");
}

QString SkinInstaller::skinName() const
{
    return _skinname;
}

/* Should be somewhere else? */
inline QString skinFolder()
{
    QString s = "/usr/local/share/SpotLite/skins";

    if ( !QFile::exists(s))
    {
        s = QCoreApplication::applicationDirPath()+"/skins";
    }

    return s;
}

void SkinInstaller::install()
{
    Zip zip(_filename);
    zip.extract( skinFolder() );
}

void SkinInstaller::sudoInstall()
{
#ifdef Q_OS_MAC
    install();
#else

#ifdef Q_OS_WIN
/*    QProcess proc;
    proc.start(QCoreApplication::applicationDirPath()+"/SpotLite-skininstaller.exe", QStringList() << _filename);
    proc.waitForFinished(-1); */
#else
    QStringList sudoprograms;
    sudoprograms << "/usr/bin/kdesudo" << "/usr/local/bin/kdesudo" << "/usr/bin/gksu" << "/usr/local/bin/gksu";

    foreach (QString prog, sudoprograms)
    {
        if (QFile::exists(prog))
        {
            QProcess proc;
            proc.start(prog, QStringList() << QCoreApplication::applicationDirPath()+"/SpotLite-skininstaller" <<  _filename); // changeme
            proc.waitForFinished(-1);

            return;
        }
    }

    throw std::runtime_error("Zowel Kdesudo als gksu is niet geinstalleerd");

#endif
#endif
}
