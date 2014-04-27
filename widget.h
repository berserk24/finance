#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QCryptographicHash>
#include "general_window.h"
#include <QSettings>
#include <QDebug>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    general_window *gen_window;
    QSqlDatabase *db;
    QSqlQuery *query;
    QSettings *settings;
    void create_database();
    void get_settings();
    void set_settings();

private slots:
    void slot_login();

};

#endif // WIDGET_H
