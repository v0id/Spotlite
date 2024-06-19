#include "uisettingsdialog.h"
#include "ui_uisettingsdialog.h"
#include <QFont>
#include <QFontDialog>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QSettings>

#include "mainwindow.h"

UISettingsDialog::UISettingsDialog(QSettings *s, const QString &skinToSelect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UISettingsDialog),
    _listFont("Ubuntu", 9),
    _spotFont("Ubuntu", 12),
    _s(s)
{
    _listFont.fromString(_s->value("listfont").toString() );
    _spotFont.fromString(_s->value("spotfont").toString() );
    ui->setupUi(this);
    ui->listFontinfo->setText( _fontToString(_listFont) );
    ui->spotFontinfo->setText( _fontToString(_spotFont) );

    ui->singleClickCombo->setCurrentIndex(_s->value("singleclickaction", 1).toInt() );
    ui->doubleClickCombo->setCurrentIndex(_s->value("doubleclickaction", 2).toInt() );

    QString currentskin = skinToSelect;
    if ( currentskin.isEmpty() )
        currentskin = _s->value("skin", "standaard").toString();
    QString skinpath = MainWindow::skinFolder();

    QDirIterator iter(skinpath, QDir::Dirs | QDir::NoDotAndDotDot );
    while (iter.hasNext())
    {
        QString screenshotfile = iter.next()+"/screenshot.png";
        QString name           = iter.fileName();
        QListWidgetItem *item  = new QListWidgetItem(name, ui->skinsList);
        if ( QFile::exists(screenshotfile) )
            item->setIcon( QIcon(screenshotfile) );
        if (currentskin == name)
            ui->skinsList->setCurrentItem(item);
    }
}

UISettingsDialog::~UISettingsDialog()
{
    delete ui;
}

void UISettingsDialog::on_listFontButton_clicked()
{
    bool ok;

    _listFont = QFontDialog::getFont(&ok, _listFont);
    ui->listFontinfo->setText( _fontToString(_listFont) );
}

void UISettingsDialog::on_spotFontButton_clicked()
{
    bool ok;

    _spotFont = QFontDialog::getFont(&ok, _spotFont);
    ui->spotFontinfo->setText( _fontToString(_spotFont) );
}


QString UISettingsDialog::_fontToString(QFont f)
{
    return QString("%1 (%2 pt)").arg(f.family(), QString::number(f.pointSize()) );
}

void UISettingsDialog::accept()
{
    _s->setValue("listfont", _listFont.toString() );
    _s->setValue("spotfont", _spotFont.toString() );
    _s->setValue("singleclickaction", ui->singleClickCombo->currentIndex() );
    _s->setValue("doubleclickaction", ui->doubleClickCombo->currentIndex() );
    _s->setValue("skin", ui->skinsList->currentItem()->text() );
    _s->sync();
    QDialog::accept();
}
