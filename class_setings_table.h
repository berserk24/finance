#ifndef CLASS_SETINGS_TABLE_H
#define CLASS_SETINGS_TABLE_H

#include <QWidget>
#include <QCheckBox>
#include <QDebug>

namespace Ui {
class class_setings_table;
}

class class_setings_table : public QWidget
{
    Q_OBJECT

public:
    explicit class_setings_table(QWidget *parent = 0);
    ~class_setings_table();

public slots:
    void slot_set_checkbox(QString, bool);
    void _close();
    void slot_set_collumn_state();

signals:
    void signal_set_collumn_state(QList<bool>);

private:
    Ui::class_setings_table *ui;
    QCheckBox *chbox;

};

#endif // CLASS_SETINGS_TABLE_H
