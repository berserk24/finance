#include "class_setings_table.h"
#include "ui_class_setings_table.h"

class_setings_table::class_setings_table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_setings_table)
{
    ui->setupUi(this);

    this->show();

    connect(ui->pushButton_cancel, SIGNAL(clicked()), SLOT(_close()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), SLOT(slot_set_collumn_state()));
}

void class_setings_table::slot_set_checkbox(QString str, bool state)
{
    chbox = new QCheckBox(ui->verticalLayout->widget());
    chbox->setText(str);
    chbox->setChecked(state);
    ui->verticalLayout->addWidget(chbox);
}

void class_setings_table::_close()
{
    this->hide();
    delete this;
}

void class_setings_table::slot_set_collumn_state()
{
    QCheckBox *cb;
    QList<bool> list;
    for (int i = 1; i < ui->verticalLayout->count(); i++)
    {
        cb = qobject_cast <QCheckBox *>(ui->verticalLayout->itemAt(i)->widget());
        list << cb->isChecked();
    }
    delete cb;
    emit signal_set_collumn_state(list);
    _close();
}

class_setings_table::~class_setings_table()
{
    delete ui;
}
