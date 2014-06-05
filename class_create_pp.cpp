#include "class_create_pp.h"
#include "ui_class_create_pp.h"

class_create_pp::class_create_pp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_create_pp)
{
    ui->setupUi(this);

    query = new QSqlQuery;

    slot_set_regexp();
    slot_load_rss();
    slot_get_count_pp();
    slot_load_firm();
    slot_set_date();
    slot_load_bik();
    slot_set_dest_pay();
    slot_select_receiver_bank_city(ui->comboBox_receiver_bik->itemData(ui->comboBox_receiver_bik->currentIndex()).toInt());

    //Раскрываем/скрываем разделы
    connect(ui->groupBox_gen_info, SIGNAL(clicked(bool)), SLOT(slot_show_info(bool)));
    connect(ui->groupBox_payer, SIGNAL(clicked(bool)), SLOT(slot_show_payer(bool)));
    connect(ui->groupBox_payer_recv, SIGNAL(clicked(bool)), SLOT(slot_show_payer_recv(bool)));
    connect(ui->groupBox_receiver, SIGNAL(clicked(bool)), SLOT(slot_show_receiver(bool)));
    connect(ui->groupBox_dest_pay, SIGNAL(clicked(bool)), SLOT(slot_show_dest_pay(bool)));
    connect(ui->groupBox_budget, SIGNAL(clicked(bool)), SLOT(slot_show_budget(bool)));
    connect(ui->groupBox_auto_receiver, SIGNAL(clicked(bool)), SLOT(slot_show_auto_receiver(bool)));


    //Получаем значение счтчика платёжек
    connect(ui->comboBox_payer_rs, SIGNAL(currentIndexChanged(int)), SLOT(slot_get_count_pp()));

    //Заполняем реквизиты плательщика
    connect(ui->comboBox_payer_rs, SIGNAL(currentIndexChanged(int)), SLOT(slot_load_firm()));

    connect(ui->comboBox_receiver_bik, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_receiver_bank()));
    connect(ui->comboBox_receiver_bank, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_receiver_bank()));

    //заполняем автоматом реквизиты получателя
    connect(ui->pushButton_load_receiver, SIGNAL(clicked()), SLOT(slot_load_auto_rekv()));

    //Включаем/выключаем кнопку создать платёжку
    {
        connect(ui->lineEdit_sum, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->comboBox_ocherednost, SIGNAL(editTextChanged(QString)), SLOT(slot_set_enable_add()));
        connect(ui->comboBox_type_pp, SIGNAL(editTextChanged(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer_bank, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer_bank2, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer_inn, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer_bik, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_payer_ks, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_receiver, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_receiver_bank2, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_receiver_inn, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_receiver_rs, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_dest_pay, SIGNAL(textEdited(QString)), SLOT(slot_set_enable_add()));
        connect(ui->comboBox_receiver_bank, SIGNAL(editTextChanged(QString)), SLOT(slot_set_enable_add()));
        connect(ui->comboBox_receiver_bik, SIGNAL(editTextChanged(QString)), SLOT(slot_set_enable_add()));
        connect(ui->pushButton_load_receiver, SIGNAL(clicked()), SLOT(slot_set_enable_add()));
        connect(ui->lineEdit_pp_num, SIGNAL(textChanged(QString)), SLOT(slot_set_enable_add()));
    }

    //Создаём платёжку
    connect(ui->pushButton_create_pp, SIGNAL(clicked()), SLOT(slot_create_pp()));

    //Заполняем назначение платежа
    connect(ui->comboBox_nds, SIGNAL(currentIndexChanged(int)), SLOT(slot_set_dest_pay()));
    connect(ui->lineEdit_nds, SIGNAL(textChanged(QString)), SLOT(slot_set_dest_pay()));
    connect(ui->lineEdit_sum, SIGNAL(textChanged(QString)), SLOT(slot_set_dest_pay()));
}
//Заполняем назначение платежа
void class_create_pp::slot_set_dest_pay()
{
    if (ui->comboBox_nds->currentIndex() == 0)
    {
        ui->lineEdit_dest_pay->setText(ui->lineEdit_dest_pay->text().mid(0,ui->lineEdit_dest_pay->text().indexOf(",\r")) + ",\r в т. ч. НДС (" + ui->lineEdit_nds->text() + "%) - " + QString::number(ui->lineEdit_sum->text().toFloat()*ui->lineEdit_nds->text().toInt()/(100+ui->lineEdit_nds->text().toInt()), 'f', 2));
        if (sender() == ui->comboBox_nds)
            ui->lineEdit_nds->setText("18");
    }
    if (ui->comboBox_nds->currentIndex() == 1)
    {
        ui->lineEdit_dest_pay->setText(ui->lineEdit_dest_pay->text().mid(0,ui->lineEdit_dest_pay->text().indexOf(",\r")) + ",\r НДС не облагается");
        ui->lineEdit_nds->setText("");
    }
    if (ui->comboBox_nds->currentIndex() == 2)
    {
        ui->lineEdit_dest_pay->setText(ui->lineEdit_dest_pay->text().mid(0,ui->lineEdit_dest_pay->text().indexOf(",\r")));
        ui->lineEdit_nds->setText("");
    }
}

//Создаём платёжку
void class_create_pp::slot_create_pp()
{
    {
        QString count, str_null;
        str_null = QString::null;
        query->prepare("SELECT number FROM rss WHERE id = ?");
        query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toString());
        query->exec();
        query->first();
        count = query->value(0).toString();

        query->prepare("INSERT INTO pp (rs_id, type_pp, type_doc, num, date_pp, date_oper, sum_pp, payer_count,"
                                        "payer, payer_inn, payer1, payer2, payer3, payer4, payer_rs, payer_bank1,"
                                        "payer_bank2, payer_bik, payer_ks, receiver_count, receiver, receiver_inn, receiver1, receiver2, receiver3,"
                                        "receiver4, receiver_rs, receiver_bank1, receiver_bank2, receiver_bik, receiver_ks, type_pay,"
                                        "type_trans, code, dest_pay, dest_pay1, dest_pay2, dest_pay3, dest_pay4, dest_pay5, dest_pay6,"
                                        "state_sender, payer_kpp, receiver_kpp, pokazatel_kbk, okato, pokazatel_osnovaniya, pokazatel_period,"
                                        "pokazatel_num, pokazatel_date, pokazatel_type, ocherednost, type_akkred, srok_pay,"
                                        "usl_pay1, usl_pay2, usl_pay3, pay_po_predst, dop_usl, num_scheta_postav, date_send_doc, status_pp) "
                       "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toString());
        query->addBindValue("1");
        query->addBindValue("0");
        query->addBindValue(ui->lineEdit_pp_num->text());
        query->addBindValue(ui->dateEdit_date->date());
        query->addBindValue(ui->dateEdit_date->date());
        query->addBindValue(ui->lineEdit_sum->text().replace(",", "."));
        query->addBindValue(count);
        query->addBindValue(ui->lineEdit_payer->text());
        query->addBindValue(ui->lineEdit_payer_inn->text());
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(count);
        query->addBindValue(ui->lineEdit_payer_bank->text());
        query->addBindValue(ui->lineEdit_payer_bank2->text());
        query->addBindValue(ui->lineEdit_payer_bik->text());
        query->addBindValue(ui->lineEdit_payer_ks->text());
        query->addBindValue(ui->lineEdit_receiver_rs->text());
        query->addBindValue(ui->lineEdit_receiver->text());
        query->addBindValue(ui->lineEdit_receiver_inn->text());
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(ui->lineEdit_receiver_rs->text());
        query->addBindValue(ui->comboBox_receiver_bank->currentText());
        query->addBindValue(ui->lineEdit_receiver_bank2->text());
        query->addBindValue(ui->comboBox_receiver_bik->currentText());
        query->addBindValue(ui->lineEdit_receiver_ks->text());
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(ui->lineEdit_dest_pay->text().replace("\r", ""));
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(ui->lineEdit_payer_kpp->text());
        query->addBindValue(ui->lineEdit_receiver_kpp->text());
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(QDate::fromString("01.01.1900", "dd.MM.yyyy"));
        query->addBindValue(str_null);
        query->addBindValue(ui->comboBox_ocherednost->currentText());
        query->addBindValue(str_null);
        query->addBindValue(ui->dateEdit_date->date());
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(str_null);
        query->addBindValue(ui->dateEdit_date->date());
        query->addBindValue(0);
        if (query->exec())
        {
            query->clear();
            query->prepare("UPDATE count_pp SET count_pp=?+1 WHERE id = ?");
            query->addBindValue(ui->lineEdit_pp_num->text().toInt());
            query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toString());
            query->exec();
            query->clear();
            query->prepare("UPDATE rss_balans SET balans = (balans - ?) WHERE id = ?");
            query->addBindValue(ui->lineEdit_sum->text());
            query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toString());
            query->exec();
            query->clear();
        }
    }
    slot_clear_form();

}

//Включаем/выключаем кнопку создать платёжку
void class_create_pp::slot_set_enable_add()
{
    if (ui->lineEdit_sum->text().replace(" ", "") != "" and ui->comboBox_ocherednost->currentText().replace(" ", "") != ""
        and ui->comboBox_type_pp->currentText().replace(" ", "") != ""
        and ui->lineEdit_payer->text().replace(" ", "") != "" and ui->lineEdit_payer_bank->text().replace(" ", "") != ""
        and ui->lineEdit_payer_bank2->text().replace(" ", "") != "" and (ui->lineEdit_payer_inn->text().length() == 10
                                                        or ui->lineEdit_payer_inn->text().length() == 12)
        and ui->lineEdit_payer_bik->text().length() == 9 and ui->lineEdit_payer_ks->text().length() == 20
        and ui->lineEdit_receiver->text().replace(" ", "") != "" and ui->lineEdit_receiver_bank2->text().replace(" ", "") != ""
        and ui->comboBox_receiver_bank->currentText().replace(" ", "") != "" and ui->comboBox_receiver_bik->currentText().length() == 9
        and (ui->lineEdit_receiver_inn->text().length() == 10 or ui->lineEdit_receiver_inn->text().length() == 12)
        and ui->lineEdit_receiver_rs->text().length() == 20 and ui->lineEdit_receiver_rs->text().length() == 20
        and ui->lineEdit_dest_pay->text().replace(" ", "") != ""
        and ui->lineEdit_balans_rs->text().toDouble() > ui->lineEdit_sum->text().toDouble() and count_pp <= ui->lineEdit_pp_num->text().toInt())
    {
        ui->pushButton_create_pp->setEnabled(true);
    }
    else
    {
        ui->pushButton_create_pp->setEnabled(false);
    }
}

//Устанавливаем регулярные выражения
void class_create_pp::slot_set_regexp()
{
    QValidator *validator = new QRegExpValidator(QRegExp("[1-9]\\d{11}"), this);
    ui->lineEdit_pp_num->setValidator(validator);
    ui->comboBox_ocherednost->setValidator(validator);
    ui->lineEdit_payer_inn->setValidator(validator);
    ui->lineEdit_receiver_inn->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("d{9}"), this);
    ui->lineEdit_payer_bik->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("[1-9]\\d{8}"), this);
    ui->lineEdit_payer_kpp->setValidator(validator);
    ui->lineEdit_receiver_kpp->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("[1-9]\\d{19}"), this);
    ui->lineEdit_payer_ks->setValidator(validator);
    ui->lineEdit_receiver_ks->setValidator(validator);
    ui->lineEdit_receiver_rs->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("^[1-9]\\d{0,8}|^[1-9]\\d{0,8}.[0-9][0-9]|^0.[0-9][0-9]"), this);
    ui->lineEdit_sum->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("^[1-9]\\d{1}"), this);
    ui->lineEdit_nds->setValidator(validator);
}

//Автозаполнение реквизитов получателя
void class_create_pp::slot_load_auto_rekv()
{
    QString str, str1, str2 = "", str3 = "";
    str1 = "";
    str = ui->plainTextEdit_receiver_auto->toPlainText();
    while (str.length() > 1)
    {
        str1 = str.mid(0, str.indexOf("\n"));
        if (str.indexOf("\n") != -1)
        {
            str.remove(0, str.indexOf("\n") + 1);
        }
        else
        {
            str.remove(0, str.length());
        }
        if ((str1.length() == 10 or str1.length() == 12) and str1.mid(0,4).toInt() > 99)
        {
            ui->lineEdit_receiver_inn->setText(str1);
            continue;
        }
        if (str1.length() == 9 and str1.mid(0, 2) == "04")
        {
            ui->comboBox_receiver_bik->setCurrentIndex(ui->comboBox_receiver_bik->findText(str1));
            continue;
        }
        else if (str1.length() == 9 and str1.mid(0, 3).toInt() > 99)
        {
            ui->lineEdit_receiver_kpp->setText(str1);
            continue;
        }
        if (str1.length() == 20 and str1.mid(0,3).toInt() > 99)
        {
            ui->lineEdit_receiver_rs->setText(str1);
            continue;
        }
        if (str2 == "")
        {
            str2 = str1;
            continue;
        }
        if (str3 == "")
        {
            str3 = str1;
            continue;
        }
    }
    if (str2 > str3)
    {
        ui->lineEdit_receiver->setText(str3);
        ui->lineEdit_dest_pay->setText(str2);
        slot_set_dest_pay();
    }
    else
    {
        ui->lineEdit_receiver->setText(str2);
        ui->lineEdit_dest_pay->setText(str3);
        slot_set_dest_pay();
    }
}


//Раскрываем/скрываем разделы
void class_create_pp::slot_show_auto_receiver(bool state)
{
    if (state)
    {
        ui->groupBox_auto_receiver->setMinimumHeight(150);
        ui->groupBox_auto_receiver->setMaximumHeight(150);
    }
    else
    {
        ui->groupBox_auto_receiver->setMinimumHeight(20);
        ui->groupBox_auto_receiver->setMaximumHeight(20);
    }
}

void class_create_pp::slot_show_info(bool state)
{
    if (state)
    {
        ui->groupBox_gen_info->setMinimumHeight(120);
        ui->groupBox_gen_info->setMaximumHeight(120);
    }
    else
    {
        ui->groupBox_gen_info->setMinimumHeight(20);
        ui->groupBox_gen_info->setMaximumHeight(20);
    }
}

void class_create_pp::slot_show_payer(bool state)
{
    if (state)
    {
        ui->groupBox_payer->setMinimumHeight(100);
        ui->groupBox_payer->setMaximumHeight(250);
    }
    else
    {
        ui->groupBox_payer->setMinimumHeight(20);
        ui->groupBox_payer->setMaximumHeight(20);
        ui->groupBox_payer_recv->setMinimumHeight(20);
        ui->groupBox_payer_recv->setMaximumHeight(20);
        ui->groupBox_payer_recv->setChecked(false);
    }
}

void class_create_pp::slot_show_payer_recv(bool state)
{
    if (state)
    {
        ui->groupBox_payer->setMinimumHeight(250);
        ui->groupBox_payer_recv->setMinimumHeight(150);
        ui->groupBox_payer_recv->setMaximumHeight(150);
    }
    else
    {
        ui->groupBox_payer->setMinimumHeight(100);
        ui->groupBox_payer_recv->setMinimumHeight(20);
        ui->groupBox_payer_recv->setMaximumHeight(20);
    }
}

void class_create_pp::slot_show_receiver(bool state)
{
    if (state)
    {
        ui->groupBox_receiver->setMinimumHeight(200);
        ui->groupBox_receiver->setMaximumHeight(200);
    }
    else
    {
        ui->groupBox_receiver->setMinimumHeight(20);
        ui->groupBox_receiver->setMaximumHeight(20);
    }
}

void class_create_pp::slot_show_dest_pay(bool state)
{
    if (state)
    {
        ui->groupBox_dest_pay->setMinimumHeight(100);
        ui->groupBox_dest_pay->setMaximumHeight(100);
    }
    else
    {
        ui->groupBox_dest_pay->setMinimumHeight(20);
        ui->groupBox_dest_pay->setMaximumHeight(20);
    }
}

void class_create_pp::slot_show_budget(bool state)
{
    if (state)
    {
        ui->groupBox_budget->setMinimumHeight(200);
        ui->groupBox_budget->setMaximumHeight(200);
    }
    else
    {
        ui->groupBox_budget->setMinimumHeight(20);
        ui->groupBox_budget->setMaximumHeight(20);
    }
}

//Очищаем форму
void class_create_pp::slot_clear_form()
{
    ui->lineEdit_sum->setText("");
    ui->comboBox_ocherednost->setCurrentIndex(0);
    ui->comboBox_type_pp->setCurrentIndex(0);
    ui->lineEdit_receiver->setText("");
    ui->lineEdit_receiver_inn->setText("");
    ui->lineEdit_receiver_rs->setText("");
    ui->lineEdit_receiver_kpp->setText("");
    ui->comboBox_receiver_bik->setCurrentIndex(0);
    ui->lineEdit_dest_pay->setText("");
    ui->comboBox_nds->setCurrentIndex(0);
    slot_get_count_pp();
}

//Получаем значение счётчика платёжек
void class_create_pp::slot_get_count_pp()
{
    QString count;
    int id = ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toInt();
    query->prepare("SELECT * FROM count_pp WHERE id = ?");
    query->addBindValue(id);
    query->exec();
    query->first();
    count = query->value(2).toString();
    if (query->value(1).toDate() < QDate::currentDate())
    {
        query->clear();
        query->prepare("UPDATE count_pp SET date_count = ?, count_pp = 1 WHERE id = ?");
        query->addBindValue(QDate(QDate::currentDate().year()+1, 1, 1));
        query->addBindValue(id);
        query->exec();
        count = "1";
    }
    query->clear();
    ui->lineEdit_pp_num->setText(count);
    count_pp = count.toInt();
}

//Заполняем расчётные счета
void class_create_pp::slot_load_rss()
{
    QSqlQuery *query_rs = new QSqlQuery;
    int n = 0;
    if (ui->comboBox_payer_rs->count() > 0 or ui->comboBox_payer_rs->currentIndex() > 0)
    {
        n = ui->comboBox_payer_rs->currentIndex();
    }
    ui->comboBox_payer_rs->clear();
    query_rs->exec("SELECT id, name FROM rss ORDER BY id");
    query_rs->first();

    ui->comboBox_payer_rs->addItem(query_rs->value(1).toString(), QVariant(query_rs->value(0).toInt()));
    while(query_rs->next())
    {
        ui->comboBox_payer_rs->addItem(query_rs->value(1).toString(), QVariant(query_rs->value(0).toInt()));
    }
    query_rs->clear();
    ui->comboBox_payer_rs->setCurrentIndex(n);
    delete query_rs;
}

//Устанавливаем интервал даты
void class_create_pp::slot_set_date()
{
    ui->dateEdit_date->setDateRange(QDate::currentDate(), QDate::currentDate().addYears(1));
    ui->dateEdit_date->setDate(QDate::currentDate());
}

//Заполняем реквизиты плательщика
void class_create_pp::slot_load_firm()
{
    QString bik = "";
    query->prepare("SELECT firms.*, rss.bik FROM firms "
                    "LEFT JOIN rss ON rss.firm = firms.id "
                    "WHERE rss.id = ?");
    query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toInt());
    query->exec();
    query->first();
    ui->lineEdit_payer->setText(query->value(1).toString());
    ui->lineEdit_payer_inn->setText(query->value(2).toString());
    ui->lineEdit_payer_kpp->setText(query->value(3).toString());
    bik = query->value(5).toString();
    query->clear();

    query->prepare("SELECT * FROM ref_banks WHERE bik = ?");
    query->addBindValue(bik);
    query->exec();
    query->first();
    ui->lineEdit_payer_bank->setText(query->value(4).toString());
    ui->lineEdit_payer_bik->setText(bik);
    ui->lineEdit_payer_bank2->setText(query->value(2).toString());
    ui->lineEdit_payer_ks->setText(query->value(8).toString());
    query->clear();
    query->prepare("SELECT CAST(balans.balans + rss.start_balans AS VARCHAR(20)) "
                   "FROM rss_balans balans "
                   "LEFT JOIN rss ON balans.id = rss.id "
                   "WHERE rss.id = ?");
    query->addBindValue(ui->comboBox_payer_rs->itemData(ui->comboBox_payer_rs->currentIndex()).toInt());
    query->exec();
    query->first();
    ui->lineEdit_balans_rs->setText(query->value(0).toString());
    query->clear();
}

//Заполняем форму банка получателя
void class_create_pp::slot_load_bik()
{
    query->exec("SELECT id, bik, name FROM ref_banks ORDER BY name");
    query->first();
    ui->comboBox_receiver_bik->addItem(query->value(1).toString(), query->value(0).toInt());
    ui->comboBox_receiver_bank->addItem(query->value(2).toString(), query->value(0).toInt());
    while (query->next())
    {
        ui->comboBox_receiver_bik->addItem(query->value(1).toString(), query->value(0).toInt());
        ui->comboBox_receiver_bank->addItem(query->value(2).toString(), query->value(0).toInt());
    }
    query->clear();
}

//
void class_create_pp::slot_select_receiver_bank()
{
    int id;
    if (QObject::sender() == ui->comboBox_receiver_bik)
    {
        ui->comboBox_receiver_bank->blockSignals(true);
        ui->comboBox_receiver_bank->setCurrentIndex(ui->comboBox_receiver_bik->currentIndex());
        ui->comboBox_receiver_bank->blockSignals(false);
        id = ui->comboBox_receiver_bik->itemData(ui->comboBox_receiver_bik->currentIndex()).toInt();
    }
    if (QObject::sender() == ui->comboBox_receiver_bank)
    {
        ui->comboBox_receiver_bik->blockSignals(true);
        ui->comboBox_receiver_bik->setCurrentIndex(ui->comboBox_receiver_bank->currentIndex());
        ui->comboBox_receiver_bik->blockSignals(false);
        id = ui->comboBox_receiver_bank->itemData(ui->comboBox_receiver_bank->currentIndex()).toInt();
    }
    slot_select_receiver_bank_city(id);
}

//
void class_create_pp::slot_select_receiver_bank_city(int id)
{
    query->prepare("SELECT sity, ks FROM ref_banks WHERE id = ?");
    query->addBindValue(id);
    query->exec();
    query->first();
    ui->lineEdit_receiver_bank2->setText(query->value(0).toString());
    ui->lineEdit_receiver_ks->setText(query->value(1).toString());
    query->clear();
}
