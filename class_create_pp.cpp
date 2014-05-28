#include "class_create_pp.h"
#include "ui_class_create_pp.h"

class_create_pp::class_create_pp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_create_pp)
{
    ui->setupUi(this);
    connect(ui->groupBox_gen_info, SIGNAL(clicked(bool)), SLOT(slot_show_info(bool)));
    connect(ui->groupBox_payer, SIGNAL(clicked(bool)), SLOT(slot_show_payer(bool)));
    connect(ui->groupBox_receiver, SIGNAL(clicked(bool)), SLOT(slot_show_receiver(bool)));
    connect(ui->groupBox_dest_pay, SIGNAL(clicked(bool)), SLOT(slot_show_dest_pay(bool)));
    connect(ui->groupBox_budget, SIGNAL(clicked(bool)), SLOT(slot_show_budget(bool)));
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
