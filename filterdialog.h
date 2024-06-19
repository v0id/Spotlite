#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include "customfilter.h"

namespace Ui {
    class FilterDialog;
}
class QTreeWidgetItem;

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(const CustomFilter &f = CustomFilter(), QWidget *parent = 0);
    ~FilterDialog();
    CustomFilter filtersettings();

    inline CustomFilter &filter()
    {
        return _f;
    }

protected:
    Ui::FilterDialog *ui;
    CustomFilter _f;
    void _addSubcatsToTree(QTreeWidgetItem *parent, int cat, const char *letter);
    void _createOptionsTree(int cat);
    void accept();

private slots:
    void on_catCombo_currentIndexChanged(int index);
};

#endif // FILTERDIALOG_H
