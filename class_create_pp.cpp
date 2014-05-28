#include "class_create_pp.h"
#include "ui_class_create_pp.h"

class_create_pp::class_create_pp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_create_pp)
{
    ui->setupUi(this);

    query = new QSqlQuery;

    slot_load_rss();
    slot_get_count_pp();
    slot_set_date();

    //Раскрываем/скрываем разделы
    connect(ui->groupBox_gen_info, SIGNAL(clicked(bool)), SLOT(slot_show_info(bool)));
    connect(ui->groupBox_payer, SIGNAL(clicked(bool)), SLOT(slot_show_payer(bool)));
    connect(ui->groupBox_receiver, SIGNAL(clicked(bool)), SLOT(slot_show_receiver(bool)));
    connect(ui->groupBox_dest_pay, SIGNAL(clicked(bool)), SLOT(slot_show_dest_pay(bool)));
    connect(ui->groupBox_budget, SIGNAL(clicked(bool)), SLOT(slot_show_budget(bool)));
    connect(ui->groupBox_auto_receiver, SIGNAL(clicked(bool)), SLOT(slot_show_auto_receiver(bool)));


    //Получаем значение счтчика платёжек
    connect(ui->comboBox_payer_rs, SIGNAL(currentIndexChanged(int)), SLOT(slot_get_count_pp()));
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
        ui->groupBox_payer->setMinimumHeight(200);
        ui->groupBox_payer->setMaximumHeight(200);
    }
    else
    {
        ui->groupBox_payer->setMinimumHeight(20);
        ui->groupBox_payer->setMaximumHeight(20);
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
        ui->groupBox_dest_pay->setMinimumHeight(200);
        ui->groupBox_dest_pay->setMaximumHeight(200);
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

}

//Получаем значение счтчика платёжек
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
        query->addBindValue(QDate::currentDate());
        query->addBindValue(id);
        query->exec();
        count = 1;
    }
    query->clear();
    ui->lineEdit_pp_num->setText(count);
}

//Заполняем расчётные счета
void class_create_pp::slot_load_rss()
{
    query->exec("SELECT id, name FROM rss ORDER BY id");
    query->first();
    ui->comboBox_payer_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    while(query->next())
    {
        ui->comboBox_payer_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    }
    query->clear();
}

void class_create_pp::slot_set_date()
{
    ui->dateEdit_date->setDateRange(QDate::currentDate(), QDate::currentDate().addYears(1));
    ui->dateEdit_date->setDate(QDate::currentDate());
}
