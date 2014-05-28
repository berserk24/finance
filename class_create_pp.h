#ifndef CLASS_CREATE_PP_H
#define CLASS_CREATE_PP_H

#include <QWidget>
#include <QSqlQuery>
#include <QDate>
#include <QDebug>
#include <QSqlError>

namespace Ui {
class class_create_pp;
}

class class_create_pp : public QWidget
{
    Q_OBJECT
public:
    explicit class_create_pp(QWidget *parent = 0);

signals:

public slots:
    void slot_show_info(bool state);
    void slot_show_payer(bool state);
    void slot_show_receiver(bool state);
    void slot_show_dest_pay(bool state);
    void slot_show_budget(bool state);
    void slot_show_auto_receiver(bool state);
    void slot_clear_form();
    void slot_get_count_pp();
    void slot_load_rss();
    void slot_set_date();

private:
    Ui::class_create_pp *ui;
    QSqlQuery *query;

};

#endif // CLASS_CREATE_PP_H
