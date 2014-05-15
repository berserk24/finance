#include "general_window.h"
#include "ui_general_window.h"

general_window::general_window(QWidget *parent, QSqlDatabase *db1) :
    QMainWindow(parent),
    ui(new Ui::general_window)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    db = db1;

    user_id = 0;

    //slot_set_enable_menu(user_id);

    i_rs = i_firm = i_client = i_tarif = i_load_pp = i_pp = i_balans_client =
            i_balans_rs = i_client_pay = i_manage_users = i_report_client = i_auto_actions = -1;

    statusBar()->addWidget(&lb);

    ui->tabWidget->setTabsClosable(true);

    get_access();

    //Показать справочник расчётных счетов
    connect(ui->action_show_rss, SIGNAL(triggered()), SLOT(show_rss_form()));

    //Показать справочник организаций
    connect(ui->action_ref_firm, SIGNAL(triggered()), SLOT(show_ref_firm()));

    //Показать справочник контрагентов
    connect(ui->action_ref_client, SIGNAL(triggered()), SLOT(show_ref_client()));

    //Показать справочник тарифов
    connect(ui->action_ref_tarif, SIGNAL(triggered()), SLOT(slot_show_ref_tarif()));

    //Выход из программы
    connect(ui->action_exit, SIGNAL(triggered()), SLOT(close()));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(slot_del_tab(int)));

    connect(ui->action_load_pp, SIGNAL(triggered()), SLOT(slot_show_load_pp()));

    //Показываем платёжки
    connect(ui->action_pp, SIGNAL(triggered()), SLOT(slot_show_ref_pp()));

    //Показываем баланс счетов клиентов
    connect(ui->action_balans_client, SIGNAL(triggered()), SLOT(slot_show_ref_balans_client()));

    //Показываем баланс расчётных счетов
    connect(ui->action_balans_rs, SIGNAL(triggered()), SLOT(slot_show_ref_balans_rs()));

    //Показываем движения по счетам клиентов
    connect(ui->action_client_pays, SIGNAL(triggered()), SLOT(slot_show_ref_client_pay()));

    //Показываем пользователей
    connect(ui->action_users, SIGNAL(triggered()), SLOT(slot_show_manage_users()));

    //Показываем отчёт по клиенту
    connect(ui->action_rep_client, SIGNAL(triggered()), SLOT(slot_show_report_client()));

    //Обновляем данные внутри вкладки
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(slot_refresh_tab(int)));

    //Показываем настройки
    connect(ui->action_setting, SIGNAL(triggered()), SLOT(slot_show_settings_window()));

    //Показываем настройки
    connect(ui->action_auto_actions, SIGNAL(triggered()), SLOT(slot_show_auto_actions()));

    //Обновляем справочник банков
    connect(ui->action_update_ref_banks, SIGNAL(triggered()), SLOT(slot_show_update_ref_banks()));
}

void general_window::slot_show_update_ref_banks()
{
    update_banks = new class_update_ref_banks;
}

void general_window::get_access()
{
    query = new QSqlQuery;
    query->exec("SELECT * FROM users_access WHERE LOWER(id) = LOWER(CURRENT_USER)");
    query->first();
    if (query->value(1).toInt() == 1)
    {
        ui->menu_ref->setEnabled(true);
    }
    else
    {
        ui->menu_ref->setEnabled(false);
    }
    if (query->value(2).toInt() == 1)
    {
        ui->action_load_pp->setEnabled(true);
        ui->action_balans_rs->setEnabled(true);
    }
    else
    {
        ui->action_load_pp->setEnabled(false);
        ui->action_balans_rs->setEnabled(false);
    }
    if (query->value(3).toInt() == 1)
    {
        ui->action_pp->setEnabled(true);
        ui->action_client_pays->setEnabled(true);
    }
    else
    {
        ui->action_pp->setEnabled(false);
        ui->action_client_pays->setEnabled(false);
    }
    if (query->value(4).toInt() == 1)
    {
        ui->action_rep_client->setEnabled(true);
    }
    else
    {
        ui->action_rep_client->setEnabled(false);
    }
    if (query->value(5).toInt() == 1)
    {
        ui->action_balans_client->setEnabled(true);
    }
    else
    {
        ui->action_balans_client->setEnabled(false);
    }
    if (query->value(0).toString() == "SYSDBA")
    {
        ui->action_users->setEnabled(true);
    }
    else
    {
        ui->action_users->setEnabled(false);
    }
    query->clear();
    delete query;
}

//Показываем настройки
void general_window::slot_show_settings_window()
{
    settings = new class_settings();
    settings->show();
}

void general_window::slot_set_status_bar_data(QString str)
{
    lb.setText(str);
}

void general_window::slot_del_tab(int index)
{
    if (ui->tabWidget->tabText(index) == "Расчётные счета") i_rs = -1;
    if (ui->tabWidget->tabText(index) == "Организации") i_firm = -1;
    if (ui->tabWidget->tabText(index) == "Контрагенты") i_client = -1;
    if (ui->tabWidget->tabText(index) == "Тарифы") i_tarif = -1;
    if (ui->tabWidget->tabText(index) == "Загрузка ПП") i_load_pp = -1;
    if (ui->tabWidget->tabText(index) == "Платёжные поручения")
    {
        i_pp = -1;
        ui->menu_view->setEnabled(false);
    }
    if (ui->tabWidget->tabText(index) == "Баланс счетов контрагентов")
    {
        slot_set_status_bar_data("");
        i_balans_client = -1;
    }
    if (ui->tabWidget->tabText(index) == "Баланс расчётных счетов")
    {
        slot_set_status_bar_data("");
        i_balans_rs = -1;
    }
    if (ui->tabWidget->tabText(index) == "Движения по счетам контрагентов") i_client_pay = -1;
    if (ui->tabWidget->tabText(index) == "Управление пользователями") i_manage_users = -1;
    if (ui->tabWidget->tabText(index) == "Отчёт по контрагенту") i_report_client = -1;
    if (ui->tabWidget->tabText(index) == "Автоматические действия") i_auto_actions = -1;
    ui->tabWidget->removeTab(index);
}

void general_window::show_rss_form()
{
    if (i_rs == -1)
    {
        rss_form = new class_rss_form(0, db);
        i_rs = ui->tabWidget->addTab(rss_form, QString("Расчётные счета"));
        ui->tabWidget->setCurrentIndex(i_rs);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(rss_form));
    }
}

void general_window::slot_show_auto_actions()
{
    if (i_auto_actions == -1)
    {
        auto_actions = new class_ref_auto_actions(0, db);
        i_auto_actions = ui->tabWidget->addTab(auto_actions, QString("Автоматические действия"));
        ui->tabWidget->setCurrentIndex(i_auto_actions);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(auto_actions));
    }
}

void general_window::show_ref_firm()
{
    if (i_firm == -1)
    {
        ref_firm = new class_ref_firm;
        i_firm = ui->tabWidget->addTab(ref_firm, QString("Организации"));
        ui->tabWidget->setCurrentIndex(i_firm);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ref_firm));
    }
}

void general_window::show_ref_client()
{
    if (i_client == -1)
    {
        ref_client = new class_ref_client(0, db);
        i_client = ui->tabWidget->addTab(ref_client, QString("Контрагенты"));
        ui->tabWidget->setCurrentIndex(i_client);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ref_client));
    }
}

void general_window::slot_show_ref_tarif()
{
    if (i_tarif == -1)
    {
        ref_tarif = new class_ref_tarif;
        i_tarif = ui->tabWidget->addTab(ref_tarif, QString("Тарифы"));
        ui->tabWidget->setCurrentIndex(i_tarif);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ref_tarif));
    }
}

void general_window::slot_show_load_pp()
{
    if (i_load_pp == -1)
    {
        load_pp = new class_load_pp(0, db);
        i_load_pp = ui->tabWidget->addTab(load_pp, QString("Загрузка ПП"));
        ui->tabWidget->setCurrentIndex(i_load_pp);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(load_pp));
    }
}

void general_window::slot_show_ref_pp()
{
    if (i_pp == -1)
    {
        pps = new class_ref_pp(0, db);
        i_pp = ui->tabWidget->addTab(pps, QString("Платёжные поручения"));
        ui->tabWidget->setCurrentIndex(i_pp);
        connect(pps, SIGNAL(show_ref_client()), SLOT(show_ref_client()));
        ui->menu_view->setEnabled(true);
        connect(ui->action_column, SIGNAL(triggered()), pps, SLOT(slot_show_settings_table()));
        connect(pps, SIGNAL(signal_send_sum_pp(QString)), SLOT(slot_set_status_bar_data(QString)));
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(pps));
    }
}

void general_window::slot_show_ref_balans_client()
{
    if (i_balans_client == -1)
    {
        balans_client = new class_ref_balans_client(0, db);
        i_balans_client = ui->tabWidget->addTab(balans_client, QString("Баланс счетов контрагентов"));
        ui->tabWidget->setCurrentIndex(i_balans_client);
        connect(balans_client, SIGNAL(signal_send_sum_client(QString)), SLOT(slot_set_status_bar_data(QString)));
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(balans_client));
    }
}

void general_window::slot_show_ref_balans_rs()
{
    if (i_balans_rs == -1)
    {
        balans_rs = new class_ref_balans_rs;
        i_balans_rs = ui->tabWidget->addTab(balans_rs, QString("Баланс расчётных счетов"));
        ui->tabWidget->setCurrentIndex(i_balans_rs);
        connect(balans_rs, SIGNAL(signal_send_sum_rs(QString)), SLOT(slot_set_status_bar_data(QString)));
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(balans_rs));
    }
}

void general_window::slot_show_ref_client_pay()
{
    if (i_client_pay == -1)
    {
        client_pay = new class_ref_client_pay(0, db);
        i_client_pay = ui->tabWidget->addTab(client_pay, QString("Движения по счетам контрагентов"));
        ui->tabWidget->setCurrentIndex(i_client_pay);
        connect(client_pay, SIGNAL(signal_send_sum_opers(QString)), SLOT(slot_set_status_bar_data(QString)));
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(client_pay));
    }
}

void general_window::slot_show_report_client()
{
    if (i_report_client == -1)
    {
        report_client = new class_report_client;
        i_report_client = ui->tabWidget->addTab(report_client, QString("Отчёт по контрагенту"));
        ui->tabWidget->setCurrentIndex(i_report_client);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(report_client));
    }
}

void general_window::slot_show_manage_users()
{
    if (i_manage_users == -1)
    {
        manage_users = new class_manage_users(0, db);
        i_manage_users = ui->tabWidget->addTab(manage_users, QString("Управление пользователями"));
        ui->tabWidget->setCurrentIndex(i_manage_users);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(manage_users));
    }
}


void general_window::slot_refresh_tab(int tab)
{
    slot_set_status_bar_data("");
    if (i_client > -1)
    {
        if (ui->tabWidget->indexOf(ref_client) == tab)
        {
            ref_client->select_tarif();
        }
    }
    if (i_rs > -1)
    {
        if (ui->tabWidget->indexOf(rss_form) == tab)
        {
            rss_form->select_firm();
            rss_form->select_table();
        }
    }
    if (i_client_pay > -1)
    {
        if (ui->tabWidget->indexOf(client_pay) == tab)
        {
            client_pay->select_client();
        }
    }
    if (i_balans_client > -1)
    {
        if (ui->tabWidget->indexOf(balans_client) == tab)
        {
            balans_client->slot_select_client();
            balans_client->slot_select_table();
        }
    }
    if (i_balans_rs > -1)
    {
        if (ui->tabWidget->indexOf(balans_rs) == tab)
        {
            balans_rs->slot_select_table();
        }
    }
    if (i_pp > -1)
    {
        if (ui->tabWidget->indexOf(pps) == tab)
        {
            pps->slot_select_client();
            pps->slot_select_client_filter();
            pps->load_rss();
            pps->slot_select_pp();
            ui->menu_view->setEnabled(true);
        }
        else
        {
            ui->menu_view->setEnabled(false);
        }
    }
    if (i_auto_actions > -1)
    {
        if (ui->tabWidget->indexOf(auto_actions) == tab)
        {
            auto_actions->slot_select_table();
        }
    }
    else
    {
        ui->menu_view->setEnabled(false);
    }
}

general_window::~general_window()
{
    delete ui;
}

//Включаем выключаем пункты меню
void general_window::slot_set_enable_menu(int id)
{
    query = new QSqlQuery;

    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->prepare("SELECT * FROM users_access WHERE id = ?");
    query->addBindValue(id);
    query->exec();
    query->first();
    ui->menu_ref->setEnabled(query->value(1).toBool());
    ui->action_load_pp->setEnabled(query->value(2).toBool());
    if (query->value(2).toBool() or query->value(3).toBool())
        ui->action_pp->setEnabled(true);
    ui->action_balans_rs->setEnabled(query->value(2).toBool());
    ui->action_client_pays->setEnabled(query->value(3).toBool());
    ui->action_balans_client->setEnabled(query->value(5).toBool());
    ui->menu_report->setEnabled(query->value(4).toBool());
    if (id == 0) ui->menu_settings->setEnabled(query->value(2).toBool());
    query->clear();
    query->exec("COMMIT");
    delete query;
}
