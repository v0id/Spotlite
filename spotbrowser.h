#ifndef SPOTBROWSER_H
#define SPOTBROWSER_H

#include <QWidget>

class QWebView;
class QUrl;
class QWidget;
class QNetworkRequest;
class QAbstractTableModel;
class QToolButton;



class SpotBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit SpotBrowser(QWidget *parent = 0);

protected:
    QWebView *_web;
    QToolButton *_prev, *_next, *_home, *_reload;

signals:

public slots:

};

#endif // SPOTBROWSER_H
