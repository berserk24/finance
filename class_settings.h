#ifndef CLASS_SETTINGS_H
#define CLASS_SETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

namespace Ui {
class class_settings;
}

class class_settings : public QWidget
{
    Q_OBJECT

public:
    explicit class_settings(QWidget *parent = 0);
    ~class_settings();

public slots:
    void slot_close();
    void slot_get_settings();
    void slot_set_settings();
    void slot_select_dir();

private:
    QSettings *settings;
    Ui::class_settings *ui;
};

#endif // CLASS_SETTINGS_H
