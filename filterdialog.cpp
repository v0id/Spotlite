#include "filterdialog.h"
#include "ui_filterdialog.h"
#include "spotcategories.h"
#include <QTreeWidgetItem>
#include <QMessageBox>

FilterDialog::FilterDialog(const CustomFilter &f, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);

    int t = f.cat()-1;
    if (t == 8)
        t = 4;
    ui->catCombo->setCurrentIndex(t);
    _f = f;
    _createOptionsTree(_f.cat() );
    ui->nameEdit->setText(_f.name());
    ui->sortCombo->setCurrentIndex(_f.sort());
    ui->spotterEdit->setText(_f.spotter());
    ui->tagEdit->setText(_f.tag());
    ui->titleEdit->setText(_f.title());
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

CustomFilter FilterDialog::filtersettings()
{
    QSet<QByteArray> options;

    for (int i=0; i < ui->optionsTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *qti = ui->optionsTree->topLevelItem(i);
        for (int j=0; j < qti->childCount(); j++)
        {
            QTreeWidgetItem *c = qti->child(j);
            if (c->checkState(0) == Qt::Checked)
                options.insert(c->data(0, Qt::UserRole).toByteArray() );
        }
    }

    int cat = ui->catCombo->currentIndex()+1;
    if (cat == 5)
        cat = 9;
    CustomFilter f(ui->nameEdit->text(), ui->titleEdit->text(), ui->spotterEdit->text(), ui->tagEdit->text(), options, cat, ui->sortCombo->currentIndex() );
    return f;
}

void FilterDialog::_addSubcatsToTree(QTreeWidgetItem *parent, int cat, const char *letter)
{
    bool hasitemsset = false;

    for (int i=0; i<99; i++)
    {
        QByteArray id;
        if (i < 10)
            id = QByteArray(letter)+"0"+QByteArray::number(i);
        else
            id = letter+QByteArray::number(i);
        QString name = SpotCategories::cat2name(cat, id);

        if (!name.isEmpty() && name != tr("Onbekend"))
        {
            QTreeWidgetItem *twi = new QTreeWidgetItem(parent);
            twi->setText(0, name);
            twi->setData(0, Qt::UserRole, id);
            if (_f.options().contains(id))
            {
                twi->setCheckState(0, Qt::Checked);
                hasitemsset = true;
            }
            else
                twi->setCheckState(0, Qt::Unchecked);
        }
    }

    if (hasitemsset)
        parent->setExpanded(true);
}

void FilterDialog::_createOptionsTree(int cat)
{
    QTreeWidget *t = ui->optionsTree;
    t->clear();

    QTreeWidgetItem *a = new QTreeWidgetItem(t);
    QTreeWidgetItem *b = new QTreeWidgetItem(t);
    QTreeWidgetItem *c = NULL;
    QTreeWidgetItem *d = NULL;

    if (cat != 4)
        c = new QTreeWidgetItem(t);

    if (cat == 1 || cat == 2 || cat == 9)
    {
        a->setText(0, tr("Formaat"));
        b->setText(0, tr("Bron"));
        d = new QTreeWidgetItem(t);
        d->setText(0, tr("Genre"));
    }

    switch (cat)
    {
    case 1:
    case 9:
        c->setText(0, tr("Talen en ondertitels"));
        break;

    case 2:
        c->setText(0, tr("Bitrate"));
        break;

    case 3:
        a->setText(0, tr("Platform"));
        b->setText(0, tr("Formaat"));
        c->setText(0, tr("Genre"));
        break;

    case 4:
        a->setText(0, tr("Platform"));
        b->setText(0, tr("Genre"));
        break;

    default:
        break;
    }

    _addSubcatsToTree(a, cat, "a");
    _addSubcatsToTree(b, cat, "b");
    if (c)
        _addSubcatsToTree(c, cat, "c");
    if (d)
        _addSubcatsToTree(d, cat, "d");
}

void FilterDialog::on_catCombo_currentIndexChanged(int index)
{
    int cat = index+1;
    if (cat == 5)
        cat = 9;
    _f.options().clear();
    _createOptionsTree(cat);
}

void FilterDialog::accept()
{
    if ( ui->nameEdit->text().isEmpty() )
    {
        QMessageBox::critical(this, tr("Fout!"), tr("Vergeet niet de titel van de filter in te vullen!"));
    }
    else
        QDialog::accept();
}
