#ifndef CLASS_SETTINGS_H
#define CLASS_SETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QFileDialog>
#include <QSqlQuery>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
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
    void slot_load_rs();
    void slot_add_widget(int id, QString name);

private:
    QSettings *settings;
    Ui::class_settings *ui;
    QSqlQuery *query;
    QHBoxLayout *hb_layout;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *button;
};

#endif // CLASS_SETTINGS_H
