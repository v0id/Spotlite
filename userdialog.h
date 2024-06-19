#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>

namespace Ui {
    class UserDialog;
}

class SpotLite;

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(SpotLite *sl, const QByteArray &userid, const QByteArray &username, QWidget *parent = 0);
    ~UserDialog();

private:
    Ui::UserDialog *ui;
    SpotLite *_sl;
    QByteArray _userid, _username;

private slots:
    void on_banButton_clicked();
    void on_friendButton_clicked();
};

#endif // USERDIALOG_H
