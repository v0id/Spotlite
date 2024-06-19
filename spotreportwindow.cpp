#include "spotreportwindow.h"
#include "ui_spotreplywindow.h"
#include "spotlite.h"
#include <QMessageBox>

SpotReportWindow::SpotReportWindow(int spotid, SpotLite *sl, const QByteArray &msgid, const QString &title, QWidget *parent) :
    SpotReplyWindow(spotid, sl, msgid, title, parent)
{
    ui->smileys->setVisible(false);
    _newsgroup = "free.willey";
    _subjectPrefix = "REPORT <"+_parentmsgid+"@"+_parentdomain+"> - ";
    setWindowTitle("Rapporteren - "+title);
}

void SpotReportWindow::accept()
{
    QVariant vl = _sl->settings()->value("reporthistory");
    QStringList list;

    if (vl.isValid() )
    {
        list = vl.toStringList();
        if (list.contains(_parentmsgid))
        {
            QMessageBox::critical(this, tr("Reeds gerapporteerd!"), tr("Deze spot is al eerder gerapporteerd..."), QMessageBox::Ok);
            close();
            return;
        }
    }

    SpotReplyWindow::accept();

    if (_hc)
    {
        list.append(_parentmsgid);
        _sl->settings()->setValue("reporthistory", list);
    }
}
