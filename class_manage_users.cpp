#include "class_manage_users.h"
#include "ui_class_manage_users.h"

class_manage_users::class_manage_users(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_manage_users)
{
    ui->setupUi(this);

    model = new QSqlQueryModel;
    query = new QSqlQuery;

    db = db1;

    slot_select_table();

    model->setHeaderData(0,Qt::Horizontal, "Имя пользователя");
    model->setHeaderData(1,Qt::Horizontal, "Справочники");
    model->setHeaderData(2,Qt::Horizontal, "Загрузка\nплатёжек");
    model->setHeaderData(3,Qt::Horizontal, "Обработка\nплатёжек");
    model->setHeaderData(4,Qt::Horizontal, "Баланс\nконтрагентов");
    model->setHeaderData(5,Qt::Horizontal, "Отчёты");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->setColumnWidth(1, 130);
    ui->tableView->setColumnWidth(2, 130);
    ui->tableView->setColumnWidth(3, 130);
    ui->tableView->setColumnWidth(4, 130);
    ui->tableView->setColumnWidth(5, 130);

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
    model->setQuery("SELECT users_access.id, yn1.data, yn2.data, yn3.data, yn4.data, yn5.data FROM users_access "
                    "LEFT JOIN yes_no yn1 ON yn1.id = users_access.ref "
                    "LEFT JOIN yes_no yn2 ON yn2.id = users_access.load_pp "
                    "LEFT JOIN yes_no yn3 ON yn3.id = users_access.work_pp "
                    "LEFT JOIN yes_no yn4 ON yn4.id = users_access.client "
                    "LEFT JOIN yes_no yn5 ON yn5.id = users_access.report");
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
        if (ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString() != "SYSDBA")
        {
            ui->pushButton_del->setEnabled(true);
            ui->checkBox_ref->setEnabled(true);
            ui->checkBox_load_pp->setEnabled(true);
            ui->checkBox_work_pp->setEnabled(true);
            ui->checkBox_report->setEnabled(true);
            ui->checkBox_client->setEnabled(true);
        }
        else
        {
            ui->pushButton_del->setEnabled(false);
            ui->checkBox_ref->setEnabled(false);
            ui->checkBox_load_pp->setEnabled(false);
            ui->checkBox_work_pp->setEnabled(false);
            ui->checkBox_report->setEnabled(false);
            ui->checkBox_client->setEnabled(false);
        }
        ui->pushButton_add->setText("Изменить");
        ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
        ui->lineEdit_name->setEnabled(false);
        ui->checkBox_ref->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString()));
        ui->checkBox_load_pp->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString()));
        ui->checkBox_work_pp->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString()));
        ui->checkBox_client->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(4).data().toString()));
        ui->checkBox_report->setChecked(func_str_to_bool(ui->tableView->selectionModel()->selectedIndexes().at(5).data().toString()));
    }
    else
    {
        ui->lineEdit_name->setEnabled(true);
        ui->pushButton_del->setEnabled(false);
        ui->pushButton_add->setText("Добавить");
        slot_clear_field();
    }
}

//Добавляем/изменяем пользователя
void class_manage_users::slot_add_user()
{
    int status = 0, st = 0;
    if (ui->pushButton_add->text() == "Добавить")
    {
        query->exec("CREATE ROLE " + ui->lineEdit_name->text() + " LOGIN PASSWORD '" + ui->lineEdit_passwd->text() + "'");
        if (!query->lastError().isValid())
        {
            db->transaction();
            if (query->exec("GRANT full_access TO " + ui->lineEdit_name->text())) status++;
            query->clear();
            query->prepare("INSERT INTO users_access (id, ref, load_pp, work_pp, report, client) VALUES(?,?,?,?,?,?)");
            query->addBindValue(ui->lineEdit_name->text());
            if (ui->checkBox_ref->isChecked())
            {
                query->addBindValue(1);
            }
            else
            {
                query->addBindValue(0);
            }
            if (ui->checkBox_load_pp->isChecked())
            {
                query->addBindValue(1);
            }
            else
            {
                query->addBindValue(0);
            }
                if (ui->checkBox_work_pp->isChecked())
            {
                query->addBindValue(1);
            }
            else
            {
            query->addBindValue(0);
            }
            if (ui->checkBox_report->isChecked())
            {
                query->addBindValue(1);
            }
            else
            {
                query->addBindValue(0);
            }
            if (ui->checkBox_client->isChecked())
            {
                query->addBindValue(1);
            }
            else
            {
                query->addBindValue(0);
            }
            if (query->exec()) status++;
            query->clear();
            if (status == 2)
            {
                db->commit();
            }
            else
            {
                db->rollback();
            }
        }
    }
    else
    {
        if (ui->lineEdit_passwd->text() != "")
        {
            if (ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString() == "postgres")
            {
                int ret = QMessageBox::warning(this, tr("Внимание"),
                                            tr("Вы уверены что хотите изменить пароль пользователя postgres?"),
                                            QMessageBox::Yes
                                            | QMessageBox::Cancel,
                                            QMessageBox::Yes);
                switch (ret)
                {
                    case QMessageBox::Yes:
                        if (query->exec("ALTER ROLE " + ui->lineEdit_name->text() + " LOGIN PASSWORD '" + ui->lineEdit_passwd->text() + "'")) status++;
                        st++;
                        break;
                }
            }
            else
            {
                if (query->exec("ALTER ROLE " + ui->lineEdit_name->text() + " LOGIN PASSWORD '" + ui->lineEdit_passwd->text() + "'")) status++;
                st++;
            }
        }
        query->prepare("UPDATE users_access SET ref = ?, load_pp = ?, work_pp = ?, client = ?, report = ? WHERE id = ?");
        if (ui->checkBox_ref->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->checkBox_load_pp->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->checkBox_work_pp->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->checkBox_client->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->checkBox_report->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        query->addBindValue(ui->lineEdit_name->text());
        if (query->exec()) status++;
        st++;
        if (status == st)
        {
            db->commit();
        }
        else
        {
            db->rollback();
        }
    }
    slot_clear_field();
    slot_select_table();
}

//Удаляем пользователя
void class_manage_users::slot_del_user()
{
    int status = 0;
    if (ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString() != "postgres")
    {
        int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Вы уверены что хотите удалить пользователя?"),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
        switch (ret)
        {
            case QMessageBox::Yes:
                db->transaction();
                if (query->exec("DROP ROLE " + ui->lineEdit_name->text())) status++;
                if (query->exec("DELETE FROM users_access WHERE id = '" + ui->lineEdit_name->text() + "'")) status++;
                if (status == 2)
                {
                    db->commit();
                }
                else
                {
                    db->rollback();
                }
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
