#ifndef CLASS_REF_CLIENT_PAY_H
#define CLASS_REF_CLIENT_PAY_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "update_client_balans.h"
#include <QSqlError>
#include <QSqlQueryModel>
#include <QDebug>

namespace Ui {
class class_ref_client_pay;
}

class class_ref_client_pay : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_client_pay(QWidget *parent, QSqlDatabase *db1);
    ~class_ref_client_pay();

public slots:
    void slot_select_table();
    void slot_enable_cancel_button();
    void slot_cancel_pay();
    void select_client();

private:
    Ui::class_ref_client_pay *ui;
    QSqlQuery *query;
    QSqlQueryModel *model;
    update_client_balans *ucb;
    QSqlDatabase *db;
};

#endif // CLASS_REF_CLIENT_PAY_H
