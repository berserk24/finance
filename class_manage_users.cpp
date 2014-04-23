#include "class_manage_users.h"
#include "ui_class_manage_users.h"

class_manage_users::class_manage_users(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_manage_users)
{
    ui->setupUi(this);

    model = new QSqlQueryModel;
    query = new QSqlQuery;
    hash = new QCryptographicHash(QCryptographicHash::Md5);

    slot_select_table();

    model->setHeaderData(1,Qt::Horizontal, "Имя пользователя");
    model->setHeaderData(2,Qt::Horizontal, "Справочники");
    model->setHeaderData(3,Qt::Horizontal, "Загрузка\nплатёжек");
    model->setHeaderData(4,Qt::Horizontal, "Обработка\nплатёжек");
    model->setHeaderData(5,Qt::Horizontal, "Баланс\nконтрагентов");
    model->setHeaderData(6,Qt::Horizontal, "Отчёты");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 150);
    ui->tableView->setColumnWidth(2, 130);
    ui->tableView->setColumnWidth(3, 130);
    ui->tableView->setColumnWidth(4, 130);
    ui->tableView->setColumnWidth(5, 130);
    ui->tableView->setColumnWidth(6, 130);

    //Включаем/выключаем кнопку добавить, удалить
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_set_field()));
    connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));

    //Добавляем или изменяем пользователя
    connect(ui->pushButton_add, SIGNAL(clicked()), SLOT(slot_add_user()));

    //Удаляем пользователя
    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_del_user()));
}

//Заполняем таблицу
void class_manage_users::slot_select_table()
{
    model->setQuery("SELECT users.id, users.name, yn1.name, yn2.name, yn3.name, yn4.name, yn5.name FROM users "
                    "LEFT JOIN users_access uac ON uac.id = users.id "
                    "LEFT JOIN yes_no yn1 ON yn1.id = uac.ref "
                    "LEFT JOIN yes_no yn2 ON yn2.id = uac.load_pp "
                    "LEFT JOIN yes_no yn3 ON yn3.id = uac.work_pp "
                    "LEFT JOIN yes_no yn4 ON yn4.id = uac.client "
                    "LEFT JOIN yes_no yn5 ON yn5.id = uac.report");
    ui->tableView->setModel(model);
    ui->tableView->show();
}

//Включаем/выключаем кнопку добавить
void class_manage_users::slot_enable_add()
{
    if (ui->lineEdit_name->text() != "")
    {
        ui->pushButton_add->setEnabled(true);
    }
    else
    {
        ui->pushButton_add->setEnabled(false);
    }
}

//Заполняем поля
void class_manage_users::slot_set_field()
{
    if (ui->tableView->selectionModel()->hasSelection() and ui->tableView->selectionModel()->selectedRows().size() == 1)
    {
        ui->pushButton_del->setEnabled(true);
        ui->pushButton_add->setText("Изменить");
        ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString());
        ui->checkBox_ref->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString()));
        ui->checkBox_load_pp->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString()));
        ui->checkBox_work_pp->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(4).data().toString()));
        ui->checkBox_client->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(5).data().toString()));
        ui->checkBox_report->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString()));
    }
    else
    {
        ui->pushButton_del->setEnabled(false);
        ui->pushButton_add->setText("Добавить");
        slot_clear_field();
    }
}

//Добавляем/изменяем пользователя
void class_manage_users::slot_add_user()
{
    int user_id;
    if (ui->pushButton_add->text() == "Добавить")
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        query->prepare("INSERT INTO users (name, passwd) VALUES (?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(func_gen_hash(ui->lineEdit_passwd->text()));
        query->exec();
        query->clear();

        query->exec("SELECT seq FROM sqlite_sequence WHERE name = 'users'");
        query->first();
        user_id = query->value(0).toInt();

        query->prepare("INSERT INTO users_access (id, ref, load_pp, work_pp, client, report) "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query->addBindValue(user_id);
        if (ui->checkBox_ref->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_load_pp->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_work_pp->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_client->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_report->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        query->exec();
        query->clear();
        query->exec("COMMIT");
    }
    else
    {
        if (ui->lineEdit_passwd->text() != "")
        {
            query->exec("BEGIN IMMEDIATE TRANSACTION");
            query->prepare("UPDATE users SET name = ?, passwd = ? WHERE id = ?");
            query->addBindValue(ui->lineEdit_name->text());
            query->addBindValue(func_gen_hash(ui->lineEdit_passwd->text()));
            query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
            query->exec();
            query->clear();
            query->exec("COMMIT");
        }
        else
        {
            query->exec("BEGIN IMMEDIATE TRANSACTION");
            query->prepare("UPDATE users SET name = ? WHERE id = ?");
            query->addBindValue(ui->lineEdit_name->text());
            query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
            query->exec();
            query->clear();
            query->exec("COMMIT");
        }
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        query->prepare("UPDATE users_access SET ref = ?, load_pp = ?, work_pp = ?, client = ?, report = ? WHERE id = ?");
        if (ui->checkBox_ref->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_load_pp->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_work_pp->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_client->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        if (ui->checkBox_report->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
        query->exec();
        query->clear();
        query->exec("COMMIT");
    }
    slot_clear_field();
    slot_select_table();
}

//Удаляем пользователя
void class_manage_users::slot_del_user()
{
    if (ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString() != "admin")
    {
        int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Вы уверены что хотите удалить пользователя?"),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
        switch (ret)
        {
            case QMessageBox::Yes:
                query->exec("BEGIN IMMEDIATE TRANSACTION");
                query->prepare("DELETE FROM users_access WHERE id = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
                query->exec();
                query->clear();
                query->prepare("DELETE FROM users WHERE id = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
                query->exec();
                query->clear();
                query->exec("COMMIT");
                break;
            case QMessageBox::Cancel:
                return;
                break;
        }
    }
    slot_clear_field();
    slot_select_table();
}

void class_manage_users::slot_clear_field()
{
    ui->lineEdit_name->setText("");
    ui->lineEdit_passwd->setText("");
    ui->checkBox_ref->setChecked(false);
    ui->checkBox_load_pp->setChecked(false);
    ui->checkBox_work_pp->setChecked(false);
    ui->checkBox_client->setChecked(false);
    ui->checkBox_report->setChecked(false);
}

QString class_manage_users::func_gen_hash(QString str)
{
    hash->reset();
    hash->addData(str.toUtf8());
    return hash->result().toHex();
}

bool class_manage_users::func_str_to_bool(QString str)
{
    if (str == "Да")
    {
        return true;
    }
    if (str == "Нет")
    {
        return false;
    }
}

class_manage_users::~class_manage_users()
{
    delete ui;
}
