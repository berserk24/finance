#ifndef GENERAL_WINDOW_H
#define GENERAL_WINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QVector>
#include <QSqlQuery>
#include <QDebug>
#include <QThread>
#include <QtConcurrentRun>
#include <QLabel>
#include "class_rss_form.h"
#include "class_ref_firm.h"
#include "class_ref_client.h"
#include "class_ref_tarif.h"
#include "class_load_pp.h"
#include "class_ref_pp.h"
#include "class_ref_balans_client.h"
#include "class_ref_balans_rs.h"
#include "class_ref_client_pay.h"
#include "class_manage_users.h"
#include "class_report_client.h"
#include "class_settings.h"
#include "class_ref_auto_actions.h"

namespace Ui {
class general_window;
}

class general_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit general_window(QWidget *parent, QSqlDatabase *db1);
    ~general_window();

private:
    Ui::general_window *ui;
    class_ref_firm *ref_firm;
    class_rss_form *rss_form;
    class_ref_client *ref_client;
    class_ref_tarif *ref_tarif;
    class_load_pp *load_pp;
    //QThread *thread;
    class_ref_balans_client *balans_client;
    class_ref_balans_rs *balans_rs;
    class_ref_client_pay *client_pay;
    class_ref_pp *pps;
    class_manage_users *manage_users;
    class_report_client *report_client;
    class_ref_auto_actions *auto_actions;
    QWidget *tab;
    QSqlQuery *query;
    QLabel lb;
    QSqlDatabase *db;
    int i_rs, i_firm, i_client, i_tarif, i_load_pp, i_pp, user_id,
    i_balans_client, i_balans_rs, i_client_pay, i_client_group, i_manage_users,
    i_balans_group, i_report_client, i_auto_actions;
    class_settings *settings;

public slots:
    void show_rss_form();
    void show_ref_firm();
    void show_ref_client();
    void slot_show_ref_tarif();
    void slot_show_load_pp();
    void slot_show_ref_pp();
    void slot_show_ref_balans_rs();
    void slot_show_ref_balans_client();
    void slot_show_ref_client_pay();
    void slot_show_manage_users();
    void slot_show_report_client();
    void slot_show_auto_actions();
    void slot_del_tab(int);
    void slot_refresh_tab(int tab);
    void slot_set_enable_menu(int id);
    void slot_set_status_bar_data(QString);
    void slot_show_settings_window();
    void get_access();

};

#endif // GENERAL_WINDOW_H
