#ifndef CLASS_REF_TARIF_H
#define CLASS_REF_TARIF_H

#include <QWidget>
#include <QValidator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class class_ref_tarif;
}

class class_ref_tarif : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_tarif(QWidget *parentparent = 0);
    ~class_ref_tarif();

public slots:
    void slot_enable_add_edit();
    void slot_add_edit_tarif();
    void slot_enable_del();
    void slot_del_tarif();
    void slot_sort_pp(int sort_id);


private:
    Ui::class_ref_tarif *ui;
    QSqlQueryModel *model;
    QSqlQuery *query, *query_def;
    QString query_str;
    int id_column;

    void clear_field();
    void select_table();
    void clear_default();

};

#endif // CLASS_REF_TARIF_H
