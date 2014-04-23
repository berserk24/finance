#ifndef CLASS_REF_CLIENT_H
#define CLASS_REF_CLIENT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>

namespace Ui {
class class_ref_client;
}

class class_ref_client : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_client(QWidget *parent = 0);
    ~class_ref_client();

public slots:
    void slot_enable_add_edit();
    void slot_add_edit_client();
    void slot_enable_del();
    void slot_del_client();
    void slot_enable_edit_tarif();
    void select_table();
    void select_tarif();

private:
    Ui::class_ref_client *ui;
    QSqlQueryModel *model;
    QSqlQuery *query;

    void clear_field();

};

#endif // CLASS_REF_CLIENT_H
