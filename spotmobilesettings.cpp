#include "spotmobilesettings.h"
#include "ui_spotmobilesettings.h"
#include <QSettings>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QStringList>
#include <QMessageBox>

SpotMobileSettings::SpotMobileSettings(QSettings *qs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpotMobileSettings),
    s(qs)
{
    ui->setupUi(this);

    QList<QHostAddress> ips = QNetworkInterface::allAddresses();
    QStringList iplist;
    foreach (QHostAddress ip, ips)
    {
        iplist << ip.toString();
    }
    qSort(iplist);
    ui->listenAddrCombo->addItems(iplist);

    for (int i=0; i<iplist.count(); i++)
    {
        if (iplist[i] != "127.0.0.1" && iplist[i] != "0:0:0:0:0:0:0:1" && !iplist[i].startsWith("FE80"))
        {
            ui->listenAddrCombo->setCurrentIndex(i);
            break;
        }
    }

    if ( s->value("spotMobileAddress").isValid() )
    {
        int idx = ui->listenAddrCombo->findText( s->value("spotMobileAddress").toString() );
        if (idx != -1)
            ui->listenAddrCombo->setCurrentIndex(idx);
        ui->portEdit->setText( s->value("spotMobilePort").toString() );
        ui->userEdit->setText( s->value("spotMobileUser").toString() );
        ui->passEdit->setText( QByteArray::fromBase64(s->value("spotMobilePass").toByteArray() ));
        ui->checkBox->setChecked( s->value("spotMobileEnabled").toBool() );
        on_checkBox_clicked(ui->checkBox->isChecked() );
    }
}

SpotMobileSettings::~SpotMobileSettings()
{
    delete ui;
}

void SpotMobileSettings::on_checkBox_clicked(bool checked)
{
    ui->listenAddrCombo->setEnabled(checked);
    ui->portEdit->setEnabled(checked);
    ui->userEdit->setEnabled(checked);
    ui->passEdit->setEnabled(checked);
}

void SpotMobileSettings::accept()
{
    QByteArray user = ui->userEdit->text().trimmed().toLatin1();
    QByteArray pass = ui->passEdit->text().trimmed().toLatin1().toBase64();
    QByteArray addr = ui->listenAddrCombo->currentText().toLatin1();
    bool enabled = ui->checkBox->isChecked();
    int port = ui->portEdit->text().toInt();

    if (port < 1 || port > 65535)
    {
        QMessageBox::critical(this, tr("Poort?"), tr("Poort nummer ongeldig"));
        ui->portEdit->setFocus();
        return;
    }

    s->setValue("spotMobileUser", user);
    s->setValue("spotMobilePass", pass);
    s->setValue("spotMobileAddress", addr);
    s->setValue("spotMobilePort", port);
    s->setValue("spotMobileEnabled", enabled);
    s->sync();

    QDialog::accept();
}
