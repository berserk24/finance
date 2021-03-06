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
    explicit class_create_pp(QWidget *parent = 0, int id = 0, int type = 0);

signals:

public slots:
    void slot_show_info(bool state);
    void slot_show_payer(bool state);
    void slot_show_payer_recv(bool state);
    void slot_show_receiver(bool state);
    void slot_show_dest_pay(bool state);
    void slot_show_budget(bool state);
    void slot_show_auto_receiver(bool state);
    void slot_clear_form();
    void slot_get_count_pp();
    void slot_load_rss();
    void slot_set_date();
    void slot_load_firm();
    void slot_load_bik();
    void slot_select_receiver_bank();
    void slot_select_receiver_bank_city(int id);
    void slot_set_regexp();
    void slot_load_auto_rekv();
    void slot_set_enable_add();
    void slot_create_pp();
    void slot_set_dest_pay();
    void slot_load_pp();

private:
    Ui::class_create_pp *ui;
    QSqlQuery *query;
    int count_pp, count_pp_first, pp_id, act_type, payer_rs;
    QString summ;

};

#endif // CLASS_CREATE_PP_H
