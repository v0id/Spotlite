#ifndef SPOTREPLYWINDOW_H
#define SPOTREPLYWINDOW_H

#include <QDialog>
#include <QTime>

class QListWidgetItem;
class HashCash;
class SpotLite;

namespace Ui {
    class SpotReplyWindow;
}
class SpotNNTP;

class SpotReplyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SpotReplyWindow(int spotid, SpotLite *sl, const QByteArray &msgid, const QString &title, QWidget *parent = 0);
    ~SpotReplyWindow();

signals:
    void newComment(const QByteArray &msg);

protected:
    Ui::SpotReplyWindow *ui;
    int _spotid;
    HashCash *_hc;
    QTime _timer;
    SpotLite *_sl;
    QByteArray _parentmsgid, _parentdomain, _from, _msgid, _msg, _newsgroup, _subjectPrefix;
    QString _title;
    SpotNNTP *_nntp;
    bool seperateConnection;

protected slots:
    void on_smileys_itemClicked(QListWidgetItem* item);
    void accept();
    void onComputed();

    void onError(int error, const QString &msg);
    void onLoggedIn();
    void onPostingComplete();
};

#endif // SPOTREPLYWINDOW_H
