#ifndef CLASS_REF_BALANS_RS_H
#define CLASS_REF_BALANS_RS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>

namespace Ui {
class class_ref_balans_rs;
}

class class_ref_balans_rs : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_balans_rs(QWidget *parent = 0);
    ~class_ref_balans_rs();

public slots:
    void slot_select_table();
    void slot_sum_balans_rs();

signals:
    void signal_send_sum_rs(QString);

private:
    Ui::class_ref_balans_rs *ui;
    QSqlQueryModel *model;
};

#endif // CLASS_REF_BALANS_RS_H
