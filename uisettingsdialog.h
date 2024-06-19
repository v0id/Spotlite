#ifndef UISETTINGSDIALOG_H
#define UISETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class UISettingsDialog;
}

class QSettings;
class UISettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UISettingsDialog(QSettings *s, const QString &skinToSelect = "", QWidget *parent = 0);
    ~UISettingsDialog();

private:
    Ui::UISettingsDialog *ui;
    QFont _listFont, _spotFont;
    QSettings *_s;
    QString _fontToString(QFont f);

private slots:
    void on_spotFontButton_clicked();
    void on_listFontButton_clicked();
    void accept();

};

#endif // UISETTINGSDIALOG_H
