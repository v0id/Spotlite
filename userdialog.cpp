#include "userdialog.h"
#include "ui_userdialog.h"
#include "spotlite.h"
#include "spotsignature.h"
#include <QMessageBox>

UserDialog::UserDialog(SpotLite *sl, const QByteArray &userid, const QByteArray &username, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDialog),
    _sl(sl),
    _userid(userid), _username(username)
{
    ui->setupUi(this);
    ui->usernameLabel->setText( ui->usernameLabel->text()+_username);

    QByteArray multiline_userid;

    for (int i=0; i<_userid.length(); i+= 20)
    {
        multiline_userid += _userid.mid(i, 20)+"\n";
    }

    ui->useridLabel->setPlainText(multiline_userid);
    ui->crcLabel->setText(ui->crcLabel->text()+SpotSignature::crc32base64(_userid));

    if ( _userid.isEmpty() )
    {
        ui->friendButton->setEnabled(false);
        ui->banButton->setEnabled(false);
        ui->useridLabel->setPlainText( tr("*niet een Spot gebruiker*"));
    }
}

UserDialog::~UserDialog()
{
    delete ui;
}


void UserDialog::on_friendButton_clicked()
{
    _sl->addFriend(_userid, _username);
    done(Accepted);
}

void UserDialog::on_banButton_clicked()
{
    _sl->banUser(_userid, _username);
    done(Accepted);
}
