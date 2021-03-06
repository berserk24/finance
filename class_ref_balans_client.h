#ifndef CLASS_REF_BALANS_CLIENT_H
#define CLASS_REF_BALANS_CLIENT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QValidator>
#include <QSqlError>
#include <QDate>
#include "update_client_balans.h"
#include <cmath>
#include <QDebug>

namespace Ui {
class class_ref_balans_client;
}

class class_ref_balans_client : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_balans_client(QWidget *parent, QSqlDatabase *db1);
    ~class_ref_balans_client();

public slots:
    void slot_select_table();
    void slot_enable_add_button();
    void slot_edit_balans();
    void slot_clear_field();
    void slot_select_client();
    void slot_enable_to_client();
    void slot_sum_balans_client();
    void slot_set_margin();
    void slot_sort_pp(int sort_id);

signals:
    void signal_send_sum_client(QString);

private:
    Ui::class_ref_balans_client *ui;
    QSqlQueryModel *model;
    QSqlQuery *query;
    update_client_balans *ucb;
    QString query_str;
    int id_column;
    QSqlDatabase *db;

};

#endif // CLASS_REF_BALANS_CLIENT_H
