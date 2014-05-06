#include "class_ref_auto_actions.h"
#include "ui_class_ref_auto_actions.h"

class_ref_auto_actions::class_ref_auto_actions(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_ref_auto_actions)
{
    ui->setupUi(this);

    db = db1;

    QValidator *validator = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9].[0-9][0-9]|[0-9][0-9].[0-9][0-9]|[0-9][0-9].[0-9]"), this);
    ui->lineEdit_margin->setValidator(validator);
}

class_ref_auto_actions::~class_ref_auto_actions()
{
    delete ui;
}
