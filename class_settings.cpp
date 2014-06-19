#include "class_settings.h"
#include "ui_class_settings.h"

class_settings::class_settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_settings)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::UserScope, "finance", "finance", this);
    query = new QSqlQuery;

    slot_get_settings();

    slot_load_rs();

    connect(ui->pushButton_cancel, SIGNAL(clicked()), SLOT(slot_close()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), SLOT(slot_set_settings()));

    connect(ui->pushButton_select_path, SIGNAL(clicked()), SLOT(slot_select_dir()));
}

//
void class_settings::slot_add_widget(int id, QString name)
{
    hb_layout = new QHBoxLayout;
    label = new QLabel;
    label->setText(name);
    hb_layout->addWidget(label);
    lineEdit = new QLineEdit;
    hb_layout->addWidget(lineEdit);
    button = new QPushButton;
    button->setText("...");
    button->setMaximumSize(30,30);
    hb_layout->addWidget(button);
    ui->verticalLayout->addLayout(hb_layout);
}

//Загружаем расчтные счета
void class_settings::slot_load_rs()
{
    query->exec("SELECT id, name FROM rss");
    query->first();
    slot_add_widget(query->value(0).toInt(), query->value(1).toString());
    while(query->next())
    {
        slot_add_widget(query->value(0).toInt(), query->value(1).toString());
    }
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
