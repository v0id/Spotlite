#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class QSettings;
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QSettings *qs, QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void accept();

private:
    Ui::SettingsDialog *ui;
    QSettings *s;

private slots:
    void on_spamCheck_toggled(bool checked);
    void on_meteenTonenRadio_toggled(bool checked);
    void on_sslBox_clicked(bool checked);
    void on_imagesCheck_clicked(bool checked);
};

#endif // SETTINGSDIALOG_H
