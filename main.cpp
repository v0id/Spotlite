#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include "mainwindow.h"

#include <QtGui/QFileOpenEvent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("SpotLite");
    a.setApplicationName("SpotLite");
    QTranslator _qttrans;
    _qttrans.load("qt_nl.qm", /*QLibraryInfo::location(QLibraryInfo::TranslationsPath)*/ a.applicationDirPath() );
    QApplication::installTranslator(&_qttrans);
    MainWindow w;
    w.show();

    QStringList args = a.arguments();
    if ( args.count() > 1)
    {
        foreach (QString arg, args)
        {
            w.openFile(arg);
        }
    }


    return a.exec();
}
