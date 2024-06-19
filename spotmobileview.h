#ifndef SPOTMOBILEVIEW_H
#define SPOTMOBILEVIEW_H

#include "spotview.h"

class SpotMobileHandler;

class SpotMobileView : public SpotView
{
    Q_OBJECT
public:
    explicit SpotMobileView(SpotLite *sl, SpotMobileHandler *sh, int spotid, const QString &skinpath, QObject *parent = 0);

protected:
    SpotMobileHandler *_sh;

    virtual void _initWebbrowser();
    virtual void _displayHTML(const QByteArray &html, bool final = true);

signals:

public slots:

};

#endif // SPOTMOBILEVIEW_H
