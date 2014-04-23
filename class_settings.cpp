#include "class_settings.h"
#include "ui_class_settings.h"

class_settings::class_settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_settings)
{
    ui->setupUi(this);

    query = new QSqlQuery;

    slot_get_settings();

    connect(ui->pushButton_cancel, SIGNAL(clicked()), SLOT(slot_close()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), SLOT(slot_save_settings()));

    connect(ui->pushButton_select_path, SIGNAL(clicked()), SLOT(slot_select_dir()));
}

void class_settings::slot_get_settings()
{
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT value FROM settings WHERE name = 'path_to_pp_dir'");
    query->first();
    ui->lineEdit_pp_path->setText(query->value(0).toString());
    query->clear();
    query->exec("COMMIT");
}

void class_settings::slot_save_settings()
{
    slot_set_settings("path_to_pp_dir", ui->lineEdit_pp_path->text());
    slot_close();
}

void class_settings::slot_select_dir()
{
    ui->lineEdit_pp_path->setText(QFileDialog::getExistingDirectory(this, "Выбрать папку", ""));
}

//Записываем настройку в базу
void class_settings::slot_set_settings(QString name, QString value)
{
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT value FROM settings WHERE name = 'path_to_pp_dir'");
    query->first();

    if (query->value(0).toString() == "")
    {
        query->clear();
        query->prepare("INSERT INTO settings (value, name) VALUES (?, ?)");
    }
    else
    {
        query->clear();
        query->prepare("UPDATE settings SET value = ? WHERE name = ?");
    }
    query->addBindValue(value);
    query->addBindValue(name);
    query->exec();
    query->clear();
    query->exec("COMMIT");
}

void class_settings::slot_close()
{
    this->hide();
    delete this;
}

class_settings::~class_settings()
{
    delete ui;
}
