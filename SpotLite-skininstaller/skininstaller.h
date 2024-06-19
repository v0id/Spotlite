#ifndef SKININSTALLER_H
#define SKININSTALLER_H

#include <QObject>

class SkinInstaller : public QObject
{
    Q_OBJECT
public:
    explicit SkinInstaller(const QString &filename, QObject *parent = 0);
    QString skinName() const;
    void install();
    void sudoInstall();

protected:
    QString _filename, _skinname;

signals:

public slots:

};

#endif // SKININSTALLER_H
