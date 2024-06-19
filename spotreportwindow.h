#ifndef SPOTREPORTWINDOW_H
#define SPOTREPORTWINDOW_H

#include "spotreplywindow.h"


class SpotReportWindow : public SpotReplyWindow
{
    Q_OBJECT
public:
    explicit SpotReportWindow(int spotid, SpotLite *sl, const QByteArray &msgid, const QString &title, QWidget *parent = 0);

signals:

protected slots:
    void accept();
};

#endif // SPOTREPORTWINDOW_H
