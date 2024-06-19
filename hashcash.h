#ifndef HASHCASH_H
#define HASHCASH_H

#include <QThread>

class HashCash : public QThread
{
    Q_OBJECT
public:
    explicit HashCash(QObject *parent = 0);
    void calculate(const QByteArray &ref);
    QByteArray stamp();
    QByteArray header();
    static bool valid(const QByteArray &stamp, const QByteArray &ref);

signals:
    void computed();

public slots:

protected:
    virtual void run();
    QByteArray _ref, _stamp;
    static const int DEFAULT_BITS = 22;

};

#endif // HASHCASH_H
