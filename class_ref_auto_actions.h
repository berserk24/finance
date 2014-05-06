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

private:
    Ui::class_ref_auto_actions *ui;
    QSqlDatabase *db;
};

#endif // CLASS_REF_AUTO_ACTIONS_H
