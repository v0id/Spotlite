#ifndef ZIP_H
#define ZIP_H

#include <QObject>
#include <QFile>
#include <QStringList>

class Zip : public QObject
{
    Q_OBJECT
public:
    explicit Zip(QString filename, QObject *parent = 0);
    ~Zip();
    QStringList filenames();
    void extract(const QString &folder);
    bool hasCorrectHeader();

signals:

public slots:

protected:
    QFile _file;
};

#endif // ZIP_H
