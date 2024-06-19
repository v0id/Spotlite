#ifndef SPOTMOBILESETTINGS_H
#define SPOTMOBILESETTINGS_H

#include <QDialog>

namespace Ui {
    class SpotMobileSettings;
}

class QSettings;
class SpotMobileSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SpotMobileSettings(QSettings *qs, QWidget *parent = 0);
    ~SpotMobileSettings();

public slots:
    void accept();

private:
    Ui::SpotMobileSettings *ui;
    QSettings *s;

private slots:
    void on_checkBox_clicked(bool checked);
};

#endif // SPOTMOBILESETTINGS_H
