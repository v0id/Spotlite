#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "uisettingsdialog.h"
#include "spotsmodel.h"
#include "spotlite.h"
#include "spotview.h"
#include "spotsignature.h"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTime>
#include <QDebug>
#include <QProgressDialog>
#include <QFontMetrics>
#include <QCursor>

#include <QWebSettings>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>
#include <stdexcept>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFileDialog>
#include <QIcon>
#include <QShortcut>
#include <QKeySequence>
#include <QLabel>

#include "dummyitemdelegate.h"

#ifdef Q_WS_WIN
#include "crashdumper.h"
#endif

#include "customfilter.h"
#include "filterdialog.h"
#include "addressdialog.h"
// #include "spotmobile.h"
// #include "spotmobilesettings.h"
#include "SpotLite-skininstaller/skininstaller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _onsingleclick(load),
    _ondoubleclick(loadandopen)
    //,_smobile(NULL)
{
#ifdef Q_WS_WIN
    QByteArray dumpfile = QDir::toNativeSeparators(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/crash.dmp").toLatin1();
    Crashdumper::setMinidumpFile( dumpfile.constData() );
#endif

    ui->setupUi(this);
    //ui->treeWidget->collapse(ui->treeWidget->model()->index(6,0) ); // ero

    QString datadir;
    bool portable;

    if ( QFile::exists(QApplication::applicationDirPath()+"/SpotLite.portable" ) )
    {
        datadir = QApplication::applicationDirPath();
        portable = true;
    }
    else
    {
        datadir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        portable = false;
    }

    try
    {
        sl = new SpotLite(datadir, portable, this);
    }
    catch (std::runtime_error &re)
    {
        onError(0, re.what() );
        throw re;
    }

    connect(sl, SIGNAL(error(int,QString)), this, SLOT(onError(int,QString)));
    connect(sl, SIGNAL(notice(int,QString)), this, SLOT(onNotice(int,QString)));

    model = new SpotsModel(sl, this);
    ui->tableView->setModel(model);
    ui->imagelist->setSpotLite(sl);
    ui->imagelist->setModel(model);
    connect(ui->imagelist, SIGNAL(spotOpened(int,QString)), this, SLOT(openSpot(int,QString)));

    //ui->listView->setModel(model);

    QHeaderView *h = ui->tableView->horizontalHeader();
    h->setStretchLastSection(false);
    h->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->setColumnWidth(0, 120);
    ui->tableView->setColumnWidth(1, 50);

    //ui->listView->setModelColumn(2);

    QSettings *s = sl->settings();

    if ( !s->value("privkey").isValid() )
    {
        QProgressDialog d(tr("Bezig met aanmaken private key.."), QString(), 0, 100);
        d.setMinimumDuration(0);
        d.setValue(10);
        QApplication::processEvents();

        try
        {
            qDebug() << "Aanmaken private key";
            SpotSignature sig;
            qDebug() << "Klaar met aanmaken";
            s->setValue("privkey", sig.privateKey() );
            s->setValue("pubkey", sig.publicKey() );
            s->sync();
        }
        catch (std::runtime_error &re)
        {
            onError(0, re.what() );
            throw re;
        }
    }
    if ( !s->value("ver").isValid() )
    {
        s->setValue("ver", 7);
        s->setValue("maxspots", 1000000000);
    }
    if ( s->value("port").isValid() )
    {
        if ( s->value("geometry").isValid() )
            restoreGeometry(s->value("geometry").toByteArray());
    }
    else
    {
        SettingsDialog *sd = new SettingsDialog(sl->settings(), this);
        sd->exec();
    }
    sl->connectToServer();

    initTree(ui->treeWidget);
    setEroDisplay( s->value("ero").toBool() );
    setTrashDisplay( s->value("showtrash").toBool() );

    setSkin( s->value("skin", "standaard").toString() );

    setFonts();
    //model->setQuery("", -1, 0);
    ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));

    /* Splitter */
    ui->frame->setMaximumWidth(10000);

    if ( s->value("splitter").isValid() )
    {
        ui->splitter->restoreState( s->value("splitter").toByteArray() );
    }
    else
    {
        QList<int> sizes;
        sizes << 200 << width()-200;
        ui->splitter->setSizes(sizes);
    }

    /* Performance test */
    /*QTime timer;
    timer.start();
    model->setQuery("", 1, 0);
    for (int i=0; i<100000; i++)
        QVariant x = model->data(model->index(i,1), Qt::DisplayRole);
    qDebug() << "Time: " << timer.elapsed()/1000.0;
    */

    /*if ( s->value("spotMobileEnabled").toBool() )
    {
        QHostAddress h( s->value("spotMobileAddress").toString() );
        if ( !QNetworkInterface::allAddresses().contains(h) )
        {
            QMessageBox::warning(this, tr("IP niet meer geldig"), tr("Je IP-adres is gewijzigd, controleer SpotMobiel instellingen."));
            SpotMobileSettings smd(sl->settings());

            if ( smd.exec() == QDialog::Accepted )
                activateSpotMobile();
        }
        else
            activateSpotMobile();
    }*/

    ui->updateButton->setVisible( s->value("updateknop").toBool() );

    QShortcut *qsc = new QShortcut( QKeySequence("Ctrl+W"), this->ui->tabWidget );
    connect(qsc, SIGNAL(activated()), this, SLOT(closeCurrentTab()));

    ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
}

void MainWindow::initTree(QTreeWidget *t)
{
    QList<QVariant> ids;
    ids << -1 << 0 << -2 << 1 << 2 << 3 << 4 << 9 << -4;

    /* Top-level categories */
    for (int i=0; i<ids.count(); i++)
        t->topLevelItem(i)->setData(0, Qt::UserRole, ids[i]);

    /* Ero nieuw/watchlist */
    t->topLevelItem(7)->child(0)->setData(0, Qt::UserRole, -9);
    t->topLevelItem(7)->child(1)->setData(0, Qt::UserRole, -902);

    if ( sl->settings()->value("filters").isValid() )
    {
        loadFilters();
    }
    else
    {
        addSubcatFilter(1, 0, "DivX");
        addSubcatFilter(1, 1, "WMV");
        addSubcatFilter(1, 2, "MPG");
        addSubcatFilter(1, 3, "DVD5");
        addSubcatFilter(1, 10, "DVD9");
        addSubcatFilter(1, 9, "x264");
        addSubcatFilter(1, 4, "HD");
        addSubcatFilter(1, 6, "Blu-ray");
        addSubcatFilter(1, 8, "WMV-HD");
        addSubcatFilter(1, 5, "Afbeeldingen");

        /* genres */
        QSet<QByteArray> opt;
        opt << "d00";
        addFilterToTree(CustomFilter("Actie","","","",opt,1,0));

        opt.clear();
        opt << "d02";
        addFilterToTree(CustomFilter("Animatie","","","",opt,1,0));

        opt.clear();
        opt << "d07";
        addFilterToTree(CustomFilter("Drama","","","",opt,1,0));

        opt << "d08" << "d33";
        addFilterToTree(CustomFilter("Jeugd","","","",opt,1,0));

        opt.clear();
        opt << "d04";
        addFilterToTree(CustomFilter("Komedie","","","",opt,1,0));

        opt.clear();
        opt << "c11";
        addFilterToTree(CustomFilter("Nederlands","","","",opt,1,0));

        opt.clear();
        opt << "d16";
        addFilterToTree(CustomFilter("Romantiek","","","",opt,1,0));

        opt.clear();
        opt << "d12" << "d20";
        addFilterToTree(CustomFilter("Spannend","","","",opt,1,0));
        /* --- */

        const char *geluid[] = {"MP3", "", "WAV", "", "Lossless", "DTS", "AAC", "", "FLAC", 0};
        addSubcatFilters(2, geluid);

        addSubcatFilter(3, 0, "Windows");
        addSubcatFilter(3, 1, "MAC");
        addSubcatFilter(3, 2, "Linux");
        addSubcatFilter(3, 3, "Playstation");
        addSubcatFilter(3, 4, "Playstation 2");
        addSubcatFilter(3, 12, "Playstation 3");
        addSubcatFilter(3, 5, "PSP");
        addSubcatFilter(3, 6, "XBox");
        addSubcatFilter(3, 7, "XBox 360");
        addSubcatFilter(3, 10, "Nintendo DS");
        addSubcatFilter(3, 11, "Nintendo Wii");
        addSubcatFilter(3, 13, "Phone");
        addSubcatFilter(3, 14, "IOs");
        addSubcatFilter(3, 15, "Android");

        const char *apps[] = {"Windows", "MAC", "Linux", "", "Phone", "Navigatie", "IOs", "Android", 0};
        addSubcatFilters(4, apps);

        const char *ero[] = {"Hetero", "Homo", "Lesbo", "Bi", 0};
        addSubcatFilters(9, ero);
        loadFilters("customfilters"); // oude filters
    }

    /* Menutje */
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(catPopup(QPoint)));
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Lijst menutje */
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(listPopup(QPoint)));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Inklappen of uitklappen? */
    QVariant expanded = sl->settings()->value("expanded");
    if ( expanded.isValid() )
    {
        QStringList list = expanded.toStringList();
        foreach (QString item, list)
        {
            QList<QTreeWidgetItem *> items = ui->treeWidget->findItems(item, Qt::MatchExactly);
            if (!items.isEmpty())
                items.first()->setExpanded(true);
        }
    }
    else
    {
        ui->treeWidget->expandAll();
    }
}

void MainWindow::catPopup(const QPoint &p)
{
    QMenu menu;

    QTreeWidgetItem *item = ui->treeWidget->itemAt(p);
    bool editable = item && item->data(0, Qt::UserRole).type() == QVariant::UserType;
    int pos = 0;
    int cat = 0;
    if (editable)
    {
        pos = item->parent()->indexOfChild(item);
        cat = ui->treeWidget->indexOfTopLevelItem(item->parent())-2;
        if (cat == 5)
            cat = 9;
    }

    QAction *act = new QAction(tr("Nieuw filter toevoegen"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(catPopupNewFilter()));
    menu.addAction(act);

    act = new QAction(tr("Filter bewerken"), this);
    if (editable)
    {
        connect(act, SIGNAL(triggered()), this, SLOT(catPopupUpdateFilter()));
    }
    else
        act->setDisabled(true);
    menu.addAction(act);

    act = new QAction(tr("Filter verwijderen"), this);
    if (editable)
    {
        connect(act, SIGNAL(triggered()), this, SLOT(catPopupDeleteFilter()));
    }
    else
        act->setDisabled(true);
    menu.addAction(act);

    act = new QAction(tr("Filter omhoog verplaatsen"), this);
    if (editable && ((cat != 9 && pos != 0) || (cat == 9 && pos > 2)) )
    {
        connect(act, SIGNAL(triggered()), this, SLOT(catPopupMoveUpFilter()));
    }
    else
        act->setDisabled(true);
    menu.addAction(act);

    act = new QAction(tr("Filter omlaag verplaatsen"), this);
    if (editable && pos != item->parent()->childCount()-1 )
    {
        connect(act, SIGNAL(triggered()), this, SLOT(catPopupMoveDownFilter()));
    }
    else
        act->setDisabled(true);
    menu.addAction(act);

    menu.exec(ui->treeWidget->mapToGlobal(QPoint(p.x()+10, p.y()+25)));
}

void MainWindow::catPopupNewFilter()
{
    QTreeWidgetItem *qti = ui->treeWidget->selectedItems().first();
    if ( qti->parent() )
        qti = qti->parent();
    int cat = ui->treeWidget->indexOfTopLevelItem(qti)-2;
    if (cat < 1)
        cat = 1;
    if (cat == 5)
        cat = 9;

    FilterDialog fd(CustomFilter(cat), this);
    if ( fd.exec() != QDialog::Accepted)
        return;

    addFilterToTree( fd.filtersettings() );
    saveFilters();

    QString skin = sl->settings()->value("skin", "standaard").toString();
    setSkin(skin);
}

void MainWindow::catPopupUpdateFilter()
{
    QTreeWidgetItem *qti = ui->treeWidget->selectedItems().first();

    FilterDialog fd( qti->data(0, Qt::UserRole).value<CustomFilter>(), this);
    if ( fd.exec() != QDialog::Accepted)
        return;

    CustomFilter f = fd.filtersettings();
    qti->setData(0, Qt::UserRole, f);
    qti->setText(0, f.name() );

    saveFilters();
}

void MainWindow::catPopupDeleteFilter()
{
    QTreeWidgetItem *qti = ui->treeWidget->selectedItems().first();

    if (QMessageBox::question(this, tr("Weet je het zeker?"), tr("Filter \"%1\" verwijderen?").arg(qti->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        delete qti;
        saveFilters();
    }
}

void MainWindow::catPopupMoveUpFilter()
{
    QTreeWidgetItem *qti    = ui->treeWidget->selectedItems().first();
    QTreeWidgetItem *parent = qti->parent();
    int idx = parent->indexOfChild(qti);

    parent->removeChild(qti);
    parent->insertChild(idx-1, qti);
    ui->treeWidget->setCurrentItem(qti, 0);
    saveFilters();
}

void MainWindow::catPopupMoveDownFilter()
{
    QTreeWidgetItem *qti    = ui->treeWidget->selectedItems().first();
    QTreeWidgetItem *parent = qti->parent();
    int idx = parent->indexOfChild(qti);

    parent->removeChild(qti);
    parent->insertChild(idx+1, qti);
    ui->treeWidget->setCurrentItem(qti, 0);
    saveFilters();
}

void MainWindow::onCreateTagFilter(int cat, const QByteArray &tag)
{
    CustomFilter f;
    f.setCat(cat);
    f.setTag(tag);
    f.setName(tag);
    FilterDialog fd(f, this);
    if ( fd.exec() != QDialog::Accepted)
        return;

    addFilterToTree( fd.filtersettings() );

    saveFilters();
}

void MainWindow::addFilterToTree(const CustomFilter &f)
{
    /* Nieuw filtertje aan boom toevoegen */
    int t = f.cat()+2;
    if (t == 11) /* ero */
        t = 7;

    QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(t);
    QTreeWidgetItem *qti    = new QTreeWidgetItem(parent);
    qti->setText(0, f.name() );
    qti->setData(0, Qt::UserRole, f);

    /*
    QLabel *l = new QLabel();
    l->setProperty("cat", f.cat() );
    l->setProperty("customfilter", !f.isStandardFilter() );
    l->setText( f.name() );
    ui->treeWidget->setItemWidget(qti, 0, l);
    ui->treeWidget->setItemDelegate(new DummyItemDelegate(this));*/
}

void MainWindow::addSubcatFilter(int cat, int subcat, const QByteArray &name)
{
    addFilterToTree(CustomFilter(cat, subcat, name));
}

void MainWindow::addSubcatFilters(int cat, const char **filters)
{
    int i = 0;
    for (const char **f = filters; *f; f++, i++)
    {
        if (**f)
            addSubcatFilter(cat, i, *f);
    }
}

void MainWindow::saveFilters()
{
    QList<QVariant> fl;

    for (int i=0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *tli = ui->treeWidget->topLevelItem(i);
        for (int j=0; j < tli->childCount(); j++)
        {
            QVariant d = tli->child(j)->data(0, Qt::UserRole);
            if ( d.type() == QVariant::UserType)
            {
                fl.append(d);
            }
        }
    }

    sl->settings()->setValue("filters", fl);
    //qDebug() << "Filtertjes" << fl;
}

void MainWindow::loadFilters(const char *setting)
{
    QVariant vl = sl->settings()->value(setting);

    if (vl.isValid() && vl.type() == vl.List)
    {
        qDebug() << "Bezig met laden van filters";
        QList<QVariant> fl = vl.toList();

        foreach (QVariant v, fl)
        {
            addFilterToTree(v.value<CustomFilter>() );
        }

        qDebug() << "Filtertjes geladen";
    }
}

void MainWindow::setEroDisplay(bool show)
{
    ui->treeWidget->topLevelItem(7)->setHidden(!show);
}

void MainWindow::setTrashDisplay(bool show)
{
    ui->treeWidget->topLevelItem(8)->setHidden(!show);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAfsluiten_triggered()
{
    close();
}

void MainWindow::on_actionConfiguratie_triggered()
{
    QSettings *set = sl->settings();
    SettingsDialog s(set);
    bool oldspamsetting = set->value("deletespam").toBool();

    if (s.exec() == QDialog::Accepted)
    {
        setEroDisplay( set->value("ero").toBool() );
        setTrashDisplay( set->value("showtrash").toBool() );
        if (set->value("lastcommentsheader").toInt() == 0)
            sl->emptyCommentsList();

        if ( !oldspamsetting && set->value("deletespam").toBool() )
        {
            QApplication::setOverrideCursor( Qt::BusyCursor );
            QApplication::processEvents();
            sl->deleteSpam();
            QApplication::restoreOverrideCursor();
        }

        sl->reconnect();
        ui->updateButton->setVisible( set->value("updateknop").toBool() );
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings *s = sl->settings();
    s->setValue("geometry", saveGeometry());
    s->setValue("splitter", ui->splitter->saveState() );

    /* Uitgeklapte items opslaan */
    QStringList list;
    for (int i=0; i< ui->treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *qti = ui->treeWidget->topLevelItem(i);
        if ( qti->isExpanded() )
        {
            list << qti->data(0, Qt::DisplayRole).toString();
        }
    }
    s->setValue("expanded", list);

    event->accept();
}

void MainWindow::onError(int, const QString &msg)
{
    QMessageBox::critical(this, tr("Error"), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void MainWindow::onNotice(int , const QString &msg)
{
    qDebug() << "NOTICE:" << msg;
    ui->statusBar->showMessage(msg, 300000);
}

void MainWindow::on_zoekButton_clicked()
{
//    QTime timer;
//    timer.start();

    onNotice(0, tr("Bezig met zoeken... Kan even duren..."));
    QApplication::setOverrideCursor( Qt::BusyCursor );
    QApplication::processEvents();
    QString q = ui->queryEdit->text();

    if ( ui->onlycat_checkbox->isChecked() )
    {
        QTreeWidgetItem *current = ui->treeWidget->currentItem();
        QString catname = current->data(0, Qt::DisplayRole).toString();
        QVariant v = current->data(0, Qt::UserRole);
        CustomFilter f;

        if ( v.type() == v.UserType)
        {
            f = v.value<CustomFilter>();
        }
        else
        {
            int cat = v.toInt();
            if (cat > 0)
                f.setCat(cat);
            else if (cat < 0)
            {
                QApplication::restoreOverrideCursor();
                onError(0, tr("Het doorzoeken van '%1' wordt momenteel niet ondersteund").arg(catname) );
                return;
            }
        }

        f.setTitle(q);
        model->setQuery(f);
        ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    }
    else
    {
        model->setQuery(q.toLatin1(), 0, 0);
        ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    }


//    qDebug() << "s1 in" << (timer.elapsed() / 1000.0) << "secs";
    ui->tabWidget->setTabText(0, tr("Zoekresultaten: ")+q.replace("&", "&&") );
    ui->tabWidget->setCurrentIndex(0);
    ui->tableView->scrollToTop();

    ui->imagelist->setIndex(0);
    if ( ui->tabWidget_2->currentIndex() == 1)
        ui->imagelist->refresh();

    onNotice(0, tr("%1 resultaten voor zoekopdracht '%2'").arg( QString::number( model->rowCount(QModelIndex()) ), q) );
    QApplication::restoreOverrideCursor();
//    qDebug() << "s2 in" << (timer.elapsed() / 1000.0) << "secs";
}

/*
void MainWindow::on_treeWidget_activated(QModelIndex index)
{
    if (index.isValid() )
    {
        QString catname = index.model()->data(index).toString();
        int cat = index.model()->data(index, Qt::UserRole).toInt();
        if (cat > 99) // subcat
            model->setQuery("", 0, cat);
        else
            model->setQuery("", cat, 0);

        ui->tabWidget->setTabText(0, catname);
        onNotice(0, tr("%1 resultaten in categorie '%2'").arg( QString::number( model->rowCount(QModelIndex()) ), catname) );
    }
}
*/

void MainWindow::on_tableView_clicked(QModelIndex index)
{
    Qt::KeyboardModifiers keys = QApplication::keyboardModifiers();
    if (keys & Qt::ShiftModifier || keys & Qt::ControlModifier)
    {
        return;
    }

    if ( _trashCategory() )
    {
        listPopup( ui->tableView->viewport()->mapFromGlobal(QCursor::pos() ) );
        return;
    }

    switch (_onsingleclick)
    {
    case load:
        loadSpot(index);
        break;
    case loadandopen:
        openSpot(index);
        break;
    default:
        break;
    }
}

void MainWindow::on_tableView_doubleClicked(QModelIndex index)
{
    if ( _trashCategory() )
        return;

    switch (_ondoubleclick)
    {
    case load:
        loadSpot(index);
        break;
    case loadandopen:
        openSpot(index);
        break;
    default:
        break;
    }
}

void MainWindow::loadSpot(const QModelIndex &index)
{
    if (index.isValid() )
    {
        int id        = index.model()->data(index, Qt::UserRole).toInt();

        if (!spotviews.contains(id))
        {
            QString title = index.model()->data(index.model()->index(index.row(), 2)).toString();

            SpotView *spotview = new SpotView(sl, ui->tabWidget, id, title, skinpath, this);
            spotviews.insert(id, spotview);
            connect(spotview, SIGNAL(createTagFilter(int,QByteArray)), this, SLOT(onCreateTagFilter(int,QByteArray)));
            connect(spotview, SIGNAL(notice(int,QString)), this, SLOT(onNotice(int,QString)));
            connect(spotview, SIGNAL(closeRequested()), this, SLOT(onSpotviewClose()));
            spotview->loadSpot();
        }
    }
}

void MainWindow::openSpot(const QModelIndex &index)
{
    if (index.isValid() )
    {
        loadSpot(index);
        int id        = index.model()->data(index, Qt::UserRole).toInt();
        if (spotviews.contains(id))
        {
            spotviews[id]->activate();
        }
    }
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (index == 0)
    {
        onNotice(0, tr("Overzicht kan niet gesloten worden"));
        return;
    }

    foreach (SpotView *v, spotviews)
    {
        if (v->tabIndex() == index)
        {
            QWidget *w = ui->tabWidget->widget(index);
            ui->tabWidget->removeTab(index);
            spotviews.remove( v->spotId() );
            v->deleteLater();
            w->deleteLater();
            break;
        }
    }
}

void MainWindow::onSpotviewClose()
{
    SpotView *sv = static_cast<SpotView *>( sender() );
    on_tabWidget_tabCloseRequested( sv->tabIndex() );
}

void MainWindow::on_actionOver_SpotLite_triggered()
{
    QMessageBox::about(this, SPOTLITE_VERSION, SPOTLITE_VERSION+tr("<br>Gemaakt door Q.") );
}

void MainWindow::on_queryEdit_returnPressed()
{
    on_zoekButton_clicked();
}

void MainWindow::on_actionDatabase_optimalizeren_triggered()
{
    onNotice(0, tr("Bezig met optimalizeren database... Kan even duren..."));
    QApplication::processEvents();

    if ( sl->vacuumDB() )
        onNotice(0, tr("Database geoptimalizeerd!"));
    else
        onNotice(0, tr("Database is in gebruik, kan deze nu niet optimalizeren..."));
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    QString catname = current->data(0, Qt::DisplayRole).toString();
    QVariant v = current->data(0, Qt::UserRole);

    if ( v.type() == v.UserType)
    {
        model->setQuery( v.value<CustomFilter>() );
    }
    else
    {
        int cat = v.toInt();
        if (cat > 99) // subcat
            model->setQuery("", 0, cat);
        else
            model->setQuery("", cat, 0);
    }
    ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

    ui->tabWidget->setTabText(0, catname);
    ui->tabWidget->setCurrentIndex(0);
    ui->tableView->scrollToTop();
    ui->imagelist->setIndex(0);
    if ( ui->tabWidget_2->currentIndex() == 1)
        ui->imagelist->refresh();
    onNotice(0, tr("%1 resultaten in categorie '%2'").arg( QString::number( model->rowCount(QModelIndex()) ), catname) );
}

void MainWindow::on_actionHeaders_verwijderen_en_opnieuw_downloaden_triggered()
{
    if (QMessageBox::question(this, tr("Weet je het zeker?"), tr("Inhoud van database verwijderen, wachtlist verwijderen en opnieuw alle headers downloaden?"), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes)
    {
        onNotice(0, tr("Bezig met verwijderen databases... Kan even duren..."));
        QApplication::processEvents();

        if ( sl->emptyDatabases() )
        {
            onNotice(0, tr("Database verwijderd!"));
            sl->reconnect();
        }
        else
        {
            onError(0, tr("Database is momenteel in gebruik. Kan nog niet verwijderen"));
        }
    }
}

void MainWindow::on_actionGebruikersinterface_aanpassen_triggered()
{
    QSettings *s = sl->settings();
    QString currentskin = s->value("skin", "standaard").toString();

    UISettingsDialog d(s);
    d.exec();
    QString newskin = s->value("skin").toString();
    if (currentskin != newskin)
        setSkin(newskin);

    setFonts();
}

void MainWindow::setFonts()
{
    QSettings *s = sl->settings();

    if ( s->value("listfont").isValid() )
    {
        QFont f, sf;
        f.fromString( s->value("listfont").toString() );
        ui->treeWidget->setFont(f);
        ui->tableView->setFont(f);
        ui->tableView->verticalHeader()->setDefaultSectionSize(f.pointSize()+11);

        QFontMetrics fm(f);
        int w = fm.width("31-12-2011 (23:59)");
        ui->tableView->setColumnWidth(0, qMax(120, w));

        int iconwidth = model->iconWidth();
        if (iconwidth)
        {
            ui->tableView->setColumnWidth(1, iconwidth+8);
        }
        else
        {
            w = fm.width("LINUX");
            ui->tableView->setColumnWidth(1, qMax(50, w));
        }

        sf.fromString( s->value("spotfont").toString() );
        QWebSettings *ws = QWebSettings::globalSettings();
        ws->setFontSize(QWebSettings::MinimumFontSize, sf.pointSize() );

        _onsingleclick = (clickAction) s->value("singleclickaction").toInt();
        _ondoubleclick = (clickAction) s->value("doubleclickaction").toInt();
    }
}

void MainWindow::on_actionAdresboek_triggered()
{
    AddressDialog ad(sl);
    ad.exec();
}

QTreeWidget *MainWindow::catTree()
{
    /* FIXME: ugly workaround */
    return ui->treeWidget;
}

void MainWindow::on_actionSpotMobiel_triggered()
{
    /*SpotMobileSettings d(sl->settings());
    if ( d.exec() == QDialog::Accepted )
        activateSpotMobile();*/
}

void MainWindow::activateSpotMobile()
{
    /*
    if (_smobile)
    {
        delete _smobile;
        _smobile = NULL;
    }

    QSettings *s = sl->settings();
    if ( s->value("spotMobileEnabled").toBool() )
    {
        _smobile = new SpotMobile(sl, this, this);
        _smobile->listen( QHostAddress(s->value("spotMobileAddress").toString() ), static_cast<quint16> (s->value("spotMobilePort").toInt() ), s->value("spotMobileUser").toByteArray(), QByteArray::fromBase64(s->value("spotMobilePass").toByteArray() ) );
    }
    */
}

void MainWindow::on_actionDatabase_importeren_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Geef de bestandsnaam op"), QString(), tr("Database dump (*.dbdmp)") );
    if ( !filename.isEmpty() )
    {
        importDB(filename);
    }
}

void MainWindow::on_actionDatabase_exporteren_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Geef de bestandsnaam op"), QString(), tr("Database dump (*.dbdmp)") );
    if ( !filename.isEmpty() )
    {
        onNotice(0, tr("Bezig met exporteren... Kan even duren..."));
        QApplication::setOverrideCursor( Qt::BusyCursor );
        QApplication::processEvents();

        if ( QFile::exists(filename) )
            QFile::remove(filename);
        sl->exportDatabase(filename);
        QApplication::restoreOverrideCursor();
    }
}

QString MainWindow::skinFolder()
{
    QString s = "/usr/local/share/SpotLite/skins";

    if ( !QFile::exists(s))
    {
        s = QApplication::applicationDirPath()+"/skins";
    }

    return s;
}

void MainWindow::setSkin(const QString &skinname)
{
    skinpath = skinFolder();
    skinpath += "/"+skinname;
    model->changeTheme(skinpath);

    QString css = skinpath+"/qt.css";
    if ( QFile::exists(css) )
    {
        QFile fd(css);
        fd.open(fd.ReadOnly);
        qobject_cast<QApplication *>(QApplication::instance() )->setStyleSheet( fd.readAll() );
        fd.close();
    }
    else
        qobject_cast<QApplication *>(QApplication::instance() )->setStyleSheet("");

    QTreeWidget *t = ui->treeWidget;
    bool sizeset = false;
    QString catlistpath = skinpath+"/categorielijst/";

    for (int i=0; i < t->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *qwi = t->topLevelItem(i);
        QString icofilename = catlistpath+qwi->text(0)+".png";

        if (QFile::exists(icofilename))
        {
            QIcon ico(icofilename);
            qwi->setIcon(0, ico);

            if (!sizeset)
            {
                t->setIconSize( ico.availableSizes().first() );
                sizeset = true;
            }
        }
        else
        {
            qwi->setIcon(0, QIcon() );
        }

        QIcon defaultico;
        QString defaulticofilename = catlistpath+qwi->text(0)+"_default.png";
        if (QFile::exists(defaulticofilename))
        {
            defaultico.addFile(defaulticofilename);
        }

        for (int j=0; j < qwi->childCount(); j++)
        {
            QTreeWidgetItem *c = qwi->child(j);
            QString childicofilename = catlistpath+qwi->text(0)+"_"+c->text(0)+".png";

            if (QFile::exists(childicofilename))
            {
                QIcon ico(childicofilename);
                c->setIcon(0, ico);

                if (!sizeset)
                {
                    t->setIconSize( ico.availableSizes().first() );
                    sizeset = true;
                }
            }
            else if (!defaultico.isNull() )
            {
                c->setIcon(0, defaultico);
            }
            else
            {
                c->setIcon(0, QIcon() );
            }
        }
    }
}

void MainWindow::openFile(const QString &name)
{
    if ( name.endsWith(".spotskin", Qt::CaseInsensitive) )
    {
        try
        {
            SkinInstaller ski(name);
            ski.sudoInstall();
            //qDebug() << "skin" << ski.skinName();
            UISettingsDialog d( sl->settings(), ski.skinName() );
            d.exec();
            setSkin( sl->settings()->value("skin").toString() );
            setFonts();
        }
        catch (std::runtime_error &re)
        {
            QMessageBox::critical(this, tr("Fout bij installeren skin"), tr("FOUT: ")+re.what() );
        }
    }
    else if ( name.endsWith(".dbdmp", Qt::CaseInsensitive) || name.endsWith(".spotdb", Qt::CaseInsensitive) )
    {
        if ( QMessageBox::question(this, tr("Database importeren"), tr("Wil je het database bestand importeren (samenvoegen met bestaande gegevens)? Dit kan enkele minuten duren."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes )
        {
            importDB(name);
        }
    }
}

void MainWindow::importDB(const QString &filename)
{
    onNotice(0, tr("Bezig met importeren... Kan even duren..."));
    QApplication::setOverrideCursor( Qt::BusyCursor );
    QApplication::processEvents();

    if ( sl->importDatabase(filename) )
    {
        onNotice(0, tr("Database geimporteerd!"));
        model->resetNewcount();
        model->onNewSpots();
    }
    else
        onNotice(0, tr("Fout bij importeren. Database in gebruik?"));

    QApplication::restoreOverrideCursor();
}


void MainWindow::listPopup(const QPoint &p)
{
    QMenu menu;

    QModelIndex idx = ui->tableView->indexAt(p);

    if ( !idx.isValid() )
        return;

    int spotid    = idx.data(Qt::UserRole).toInt();
    bool multiselect = ui->tableView->selectionModel()->selectedRows().count() > 1;

    if ( _trashCategory() )
    {
        QAction *act = new QAction(tr("Verwijdering ongedaan maken"), this);
        act->setData(p);
        connect(act, SIGNAL(triggered()), this, SLOT(listUndelete()) );
        menu.addAction(act);

        act = new QAction(tr("Prullenbak legen"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(emptyTrash()));
        menu.addAction(act);
    }
    else
    {
        QAction *act = new QAction(tr("Openen"), this);
        act->setData(p);
        if (multiselect)
            act->setEnabled(false);
        else
            connect(act, SIGNAL(triggered()), this, SLOT(listOpen()));
        menu.addAction(act);

        if ( sl->isOnWatchlist(spotid) )
        {
            act = new QAction(tr("Van watchlist VERWIJDEREN"), this);
            act->setData(p);
            connect(act, SIGNAL(triggered()), this, SLOT(listWatchRemove()) );
            menu.addAction(act);
        }
        else
        {
            act = new QAction(tr("Op watchlist plaatsen"), this);
            act->setData(p);
            connect(act, SIGNAL(triggered()), this, SLOT(listWatch()));
            menu.addAction(act);
        }

        act = new QAction(tr("Verwijderen"), this);
        act->setData(p);
        connect(act, SIGNAL(triggered()), this, SLOT(listDelete()));
        menu.addAction(act);
    }

    //menu.exec(ui->tableView->mapToGlobal(QPoint(p.x()-5, p.y()+10)));
    menu.exec(ui->tableView->mapToGlobal(QPoint(p.x(), p.y()+10 )));
}

void MainWindow::listOpen()
{
    openSpot( ui->tableView->indexAt(static_cast<QAction *> ( sender() )->data().toPoint() ) );
}

void MainWindow::listDelete()
{
    QList<int> ids = _selectedIDs();

    if ( ids.count() > 1)
    {
        foreach (int id, ids)
        {
            sl->deleteSpot(id);
        }
    }
    else
    {
        int id = ui->tableView->indexAt(static_cast<QAction *> ( sender() )->data().toPoint() ).data(Qt::UserRole).toInt();
        sl->deleteSpot(id);
    }
}

void MainWindow::listWatch()
{
    QList<int> ids = _selectedIDs();

    if ( ids.count() > 1)
    {
        foreach (int id, ids)
        {
            sl->addToWatchlist(id);
        }
    }
    else
    {
        int id = ui->tableView->indexAt(static_cast<QAction *> ( sender() )->data().toPoint() ).data(Qt::UserRole).toInt();
        sl->addToWatchlist(id);
    }
}

void MainWindow::listWatchRemove()
{
    QList<int> ids = _selectedIDs();

    if ( ids.count() > 1)
    {
        foreach (int id, ids)
        {
            sl->removeFromWatchlist(id);
        }
    }
    else
    {
        int id = ui->tableView->indexAt(static_cast<QAction *> ( sender() )->data().toPoint() ).data(Qt::UserRole).toInt();
        sl->removeFromWatchlist(id);
    }
}

void MainWindow::listUndelete()
{
    QList<int> ids = _selectedIDs();

    if ( ids.count() > 1)
    {
        foreach (int id, ids)
        {
            sl->undeleteSpot(id);
        }
    }
    else
    {
        int id = ui->tableView->indexAt(static_cast<QAction *> ( sender() )->data().toPoint() ).data(Qt::UserRole).toInt();
        sl->undeleteSpot(id);
    }
}

QList<int> MainWindow::_selectedIDs()
{
    QModelIndexList rows = ui->tableView->selectionModel()->selectedRows();

    QList<int> ids;
    foreach (QModelIndex idx, rows)
    {
        ids << idx.data(Qt::UserRole).toInt();
    }

    return ids;
}

void MainWindow::emptyTrash()
{
    sl->emptyTrash();
}

bool MainWindow::_trashCategory()
{
    return ui->tabWidget->tabText(0) == "Prullenbak"; /* TODO: fix me */
}

void MainWindow::on_updateButton_clicked()
{
    //model->resetNewcount();
    QApplication::setOverrideCursor( Qt::BusyCursor );
    QApplication::processEvents();
    sl->processNewSpots();
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_tabWidget_2_currentChanged(int index)
{
    if (index == 1)
    {
        ui->imagelist->refresh();
    }
}

void MainWindow::openSpot(int id, const QString &title)
{

    if (!spotviews.contains(id))
    {
        SpotView *spotview = new SpotView(sl, ui->tabWidget, id, title, skinpath, this);
        spotviews.insert(id, spotview);
        connect(spotview, SIGNAL(createTagFilter(int,QByteArray)), this, SLOT(onCreateTagFilter(int,QByteArray)));
        connect(spotview, SIGNAL(notice(int,QString)), this, SLOT(onNotice(int,QString)));
        connect(spotview, SIGNAL(closeRequested()), this, SLOT(onSpotviewClose()));
        spotview->loadSpot();
    }
    spotviews[id]->activate();
}

void MainWindow::closeCurrentTab()
{
    on_tabWidget_tabCloseRequested(ui->tabWidget->currentIndex() );
}
