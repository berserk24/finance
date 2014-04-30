#ifndef CLASS_MANAGE_USERS_H
#define CLASS_MANAGE_USERS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

namespace Ui {
class class_manage_users;
}

class class_manage_users : public QWidget
{
    Q_OBJECT

public:
    explicit class_manage_users(QWidget *parent, QSqlDatabase *db1);
    ~class_manage_users();

public slots:
    void slot_select_table();
    void slot_enable_add();
    void slot_set_field();
    void slot_add_user();
    void slot_del_user();
    void slot_clear_field();

private:
    QSqlQueryModel *model;
    QSqlQuery *query;
    Ui::class_manage_users *ui;
    QSqlDatabase *db;
    bool func_str_to_bool(QString str);
};

#endif // CLASS_MANAGE_USERS_H
