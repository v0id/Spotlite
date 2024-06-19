#include "addressdialog.h"
#include <QToolBar>
#include "ui_addressdialog.h"
#include "spotlite.h"
#include <QtSql>


AddressDialog::AddressDialog(SpotLite *sl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressDialog), _sl(sl)
{
    ui->setupUi(this);

   _friendsModel = new QSqlTableModel(this, *_sl->spotDatabase() );
   _friendsModel->setTable("friends");
   _friendsModel->sort(1, Qt::AscendingOrder);
   _friendsModel->select();
   _friendsModel->setHeaderData(0, Qt::Horizontal, tr("Gebruiker ID"));
   _friendsModel->setHeaderData(1, Qt::Horizontal, tr("Gebruikersnaam"));
   ui->tableView->setModel(_friendsModel);
   ui->tableView->horizontalHeader()->setStretchLastSection(true);

   _foesModel = new QSqlTableModel(this, *_sl->spotDatabase() );
   _foesModel->setTable("foes");
   _foesModel->sort(1, Qt::AscendingOrder);
   _foesModel->select();
   _foesModel->setHeaderData(0, Qt::Horizontal, tr("Gebruiker ID"));
   _foesModel->setHeaderData(1, Qt::Horizontal, tr("Gebruikersnaam"));
   _foesModel->removeColumn(2);
   _foesModel->select();
   ui->tableView_2->setModel(_foesModel);
   ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
}

AddressDialog::~AddressDialog()
{
    delete ui;
}

void AddressDialog::on_delButton_clicked()
{
    _sl->spotDatabase()->transaction();
    QSqlQuery q(*_sl->spotDatabase());

    if ( ui->tabWidget->currentIndex() == 0 )
    {
        q.prepare("DELETE FROM friends WHERE userid=?");
        q.addBindValue( ui->tableView->model()->index( ui->tableView->currentIndex().row(), 0 ).data() );
    }
    else
    {
        q.prepare("DELETE FROM foes WHERE userid=?");
        q.addBindValue( ui->tableView_2->model()->index( ui->tableView_2->currentIndex().row(), 0 ).data() );
    }

    q.exec();
    _sl->spotDatabase()->commit();
    _sl->reloadFriendsAndFoes();

    _friendsModel->select();
    _foesModel->select();
}

void AddressDialog::on_tableView_clicked()
{
    ui->delButton->setEnabled(true);
}

void AddressDialog::on_tableView_2_clicked()
{
    ui->delButton->setEnabled(true);
}
