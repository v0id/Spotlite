#ifndef ADDRESSDIALOG_H
#define ADDRESSDIALOG_H

#include <QDialog>

namespace Ui {
    class AddressDialog;
}
class SpotLite;
class QSqlTableModel;

class AddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressDialog(SpotLite *sl, QWidget *parent = 0);
    ~AddressDialog();

protected:
    Ui::AddressDialog *ui;
    SpotLite *_sl;
    QSqlTableModel *_friendsModel, *_foesModel;

private slots:
    void on_tableView_2_clicked();
    void on_tableView_clicked();
    void on_delButton_clicked();
};

#endif // ADDRESSDIALOG_H
