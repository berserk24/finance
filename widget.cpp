#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::UserScope, "finance", "finance", this);

    get_settings();

    show();

    ui->pushButton_enter->setFocus();
    if (ui->lineEdit_passwd->text() == "") ui->lineEdit_passwd->setFocus();
    if (ui->lineEdit_login->text() == "") ui->lineEdit_login->setFocus();

    db = new QSqlDatabase;

    connect(ui->pushButton_enter, SIGNAL(clicked()), this, SLOT(slot_login()));
    connect(ui->lineEdit_login, SIGNAL(returnPressed()), ui->lineEdit_passwd, SLOT(setFocus()));
    connect(ui->lineEdit_passwd, SIGNAL(returnPressed()), ui->pushButton_enter, SLOT(click()));
}

void Widget::get_settings()
{
    settings->beginGroup("General");
    ui->lineEdit_login->setText(settings->value("user_name", "").toString());
    ui->_save_plogin->setChecked(settings->value("save_login", "false").toBool());
    settings->endGroup();
}

void Widget::set_settings()
{
    if (ui->_save_plogin->isChecked())
    {
        settings->beginGroup("General");
        settings->setValue("user_name", ui->lineEdit_login->text());
        settings->setValue("save_login", ui->_save_plogin->isChecked());
        settings->endGroup();
        settings->sync();
    }
    else
    {
        settings->beginGroup("General");
        settings->setValue("user_name", "");
        settings->setValue("save_login", ui->_save_plogin->isChecked());
        settings->endGroup();
        settings->sync();
    }
}

void Widget::create_database()
{
    if (db->tables().size() < 15)
    {
        //Создаём роль
        {
            query->exec("CREATE ROLE full_access");
        }

        //Создаём таблицу версии БД
        {
            query->exec("CREATE TABLE version "
                        "(id SMALLINT NOT NULL)");
            query->exec("GRANT ALL ON version TO full_access");
            query->exec("INSERT INTO version (id) VALUES (1)");
        }

        //Создаём таблицу да/нет
        {
            query->exec("   CREATE TABLE yes_no ("
                                        "id SMALLINT NOT NULL, "
                                        "data VARCHAR(3) NOT NULL, "

                                        "PRIMARY KEY (id), "
                                        "UNIQUE      (data));"
                        );
            query->exec("GRANT ALL ON yes_no TO full_access");
        }
        {
            query->exec("   INSERT INTO    yes_no   (id, data) "
                            "VALUES (0, 'Нет')");
            query->exec("   INSERT INTO    yes_no   (id, data) "
                            "VALUES (1, 'Да')");
        }

        //Создаём таблицу типов движений платёжек
        {
            query->exec("CREATE TABLE pp_in_out ("
                                    "id SMALLINT NOT NULL, "
                                    "data VARCHAR(7) NOT NULL, "
                                    "PRIMARY KEY (id), "
                                    "UNIQUE (data));"
                        );
            query->exec("GRANT ALL ON pp_in_out TO full_access");
        }

        //Создаём типы платёжек
        {
            query->exec("   INSERT INTO    pp_in_out   (id, data)"
                            "VALUES (1, 'Расход')");
            query->exec("   INSERT INTO    pp_in_out   (id, data)"
                            "VALUES (2, 'Приход')");
            query->exec("   INSERT INTO    pp_in_out   (id, data)"
                            "VALUES (3, 'Перевод')");
        }

        //Создаём таблицу типов платёжек
        {
            query->exec("CREATE TABLE pp_type ("
                                    "id SMALLINT NOT NULL, "
                                    "data VARCHAR(19) NOT NULL, "
                                    "PRIMARY KEY (id), "
                                    "UNIQUE (data));"
                        );
            query->exec("GRANT ALL ON pp_type TO full_access");
        }

        //Создаём типы платёжек
        {
            query->exec("   INSERT INTO    pp_type   (id, data)"
                            "VALUES (0, 'Платежное поручение')");
            query->exec("   INSERT INTO    pp_type   (id, data)"
                            "VALUES (1, 'Мемориальный ордер')");
            query->exec("   INSERT INTO    pp_type   (id, data)"
                            "VALUES (2, 'Банковский ордер')");
        }

        //Создаём таблицу фирм
        {
            query->exec("CREATE TABLE firms ( "
                                    "id INTEGER NOT NULL, "
                                    "name VARCHAR(70) NOT NULL, "
                                    "inn VARCHAR(12) NOT NULL, "
                                    "stroy SMALLINT NOT NULL, "
                                    "PRIMARY KEY (id), "
                                    "UNIQUE (name), "
                                    "UNIQUE (inn))"
                        );
            query->exec("CREATE GENERATOR gen_firms_id");
            query->exec("SET GENERATOR gen_firms_id TO 0");
            query->exec("CREATE TRIGGER tr_firms FOR firms ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(GEN_FIRMS_ID, 1);END");
            query->exec("GRANT ALL ON firms TO full_access");
        }

        //Создаём фирму 0
        {
            query->exec("INSERT INTO    firms "
                                        "(id, name, inn, stroy)  "
                        "VALUES ('0', 'Нет', '000000000000', 0 )"
                        );
        }

        //Создаём таблицу расчётных счетов
        {
            query->exec("CREATE TABLE rss "
                        "(id INTEGER NOT NULL, "
                        " name VARCHAR(50) NOT NULL, "
                        " bik VARCHAR(9) NOT NULL, "
                        " number VARCHAR(20) NOT NULL, "
                        " firm  INTEGER NOT NULL, "
                        " start_balans NUMERIC(18,2) NOT NULL, "
                        "PRIMARY KEY (id), "
                        "UNIQUE (bik, number), "
                        "UNIQUE (name), "
                        "FOREIGN KEY (firm) REFERENCES firms(id) "
                        "ON DELETE CASCADE "
                        "ON UPDATE CASCADE)"
                        );
            query->exec("CREATE GENERATOR gen_rss_id");
            query->exec("SET GENERATOR gen_rss_id TO 0");
            query->exec("CREATE TRIGGER tr_rss FOR rss ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(GEN_RSS_ID, 1);END");
            query->exec("GRANT ALL ON rss TO full_access");
        }

        //Создаём таблицу баланса РС
        {
            query->exec("CREATE TABLE rss_balans ( "
                                    "id INTEGER NOT NULL, "
                                    "last_date DATE NOT NULL, "
                                    "balans NUMERIC(18,2) NOT NULL, "
                                    "FOREIGN KEY (id) REFERENCES rss(id) "
                                    "ON DELETE CASCADE "
                                    "ON UPDATE CASCADE, "
                                    "UNIQUE (id))"
                        );
            query->exec("GRANT ALL ON rss_balans TO full_access");
        }

        //Создаём таблицу настроек
        {
            query->exec("CREATE TABLE settings ( "
                                    "id INTEGER, "
                                    "id_user INTEGER, "
                                    "name VARCHAR(20) NOT NULL, "
                                    "data VARCHAR(10), "
                                    "PRIMARY KEY (id), "
                                    "UNIQUE (id_user, name))"
                        );
            query->exec("GRANT ALL ON settings TO full_access");
        }

        //Создаём таблицу контрагентов
        {
            query->exec("CREATE TABLE clients ( "
                                    "id INTEGER PRIMARY KEY NOT NULL, "
                                    "name VARCHAR(30) NOT NULL, "
                                    "mail VARCHAR(50) NOT NULL, "
                                    "tarif SMALLINT NOT NULL, "
                                    "t_obnal NUMERIC(3,2) NOT NULL, "
                                    "t_trans_in NUMERIC(3,2) NOT NULL, "
                                    "t_trans_in_s NUMERIC(3,2) NOT NULL, "
                                    "t_trans_out NUMERIC(3,2) NOT NULL, "
                                    "t_kred NUMERIC(3,2) NOT NULL, "
                                    "t_nalic NUMERIC(3,2) NOT NULL, "
                                    "UNIQUE (name))"
                        );
            query->exec("CREATE GENERATOR gen_clients_id");
            query->exec("SET GENERATOR gen_clients_id TO 0");
            query->exec("CREATE TRIGGER tr_clients FOR clients ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(GEN_CLIENTS_ID, 1);END");
            query->exec("GRANT ALL ON clients TO full_access");
        }

        //Создаём клиента для %ов
        {
            query->exec("INSERT INTO    clients"
                        "   (id, name, mail, tarif, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic) "
                        "VALUES ( 0, 'Наш счёт', 0, 0, 0, 0, 0, 0, 0, 0 )");
        }

        //Создаём таблицу баланса клиентов
        {
            query->exec("CREATE TABLE client_balans ( "
                                    "id INTEGER NOT NULL, "
                                    "balans NUMERIC(18,2) NOT NULL, "
                                    "FOREIGN KEY (id) REFERENCES clients(id) "
                                    "ON DELETE CASCADE "
                                    "ON UPDATE CASCADE, "
                                    "PRIMARY KEY (id))"
                        );
            query->exec("GRANT ALL ON client_balans TO full_access");
        }

        //Создаём баланс 0ого клиента
        {
            query->exec("INSERT INTO    client_balans"
                        "   (id, balans ) "
                        "VALUES ( 0, 0 )");
        }

        //Создаём таблицу тарифов
        {
            query->exec("CREATE TABLE tarifs ( "
                                    "id INTEGER PRIMARY KEY NOT NULL, "
                                    "name VARCHAR(30) NOT NULL, "
                                    "def SMALLINT NOT NULL, "
                                    "t_obnal NUMERIC(3,2) NOT NULL, "
                                    "t_trans_in NUMERIC(3,2) NOT NULL, "
                                    "t_trans_in_s NUMERIC(3,2) NOT NULL, "
                                    "t_trans_out NUMERIC(3,2) NOT NULL, "
                                    "t_kred NUMERIC(3,2) NOT NULL, "
                                    "t_nalic NUMERIC(3,2) NOT NULL, "
                                    "UNIQUE (name)) "
                    );
            query->exec("CREATE GENERATOR gen_tarifs_id");
            query->exec("SET GENERATOR gen_tarifs_id TO 0");
            query->exec("CREATE TRIGGER tr_tarifs FOR tarifs ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(gen_tarifs_id, 1);END");
            query->exec("GRANT ALL ON tarifs TO full_access");
        }

        //Создаём 0ой тариф
        {
            query->exec("   INSERT INTO tarifs "
                                        "(id, name, def, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic) "
                                    "VALUES ("
                                            "0 , 'Индивидуальный', 0, 0, 0, 0, 0, 0, 0)"
                        );
        }

        //Создаём таблицу платёжек
        {
            query->exec("CREATE TABLE pp ("
                                    "id INTEGER PRIMARY KEY NOT NULL, "
                                    "rs_id INTEGER  NOT NULL, "
                                    "client_id INTEGER, "
                                    "type_pp SMALLINT NOT NULL, "
                                    "type_doc SMALLINT NOT NULL, "
                                    "num INTEGER NOT NULL, "
                                    "date_pp DATE NOT NULL, "
                                    "date_oper DATE NOT NULL, "
                                    "sum_pp  NUMERIC(18,2) NOT NULL, "
                                    "ticket_date DATE, "
                                    "ticket_time TIME, "
                                    "ticket_value VARCHAR(500), "
                                    "payer_count VARCHAR(20) NOT NULL, "
                                    "date_out DATE, "
                                    "payer VARCHAR(300), "
                                    "payer_inn VARCHAR(12) NOT NULL, "
                                    "payer1 VARCHAR(200), "
                                    "payer2 VARCHAR(200), "
                                    "payer3 VARCHAR(200), "
                                    "payer4 VARCHAR(200), "
                                    "payer_rs VARCHAR(20), "
                                    "payer_bank1 VARCHAR(200), "
                                    "payer_bank2 VARCHAR(200), "
                                    "payer_bik VARCHAR(9), "
                                    "payer_ks VARCHAR(20), "
                                    "receiver_count VARCHAR(20) NOT NULL, "
                                    "date_in DATE, "
                                    "receiver VARCHAR(300), "
                                    "receiver_inn VARCHAR(12) NOT NULL, "
                                    "receiver1 VARCHAR(200), "
                                    "receiver2 VARCHAR(200), "
                                    "receiver3 VARCHAR(200), "
                                    "receiver4 VARCHAR(200), "
                                    "receiver_rs VARCHAR(20), "
                                    "receiver_bank1 VARCHAR(200), "
                                    "receiver_bank2 VARCHAR(200), "
                                    "receiver_bik VARCHAR(9), "
                                    "receiver_ks VARCHAR(20), "
                                    "type_pay VARCHAR(100), "
                                    "type_trans VARCHAR(2), "
                                    "code VARCHAR(25), "
                                    "dest_pay VARCHAR(600), "
                                    "dest_pay1 VARCHAR(100), "
                                    "dest_pay2 VARCHAR(100), "
                                    "dest_pay3 VARCHAR(100), "
                                    "dest_pay4 VARCHAR(100), "
                                    "dest_pay5 VARCHAR(100), "
                                    "dest_pay6 VARCHAR(100), "
                                    "state_sender VARCHAR(2), "
                                    "payer_kpp VARCHAR(9), "
                                    "receiver_kpp VARCHAR(9), "
                                    "pokazatel_kbk VARCHAR(20), "
                                    "okato VARCHAR(11), "
                                    "pokazatel_osnovaniya VARCHAR(2), "
                                    "pokazatel_period VARCHAR(10), "
                                    "pokazatel_num VARCHAR(100), "
                                    "pokazatel_date DATE, "
                                    "pokazatel_type VARCHAR(2), "
                                    "ocherednost VARCHAR(2), "
                                    "srok_accepta INTEGER, "
                                    "type_akkred VARCHAR(200), "
                                    "srok_pay DATE, "
                                    "usl_pay1 VARCHAR(100), "
                                    "usl_pay2 VARCHAR(100), "
                                    "usl_pay3 VARCHAR(100), "
                                    "pay_po_predst VARCHAR(100), "
                                    "dop_usl VARCHAR(100), "
                                    "num_scheta_postav VARCHAR(100), "
                                    "date_send_doc DATE, "
                                    "UNIQUE (num, rs_id, type_pp, date_pp, type_doc, date_oper, sum_pp, payer_count, receiver_count), "
                                    "FOREIGN KEY (rs_id) REFERENCES rss(id) "
                                    "ON DELETE CASCADE "
                                    "ON UPDATE CASCADE)"
                        );
            query->exec("CREATE INDEX i_type_doc ON pp(type_doc)");
            query->exec("CREATE INDEX i_date_oper ON pp(date_oper)");
            query->exec("CREATE INDEX i_date ON pp(date)");
            query->exec("CREATE INDEX i_payer1 ON pp(payer1)");
            query->exec("CREATE INDEX i_receiver1 ON pp(receiver1)");
            query->exec("CREATE INDEX i_rs_id ON pp(rs_id)");
            query->exec("CREATE INDEX i_client_id ON pp(client_id)");
            query->exec("CREATE INDEX i_type ON pp(type)");

            query->exec("CREATE GENERATOR gen_pp_id");
            query->exec("SET GENERATOR gen_pp_id TO 0");
            query->exec("CREATE TRIGGER tr_pp FOR pp ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(gen_pp_id, 1);END");
            query->exec("GRANT ALL ON pp TO full_access");

        }

        //Создаём таблицу движений по счёту клиентов
        {
            query->exec("CREATE TABLE clients_operations ("
                                    "id INTEGER PRIMARY KEY NOT NULL, "
                                    "id_client INTEGER NOT NULL, "
                                    "to_client_id INTEGER, "
                                    "id_pp INTEGER, "
                                    "type_pp INTEGER NOT NULL, "
                                    "text VARCHAR(600) NOT NULL, "
                                    "date_oper DATE NOT NULL, "
                                    "summ NUMERIC(18,2) NOT NULL, "
                                    "margin NUMERIC(18,2) NOT NULL, "
                                    "FOREIGN KEY (id_pp) REFERENCES pp(id) "
                                    "ON DELETE CASCADE "
                                    "ON UPDATE CASCADE, "
                                    "UNIQUE (id_pp))"
                        );
            query->exec("CREATE GENERATOR gen_clients_operations_id");
            query->exec("SET GENERATOR gen_clients_operations_id TO 0");
            query->exec("CREATE TRIGGER tr_clients_operations FOR clients_operations ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(gen_clients_operations_id, 1);END");
            query->exec("GRANT ALL ON clients_operations TO full_access");
        }

        //Создаём таблицу соотношения платёжек клиенту
        {
            query->exec("CREATE TABLE pp_to_client ("
                                    "pp_id INTEGER PRIMARY KEY NOT NULL, "
                                    "client_id INTEGER NOT NULL, "
                                    "FOREIGN KEY (pp_id) REFERENCES pp(id) "
                                    "ON DELETE CASCADE "
                                    "ON UPDATE CASCADE, "
                                    "UNIQUE (pp_id, client_id))"
                        );
            query->exec("GRANT ALL ON pp_to_client TO full_access");
        }

        //Создаём таблицу прав пользоваталей
        {
            query->exec("CREATE TABLE users_access ("
                                    "id VARCHAR(18) PRIMARY KEY NOT NULL, "
                                    "ref SMALLINT NOT NULL, "
                                    "load_pp SMALLINT NOT NULL, "
                                    "work_pp SMALLINT NOT NULL, "
                                    "report SMALLINT NOT NULL, "
                                    "client SMALLINT NOT NULL) "
                        );
            query->exec("GRANT ALL ON users_access TO full_access");
        }

        //Права пользователя admin
        {
            query->exec("INSERT INTO users_access (id, ref, load_pp, work_pp, report, client) "
                        "VALUES ('SYSDBA', 1, 1, 1, 1, 1)");
        }

    }

    query->exec("SELECT * FROM version");
    query->first();
    if (query->value(0).toInt() < 2)
    {
        query->clear();
        //Создаём таблицу сохранённых действий
        {
            query->exec("CREATE TABLE save_actions ( "
                                    "id INTEGER NOT NULL, "
                                    "inn VARCHAR(12) NOT NULL, "
                                    "firm_name VARCHAR(300) NOT NULL, "
                                    "client_id SMALLINT NOT NULL, "
                                    "type_pp SMALLINT NOT NULL, "
                                    "percent NUMERIC(18,2), "
                                    "PRIMARY KEY (id), "
                                    "UNIQUE (inn, type_pp, client_id))"
                        );
            query->exec("CREATE GENERATOR gen_save_actions_id");
            query->exec("SET GENERATOR gen_save_actions_id TO 0");
            query->exec("CREATE TRIGGER tr_save_actions FOR save_actions ACTIVE BEFORE INSERT POSITION 0 AS BEGIN if (NEW.ID is NULL) then NEW.ID = GEN_ID(gen_save_actions_id, 1);END");
            query->exec("GRANT ALL ON save_actions TO full_access");
        }
        query->exec("UPDATE version SET id = 2");
    }

    this->hide();
    gen_window = new general_window(0, db);
    gen_window->show();
}

void Widget::slot_login()
{
    *db = QSqlDatabase::addDatabase("QIBASE");
    db->setDatabaseName(QApplication::applicationDirPath()+"/database.fdb");
    db->setHostName("localhost");
    db->setUserName(ui->lineEdit_login->text());
    db->setPassword(ui->lineEdit_passwd->text());
    if (ui->lineEdit_login->text() != "SYSDBA")
        db->setConnectOptions("ISC_DPB_SQL_ROLE_NAME=FULL_ACCESS");
    if (!db->open())
    {
        QString error_db = db->lastError().text();
        QMessageBox::critical(this, "error", error_db);
        db->close();
        return;
    }
    else
    {
        query = new QSqlQuery(*db);
        create_database();
        query->exec(" SELECT current_role FROM rdb$database");
        query->first();
        //gen_window = new general_window(this, 0);
        hide();
        set_settings();
        //gen_window->show();
        return;
    }
}

Widget::~Widget()
{
    delete ui;
}
