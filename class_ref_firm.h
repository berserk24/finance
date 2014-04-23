#ifndef CLASS_REF_FIRM_H
#define CLASS_REF_FIRM_H

#include <QWidget>
#include <QTableView>
#include <QLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

namespace Ui {
class class_ref_firm;
}

class class_ref_firm : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_firm(QWidget *parent = 0);
    ~class_ref_firm();

public slots:
    void slot_enable_add();
    void slot_add_firm();
    void slot_enable_del();
    void slot_del_firm();
    void slot_show_add_rs();

private:
    Ui::class_ref_firm *ui;
    QSqlQuery *query;
    QSqlQueryModel *model;
    QWidget *form_rss;
    QTableView *tableView;
    QLayout *obj_layout;


    void refresh_tableview();
};

#endif // CLASS_REF_FIRM_H
