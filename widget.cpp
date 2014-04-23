#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::UserScope, "obnal", "obnal", this);

    get_settings();

    db = new QSqlDatabase;

    hash = new QCryptographicHash(QCryptographicHash::Md5);

    if (func_connect_db())
    {
        create_database();
        show();
    }
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

bool Widget::func_connect_db()
{
    *db = QSqlDatabase::addDatabase("QSQLITE");
    db->setDatabaseName(QApplication::applicationDirPath()+"/database.db");
    if (!db->open())
    {
        QString error_db = db->lastError().text();
        QMessageBox::critical(this, "error", error_db);
        db->close();
        exit(1);
        return false;
    }
    else
    {
        query = new QSqlQuery(*db);
        return true;
    }
}

void Widget::create_database()
{
    if (db->tables().size() < 11)
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        //Создаём таблицу пользоваталей
        {
            query->exec("CREATE TABLE 'main'.'users' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'passwd' TEXT, \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём пользователя admin
        {
            query->exec("   INSERT INTO    users   ( \
                                                    id, name, passwd  \
                                                ) \
                        VALUES ( \
                                            '0', 'admin', '" + func_gen_hash("admin") + \
                                            "' )" \
                    );
        }

        //Создаём таблицу прав пользоваталей
        {
            query->exec("CREATE TABLE 'main'.'users_access' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'ref' TEXT NOT NULL, \
                                    'load_pp' TEXT NOT NULL, \
                                    'work_pp' TEXT NOT NULL, \
                                    'report' TEXT NOT NULL, \
                                    'client' TEXT NOT NULL) \
                    ");
        }

        //Права пользователя admin
        {
            query->exec("   INSERT INTO    users_access   ( \
                                                    id, ref, load_pp, work_pp, report, client  \
                                                ) \
                        VALUES ( \
                                            '0', 'true', 'true', 'true', 'true', 'true' \
                                            ) \
                    ");
        }

        //Создаём таблицу да/нет
        {
            query->exec("CREATE TABLE 'main'.'yes_no' ( \
                                    'id' TEXT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    UNIQUE (id COLLATE NOCASE ASC)) \
                    ");
        }
        {
            query->exec("   INSERT INTO    yes_no   (id, name)"
                            "VALUES ('false', 'Нет')");
            query->exec("   INSERT INTO    yes_no   (id, name)"
                            "VALUES ('true', 'Да')");
        }

        //Создаём таблицу типов движений платёжек
        {
            query->exec("CREATE TABLE 'main'.'pp_in_out' ( \
                                    'id' TEXT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    UNIQUE (id COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём типы платёжек
        {
            query->exec("   INSERT INTO    pp_in_out   (id, name)"
                            "VALUES (1, 'Расход')");
            query->exec("   INSERT INTO    pp_in_out   (id, name)"
                            "VALUES (2, 'Приход')");
            query->exec("   INSERT INTO    pp_in_out   (id, name)"
                            "VALUES (3, 'Перевод')");
        }

        //Создаём таблицу типов движений платёжек
        {
            query->exec("CREATE TABLE 'main'.'pp_type' ( \
                                    'id' TEXT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    UNIQUE (id COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём таблицу настроек
        {
            query->exec("CREATE TABLE 'main'.'settings' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'value' TEXT NULL, \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Колонки окна ПП
        {
            query->exec("INSERT INTO settings (value, name) VALUES ('1111111111', 'pp_collumn')");
        }

        //Создаём типы платёжек
        {
            query->exec("   INSERT INTO    pp_type   (id, name)"
                            "VALUES (0, 'Платежное поручение')");
            query->exec("   INSERT INTO    pp_type   (id, name)"
                            "VALUES (1, 'Мемориальный ордер')");
            query->exec("   INSERT INTO    pp_type   (id, name)"
                            "VALUES (2, 'Банковский ордер')");
        }

        //Создаём таблицу фирм
        {
            query->exec("CREATE TABLE 'main'.'firm' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'inn' TEXT NOT NULL, \
                                    'stroy' TEXT NOT NULL, \
                                    UNIQUE (name COLLATE NOCASE ASC), \
                                    UNIQUE (inn COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём фирму 0
        {
            query->exec("   INSERT INTO    firm   ( \
                                                    id, name, inn, stroy  \
                                                ) \
                        VALUES ( \
                                            '0', 'Нет', '000000000000', 'false' )" \
                    );
        }

        //Создаём таблицу расчётных счетов
        {
            query->exec("CREATE TABLE 'main'.'rss' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'bik' TEXT NOT NULL, \
                                    'number' TEXT NOT NULL, \
                                    'firm'  TEXT NOT NULL, \
                                    UNIQUE (bik, number COLLATE NOCASE ASC), \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём таблицу баланса РС
        {
            query->exec("CREATE TABLE 'main'.'rss_balans' ( \
                                    'id' TEXT NOT NULL, \
                                    'date' TEXT NOT NULL, \
                                    'balans' TEXT NOT NULL, \
                                    UNIQUE (id COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём таблицу контрагентов
        {
            query->exec("CREATE TABLE 'main'.'client' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'mail' TEXT NOT NULL, \
                                    'tarif'  TEXT NOT NULL, \
                                    't_obnal'  TEXT NOT NULL, \
                                    't_trans_in'  TEXT NOT NULL, \
                                    't_trans_in_s'  TEXT NOT NULL, \
                                    't_trans_out'  TEXT NOT NULL, \
                                    't_kred'  TEXT NOT NULL, \
                                    't_nalic'  TEXT NOT NULL, \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём клиента для %ов
        {
            query->exec("INSERT INTO    client"
                        "   (id, name, mail, tarif, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic) "
                        "VALUES ( 0, 'Наш счёт', '0', '0', '0', '0', '0', '0', '0', '0' )");
        }


        //Создаём таблицу групп контрагентов
        {
            query->exec("CREATE TABLE 'main'.'client_group' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём 0-ую группу
        {
            query->exec("INSERT INTO    client_group"
                        "   (id, name) "
                        "VALUES ( 0, 'Нет')");
        }

        //Создаём таблицу баланса клиентов
        {
            query->exec("CREATE TABLE 'main'.'client_balans' ( \
                                    'id' TEXT NOT NULL, \
                                    'balans' TEXT NOT NULL, \
                                    UNIQUE (id COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём баланс 0ого клиента
        {
            query->exec("INSERT INTO    client_balans"
                        "   (id, balans ) "
                        "VALUES ( 0, '0' )");
        }

        //Создаём таблицу движений по счёту клиентов
        {
            query->exec("CREATE TABLE 'main'.'client_operations' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'id_client' TEXT NOT NULL, \
                                    'to_client_id' TEXT NULL, \
                                    'id_pp' TEXT NULL, \
                                    'type' TEXT NOT NULL, \
                                    'text' TEXT NOT NULL, \
                                    'date' TEXT NOT NULL, \
                                    'summ' NUMERIC(18,2) NOT NULL, \
                                    'margin' TEXT NOT NULL) \
                    ");
        }

        //Создаём таблицу тарифов
        {
            query->exec("CREATE TABLE 'main'.'tarif' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'name' TEXT NOT NULL, \
                                    'def'  TEXT NOT NULL, \
                                    't_obnal'  TEXT NOT NULL, \
                                    't_trans_in'  TEXT NOT NULL, \
                                    't_trans_in_s'  TEXT NOT NULL, \
                                    't_trans_out'  TEXT NOT NULL, \
                                    't_kred'  TEXT NOT NULL, \
                                    't_nalic'  TEXT NOT NULL, \
                                    UNIQUE (name COLLATE NOCASE ASC)) \
                    ");
        }

        //Создаём 0ой тариф
        {
            query->exec("   INSERT INTO    tarif   ( \
                                            id, name, def, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic   \
                                                ) \
                        VALUES ( \
                                            '0' , 'Индивидуальный', '0', '0', '0', '0', '0', '0', '0' )" \
                    );
        }

        //Создаём таблицу платёжек
        {
            query->exec("CREATE TABLE 'main'.'pp' ( \
                                    'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                                    'rs_id' TEXT NOT NULL, \
                                    'client_id' TEXT NULL, \
                                    'type' TEXT NOT NULL, \
                                    'type_doc' TEXT NOT NULL, \
                                    'num' TEXT NOT NULL, \
                                    'date'  TEXT NOT NULL, \
                                    'date_oper' TEXT NOT NULL, \
                                    'sum'  NUMERIC(18,2) NOT NULL, \
                                    'ticket_date'  TEXT NULL, \
                                    'ticket_time'  TEXT NULL, \
                                    'ticket_value'  TEXT NULL, \
                                    'payer_count' TEXT NOT NULL, \
                                    'payer' TEXT NULL, \
                                    'payer_inn' TEXT NOT NULL, \
                                    'payer1' TEXT NULL, \
                                    'payer2' TEXT NULL, \
                                    'payer3' TEXT NULL, \
                                    'payer4' TEXT NULL, \
                                    'payer_rs' TEXT NULL, \
                                    'payer_bank1' TEXT NULL, \
                                    'payer_bank2' TEXT NULL, \
                                    'payer_bik' TEXT NULL, \
                                    'payer_ks' TEXT NULL, \
                                    'receiver_count' TEXT NOT NULL, \
                                    'receiver' TEXT NULL, \
                                    'receiver_inn' TEXT NOT NULL, \
                                    'receiver1' TEXT NULL, \
                                    'receiver2' TEXT NULL, \
                                    'receiver3' TEXT NULL, \
                                    'receiver4' TEXT NULL, \
                                    'receiver_rs' TEXT NULL, \
                                    'receiver_bank1' TEXT NULL, \
                                    'receiver_bank2' TEXT NULL, \
                                    'receiver_bik' TEXT NULL, \
                                    'receiver_ks' TEXT NULL, \
                                    'type_pay' TEXT NULL, \
                                    'type_trans' TEXT NULL, \
                                    'code' TEXT NULL, \
                                    'dest_pay' TEXT NULL, \
                                    'dest_pay1' TEXT NULL, \
                                    'dest_pay2' TEXT NULL, \
                                    'dest_pay3' TEXT NULL, \
                                    'dest_pay4' TEXT NULL, \
                                    'dest_pay5' TEXT NULL, \
                                    'dest_pay6' TEXT NULL, \
                                    'state_sender' TEXT NULL, \
                                    'payer_kpp' TEXT NULL, \
                                    'receiver_kpp' TEXT NULL, \
                                    'pokazatel_kbk' TEXT NULL, \
                                    'okato' TEXT NULL, \
                                    'pokazatel_osnovaniya' TEXT NULL, \
                                    'pokazatel_period' TEXT NULL, \
                                    'pokazatel_num' TEXT NULL, \
                                    'pokazatel_date' TEXT NULL, \
                                    'pokazatel_type' TEXT NULL, \
                                    'ocherednost' TEXT NULL, \
                                    'srok_accepta' TEXT NULL, \
                                    'type_akkred' TEXT NULL, \
                                    'srok_pay' TEXT NULL, \
                                    'usl_pay1' TEXT NULL, \
                                    'usl_pay2' TEXT NULL, \
                                    'usl_pay3' TEXT NULL, \
                                    'pay_po_predst' TEXT NULL, \
                                    'dop_usl' TEXT NULL, \
                                    'num_scheta_postav' TEXT NULL, \
                                    'date_send_doc' TEXT NULL, \
                                    UNIQUE (num, rs_id, type, date, dest_pay COLLATE NOCASE ASC)) \
                    ");
            query->exec("CREATE INDEX i_type_doc ON pp(type_doc)");
            query->exec("CREATE INDEX i_date_oper ON pp(date_oper)");
            query->exec("CREATE INDEX i_date ON pp(date)");
            query->exec("CREATE INDEX i_payer1 ON pp(payer1)");
            query->exec("CREATE INDEX i_receiver1 ON pp(receiver1)");
            query->exec("CREATE INDEX i_rs_id ON pp(rs_id)");
            query->exec("CREATE INDEX i_client_id ON pp(client_id)");
            query->exec("CREATE INDEX i_type ON pp(type)");
        }

        //Создаём таблицу соотношения платёжек клиенту
        {
            query->exec("CREATE TABLE 'main'.'pp_to_client' ( \
                                    'pp_id' TEXT NOT NULL, \
                                    'client_id'  TEXT NOT NULL, \
                                    UNIQUE (pp_id, client_id COLLATE NOCASE ASC)) \
                    ");
        }

        query->exec("COMMIT");
    }
}

void Widget::slot_login()
{
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT id, passwd FROM users WHERE name='" + ui->lineEdit_login->text() + "'");
    QString passwd = "";
    if (query->last())
    {
        query->first();
        passwd = query->value(1).toString();
    }

    if (func_gen_hash(ui->lineEdit_passwd->text()) == passwd)
    {
        user_id = new int;
        *user_id = query->value(0).toInt();
        gen_window = new general_window(0, user_id);
        hide();
        set_settings();
        gen_window->show();
    }
    else
    {
        QMessageBox::critical(this, "error", "User or password not found");
    }
    query->exec("COMMIT");
}

QString Widget::func_gen_hash(QString str)
{
    hash->reset();
    hash->addData(str.toUtf8());
    return hash->result().toHex();
}

Widget::~Widget()
{
    delete ui;
}
