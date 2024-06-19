#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QSettings *qs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    s(qs)
{
    ui->setupUi(this);

    if (s->value("port").isValid() )
    {
        ui->serverEdit->setText( s->value("server").toString() );
        ui->userEdit->setText( s->value("user").toString() );
        ui->nickEdit->setText( s->value("nick").toString() );
        ui->passEdit->setText( QByteArray::fromBase64(s->value("pass").toByteArray() ));
        ui->portEdit->setText( s->value("port").toString() );
        ui->maxDays->setEditText(s->value("maxdays").toString() );
        ui->maxSpots->setEditText( s->value("maxspots").toString() );
        if ( s->value("images").toBool() )
            ui->imagesCheck->setChecked(true);
        if ( !s->value("nntpImages").toBool() )
            ui->nntpImagesCheck->setChecked(false);
        if ( s->value("ssl").toBool() )
            ui->sslBox->setChecked(true);
        if ( s->value("xzver").toBool() )
            ui->xzverBox->setChecked(true);
        ui->eroCheck->setChecked( s->value("ero").toBool() );
        ui->trashCheck->setChecked( s->value("showtrash").toBool() );
        ui->nntpSwitchCheck->setChecked( s->value("nntpswitch").toBool() );
        ui->meteenTonenRadio->setChecked( !s->value("updateknop").toBool() );
        ui->naUpdateRadio->setChecked( s->value("updateknop").toBool() );
        ui->spamCheck->setChecked( s->value("spamreports", true).toBool() );
        ui->updateOnStart->setEnabled( ui->naUpdateRadio->isChecked() );
        ui->updateOnStart->setChecked( s->value("updateonstart").toBool() );
        ui->spamDeleteCheck->setChecked( s->value("deletespam").toBool() );
        ui->spamDeleteCheck->setEnabled( ui->spamCheck->isChecked() );
        ui->filterHCCheck->setChecked( s->value("filterhc", true).toBool() );
        ui->filterManyCatCheck->setChecked( s->value("filtermanycat", true).toBool() );
        ui->warmcacheCheck->setChecked( !s->value("coldcache").toBool() );
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    QString server = ui->serverEdit->text().trimmed();
    QByteArray user = ui->userEdit->text().trimmed().toLatin1();
    QByteArray pass = ui->passEdit->text().trimmed().toLatin1().toBase64();
    QByteArray nick = ui->nickEdit->text().trimmed().toLatin1();
    int     port   = ui->portEdit->text().toInt();
    int     maxdays  = ui->maxDays->currentText().toInt();
    int     maxspots = ui->maxSpots->currentText().toInt();

    if (server.isEmpty())
    {
        QMessageBox::critical(this, tr("Server?"), tr("Voer de server van uw Usenet provider in") );
        ui->serverEdit->setFocus();
        return;
    }
    if (!pass.isEmpty() && user.isEmpty() )
    {
        QMessageBox::critical(this, tr("Gebruikersnaam?"), tr("Voer een gebruikersnaam in."));
        ui->userEdit->setFocus();
        return;
    }
    if (nick.isEmpty())
    {
        QMessageBox::critical(this, tr("Nickname?"), tr("Vul je schuilnaam in!"));
        ui->nickEdit->setFocus();
        return;
    }
    if (port < 1 || port > 65535)
    {
        QMessageBox::critical(this, tr("Poort?"), tr("Poort nummer ongeldig"));
        ui->portEdit->setFocus();
        return;
    }
    if (maxdays < 0)
    {
        QMessageBox::critical(this, tr("Max. aantal dagen ongeldig"), tr("Mag niet negatief zijn"));
        ui->maxDays->setFocus();
        return;
    }
    if (maxspots < 1)
    {
        QMessageBox::critical(this, tr("Max. aantal spots ongeldig"), tr("Moet positief zijn"));
        ui->maxSpots->setFocus();
        return;
    }

    if ( s->value("server").toString() != server )
    {
        s->setValue("lastheader", (int) 0);
        s->setValue("lastcommentsheader", (int) 0);
    }
    s->setValue("server", server);
    s->setValue("user", user);
    s->setValue("pass", pass);
    s->setValue("port", port);
    s->setValue("maxdays", maxdays);
    s->setValue("maxspots", maxspots);
    s->setValue("nick", nick);
    s->setValue("images", ui->imagesCheck->isChecked() );
    s->setValue("nntpImages", ui->nntpImagesCheck->isChecked() );
    s->setValue("ssl", ui->sslBox->isChecked() );
    s->setValue("xzver", ui->xzverBox->isChecked() );
    s->setValue("ero", ui->eroCheck->isChecked() );
    s->setValue("showtrash", ui->trashCheck->isChecked() );
    s->setValue("nntpswitch", ui->nntpSwitchCheck->isChecked() );
    s->setValue("updateknop", ui->naUpdateRadio->isChecked() );
    s->setValue("spamreports", ui->spamCheck->isChecked() );
    s->setValue("deletespam", ui->spamCheck->isChecked() && ui->spamDeleteCheck->isChecked() );
    s->setValue("updateonstart", ui->updateOnStart->isChecked() );
    s->setValue("filterhc", ui->filterHCCheck->isChecked() );
    s->setValue("filtermanycat", ui->filterManyCatCheck->isChecked() );
    s->setValue("coldcache", !ui->warmcacheCheck->isChecked() );

    QDialog::accept();
}

void SettingsDialog::on_imagesCheck_clicked(bool checked)
{
    if (checked)
        QMessageBox::information(this, tr("Waarschuwing!"), tr("LET OP: het laden van plaatjes van externe websites, kan gevolgen voor uw privacy hebben. De webmaster kan dan zien welk IP-adres het plaatje binnenhaalt."));
}

void SettingsDialog::on_sslBox_clicked(bool checked)
{
    if (checked)
        ui->portEdit->setText("443");
    else if (ui->portEdit->text() == "443" || ui->portEdit->text() == "563")
        ui->portEdit->setText("119");
}

void SettingsDialog::on_meteenTonenRadio_toggled(bool checked)
{
    ui->updateOnStart->setEnabled(!checked);
}

void SettingsDialog::on_spamCheck_toggled(bool checked)
{
    ui->spamDeleteCheck->setEnabled(checked);
}
