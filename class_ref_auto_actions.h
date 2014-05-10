#ifndef CLASS_REF_AUTO_ACTIONS_H
#define CLASS_REF_AUTO_ACTIONS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QValidator>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class class_ref_auto_actions;
}

class class_ref_auto_actions : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_auto_actions(QWidget *parent, QSqlDatabase *db1);
    ~class_ref_auto_actions();

public slots:
    void slot_select_table();
    void slot_enable_button();
    void slot_del_actions();
    void slot_update_action();
    void slot_sort_pp(int sort_id);

private:
    Ui::class_ref_auto_actions *ui;
    QSqlDatabase *db;
    QSqlQueryModel *model;
    QSqlQuery *query;
    int id_column;
};

#endif // CLASS_REF_AUTO_ACTIONS_H
