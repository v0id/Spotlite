#include <QtCore/QCoreApplication>
#include <stdexcept>
#include <QDebug>
#include <QStringList>
#include <QProcess>

#include "skininstaller.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try
    {
        QStringList args = a.arguments();

        foreach (QString arg, args)
        {
            if (arg.endsWith(".spotskin", Qt::CaseInsensitive))
            {
                qDebug() << "Bezig met installeren van" << arg;
                SkinInstaller ski(arg);
                ski.install();
                qDebug() << "Klaar...";
#ifdef Q_OS_WIN
                QProcess proc;
                proc.startDetached(QCoreApplication::applicationDirPath()+"/SpotLite.exe", QStringList() << arg);
#endif
            }
        }

    }
    catch (std::runtime_error re)
    {
        qDebug() << "FOUT:" << re.what();
    }
}
