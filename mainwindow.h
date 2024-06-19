#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QMap>

namespace Ui {
    class MainWindow;
}
class SpotsModel;
class SpotLite;
class SpotView;
class SpotMobile;
class QTreeWidget;
class QTreeWidgetItem;
class CustomFilter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTreeWidget *catTree();
    void setSkin(const QString &skinname);
    static QString skinFolder();

protected:
    Ui::MainWindow *ui;
    SpotsModel *model;
    SpotLite *sl;
    QMap<int,SpotView *> spotviews;
    QString skinpath;
    enum clickAction { doNothing=0, load=1, loadandopen =2};
    clickAction _onsingleclick, _ondoubleclick;
    //SpotMobile *_smobile;

    void closeEvent (QCloseEvent *event);
    void initTree(QTreeWidget *tree);
    void setEroDisplay(bool show);
    void setTrashDisplay(bool show);
    void setFonts();
    void loadSpot(const QModelIndex &index);
    void openSpot(const QModelIndex &index);
    void saveFilters();
    void loadFilters(const char *setting = "filters");
    void addFilterToTree(const CustomFilter &f);
    void addSubcatFilter(int cat, int subcat, const QByteArray &name);
    void addSubcatFilters(int cat, const char **filters);
    void activateSpotMobile();
    void importDB(const QString &filename);
    bool _trashCategory();
    QList<int> _selectedIDs();

private slots:
    void on_tabWidget_2_currentChanged(int index);
    void on_updateButton_clicked();
    void on_actionDatabase_exporteren_triggered();
    void on_actionDatabase_importeren_triggered();
    void on_actionSpotMobiel_triggered();
    void on_actionAdresboek_triggered();
    void on_actionGebruikersinterface_aanpassen_triggered();
    void on_actionHeaders_verwijderen_en_opnieuw_downloaden_triggered();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void on_actionDatabase_optimalizeren_triggered();
    void on_queryEdit_returnPressed();
    void on_actionOver_SpotLite_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_tableView_doubleClicked(QModelIndex index);
    void on_tableView_clicked(QModelIndex index);
//    void on_treeWidget_activated(QModelIndex index);
    void on_zoekButton_clicked();
    void on_actionConfiguratie_triggered();
    void on_actionAfsluiten_triggered();
    void onError(int, const QString &);
    void onNotice(int, const QString &);
    void catPopup(const QPoint &p);
    void catPopupNewFilter();
    void catPopupUpdateFilter();
    void catPopupDeleteFilter();
    void catPopupMoveUpFilter();
    void catPopupMoveDownFilter();
    void onCreateTagFilter(int cat, const QByteArray &tag);
    void onSpotviewClose();

    void listPopup(const QPoint &p);
    void listOpen();
    void listDelete();
    void listWatch();
    void listWatchRemove();
    void listUndelete();
    void emptyTrash();
    void closeCurrentTab();

public slots:
    void openFile(const QString &name);
    void openSpot(int id, const QString &title);
};

#endif // MAINWINDOW_H
