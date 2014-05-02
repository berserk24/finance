#include "class_settings.h"
#include "ui_class_settings.h"

class_settings::class_settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_settings)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::UserScope, "finance", "finance", this);

    slot_get_settings();

    connect(ui->pushButton_cancel, SIGNAL(clicked()), SLOT(slot_close()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), SLOT(slot_set_settings()));

    connect(ui->pushButton_select_path, SIGNAL(clicked()), SLOT(slot_select_dir()));
}

void class_settings::slot_get_settings()
{
    settings->beginGroup("Load_pp");
    ui->lineEdit_pp_path->setText(settings->value("path", "").toString());
    settings->endGroup();
}

void class_settings::slot_select_dir()
{
    ui->lineEdit_pp_path->setText(QFileDialog::getExistingDirectory(this, "Выбрать папку", ""));
}

//Записываем настройку в базу
void class_settings::slot_set_settings()
{
    settings->beginGroup("Load_pp");
    settings->setValue("path", ui->lineEdit_pp_path->text().toUtf8());
    settings->endGroup();
    settings->sync();
    slot_close();
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
