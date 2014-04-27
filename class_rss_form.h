#ifndef CLASS_RSS_FORM_H
#define CLASS_RSS_FORM_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDate>
#include <QDebug>

namespace Ui {
class class_rss_form;
}

class class_rss_form : public QWidget
{
    Q_OBJECT

public:
    explicit class_rss_form(QWidget *parent, QSqlDatabase *db1);
    ~class_rss_form();

public slots:
    void slot_add_rs();
    void slot_enable_add();
    void slot_enable_del();
    void slot_del_rs();
    void select_firm();
    void select_table();

private:
    Ui::class_rss_form *ui;
    QSqlQuery *query, *firm_query;
    QSqlQueryModel *model;
    QSqlDatabase *db;


    void clear_field();

};

#endif // CLASS_RSS_FORM_H
