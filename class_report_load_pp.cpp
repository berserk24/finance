#include "class_report_load_pp.h"
#include "ui_class_report_load_pp.h"

class_report_load_pp::class_report_load_pp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_report_load_pp)
{
    ui->setupUi(this);
}

void class_report_load_pp::slot_set_text(QString str)
{
    ui->textBrowser->setHtml(str);
}

class_report_load_pp::~class_report_load_pp()
{
    delete ui;
}
