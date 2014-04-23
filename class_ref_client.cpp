#include "class_ref_client.h"
#include "ui_class_ref_client.h"

class_ref_client::class_ref_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_client)
{
    ui->setupUi(this);

    query = new QSqlQuery;
    select_tarif();
    slot_enable_edit_tarif();

    model = new QSqlQueryModel;
    select_table();
    model->setHeaderData(1,Qt::Horizontal, "Название");
    model->setHeaderData(2,Qt::Horizontal, "e-mail");
    model->setHeaderData(4,Qt::Horizontal, "Тариф");
    model->setHeaderData(5,Qt::Horizontal, "% за забор");
    model->setHeaderData(6,Qt::Horizontal, "% за транзит вход");
    model->setHeaderData(7,Qt::Horizontal, "% за транзит вход\nстройка");
    model->setHeaderData(8,Qt::Horizontal, "% за транзит выход");
    model->setHeaderData(9,Qt::Horizontal, "% в долг");
    model->setHeaderData(10,Qt::Horizontal, "% за орехи");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnWidth(5, 200);
    ui->tableView->setColumnWidth(6, 150);
    ui->tableView->setColumnWidth(7, 150);
    ui->tableView->setColumnWidth(8, 150);
    ui->tableView->setColumnWidth(9, 150);
    ui->tableView->setColumnWidth(10, 200);
    ui->tableView->setColumnHidden(11,true);

    QValidator *validator = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9],[0-9][0-9]|[0-9][0-9],[0-9][0-9]|[0-9][0-9],[0-9]"), this);
    ui->lineEdit_perc_dolg->setValidator(validator);
    ui->lineEdit_perc_nal->setValidator(validator);
    ui->lineEdit_perc_obnal->setValidator(validator);
    ui->lineEdit_perc_trans_in->setValidator(validator);
    ui->lineEdit_perc_trans_in_s->setValidator(validator);
    ui->lineEdit_perc_trans_out->setValidator(validator);

    //Вклучаем выключаем кнопку добавить/изменить
    {
        connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), SLOT(slot_enable_add_edit()));
    }

    //Включаем выключаем редактор тарифа
    connect(ui->comboBox_tarif, SIGNAL(currentIndexChanged(QString)), SLOT(slot_enable_edit_tarif()));

    //Добавляем изменяем клиента
    connect(ui->pushButton_add, SIGNAL(clicked()), SLOT(slot_add_edit_client()));

    //Включаем/выключаем кнопку удалить
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_del()));

    //Удаляем клиента
    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_del_client()));

    //connect(ui->comboBox_tarif, SIGNAL(activated(int)), SLOT(select_tarif()));
}

//Вклучаем выключаем кнопку добавить/изменить
void class_ref_client::slot_enable_add_edit()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() <= 12)
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
    else
    {
        ui->pushButton_add->setEnabled(false);
    }
}

//Заполняем комбобокс назаниями тарифов
void class_ref_client::select_tarif()
{
    ui->comboBox_tarif->clear();
    int index = 0, i = 0;
    query->exec("SELECT id, name, def FROM tarif ORDER BY id");
    query->first();
    ui->comboBox_tarif->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    while (query->next())
    {
        i++;
        if (query->value(2).toString() == "Да")
        {
            index = i;
        }
        ui->comboBox_tarif->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    }
        ui->comboBox_tarif->setCurrentIndex(index);
}

//Включаем/выключаем редактор тарифов
void class_ref_client::slot_enable_edit_tarif()
{
    if (ui->comboBox_tarif->currentIndex() == 0)
    {
        ui->lineEdit_perc_dolg->setEnabled(true);
        ui->lineEdit_perc_nal->setEnabled(true);
        ui->lineEdit_perc_obnal->setEnabled(true);
        ui->lineEdit_perc_trans_in->setEnabled(true);
        ui->lineEdit_perc_trans_in_s->setEnabled(true);
        ui->lineEdit_perc_trans_out->setEnabled(true);
        ui->lineEdit_perc_dolg->setText("");
        ui->lineEdit_perc_nal->setText("");
        ui->lineEdit_perc_obnal->setText("");
        ui->lineEdit_perc_trans_in->setText("");
        ui->lineEdit_perc_trans_in_s->setText("");
        ui->lineEdit_perc_trans_out->setText("");
    }
    else
    {
        ui->lineEdit_perc_dolg->setEnabled(false);
        ui->lineEdit_perc_nal->setEnabled(false);
        ui->lineEdit_perc_obnal->setEnabled(false);
        ui->lineEdit_perc_trans_in->setEnabled(false);
        ui->lineEdit_perc_trans_in_s->setEnabled(false);
        ui->lineEdit_perc_trans_out->setEnabled(false);
        query->prepare("SELECT t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic FROM tarif WHERE id = ?");
        query->addBindValue(ui->comboBox_tarif->itemData(ui->comboBox_tarif->currentIndex()));
        query->exec();
        query->first();
        ui->lineEdit_perc_obnal->setText(query->value(0).toString());
        ui->lineEdit_perc_trans_in->setText(query->value(1).toString());
        ui->lineEdit_perc_trans_in_s->setText(query->value(2).toString());
        ui->lineEdit_perc_trans_out->setText(query->value(3).toString());
        ui->lineEdit_perc_dolg->setText(query->value(4).toString());
        ui->lineEdit_perc_nal->setText(query->value(5).toString());
    }
}

//Добавляем/редаклируем клиента
void class_ref_client::slot_add_edit_client()
{
    int id = 0;
    if (ui->pushButton_add->text() == "Добавить")
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        query->prepare("INSERT INTO client (name, mail, tarif, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic)"
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_mail->text());
        query->addBindValue(ui->comboBox_tarif->itemData(ui->comboBox_tarif->currentIndex()));
        if (ui->lineEdit_perc_obnal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_obnal->text());
        }
        if (ui->lineEdit_perc_trans_in->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in->text());
        }
        if (ui->lineEdit_perc_trans_in_s->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in_s->text());
        }
        if (ui->lineEdit_perc_trans_out->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_out->text());
        }
        if (ui->lineEdit_perc_dolg->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_dolg->text());
        }
        if (ui->lineEdit_perc_nal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_nal->text());
        }
        query->exec();
        query->clear();
        query->prepare("SELECT id FROM client WHERE name = ?");
        query->addBindValue(ui->lineEdit_name->text());
        query->exec();
        query->first();
        id = query->value(0).toInt();
        query->clear();
        query->prepare("INSERT INTO client_balans (id, balans) "
                       "VALUES (?, 0)");
        query->addBindValue(id);
        query->exec();
        query->clear();
        query->exec("COMMIT");
        clear_field();
        select_table();
    }
    else
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        query->prepare("UPDATE client SET name = ?, mail = ?, tarif = ?, t_obnal = ?, t_trans_in = ?, t_trans_in_s = ?, t_trans_out = ?, t_kred = ?, t_nalic = ? "
                       "WHERE id = ?");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_mail->text());
        query->addBindValue(ui->comboBox_tarif->itemData(ui->comboBox_tarif->currentIndex()));
        if (ui->lineEdit_perc_obnal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_obnal->text());
        }
        if (ui->lineEdit_perc_trans_in->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in->text());
        }
        if (ui->lineEdit_perc_trans_in_s->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in_s->text());
        }
        if (ui->lineEdit_perc_trans_out->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_out->text());
        }
        if (ui->lineEdit_perc_dolg->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_dolg->text());
        }
        if (ui->lineEdit_perc_nal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_nal->text());
        }
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
        query->exec();
        query->clear();
        query->exec("COMMIT");
        clear_field();
        select_table();
    }
}

//Включаем/выключаем кнопку удалить
void class_ref_client::slot_enable_del()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() <= 12)
    {
        ui->pushButton_del->setEnabled(ui->tableView->selectionModel()->hasSelection());
        ui->pushButton_add->setEnabled(ui->tableView->selectionModel()->hasSelection());
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_add->setText("Изменить");
            ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString());
            ui->lineEdit_mail->setText(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString());
            int index = 0;
            for (index; ui->comboBox_tarif->itemData(index).toInt() != ui->tableView->selectionModel()->selectedIndexes().at(3).data().toInt();index++)
            {
            }
            ui->comboBox_tarif->setCurrentIndex(index);
            ui->lineEdit_perc_obnal->setText(ui->tableView->selectionModel()->selectedIndexes().at(5).data().toString());
            ui->lineEdit_perc_trans_in->setText(ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString());
            ui->lineEdit_perc_trans_in_s->setText(ui->tableView->selectionModel()->selectedIndexes().at(7).data().toString());
            ui->lineEdit_perc_trans_out->setText(ui->tableView->selectionModel()->selectedIndexes().at(8).data().toString());
            ui->lineEdit_perc_dolg->setText(ui->tableView->selectionModel()->selectedIndexes().at(9).data().toString());
            ui->lineEdit_perc_nal->setText(ui->tableView->selectionModel()->selectedIndexes().at(10).data().toString());
            index = 0;
        }
        else
        {
            ui->pushButton_add->setText("Добавить");
            clear_field();
        }
    }
    else
    {
        ui->pushButton_del->setEnabled(false);
        ui->pushButton_add->setEnabled(false);
        clear_field();
    }
}

//Удаляем клиента
void class_ref_client::slot_del_client()
{
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->prepare("DELETE FROM client WHERE id = ?");
    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    query->exec();
    query->clear();
    query->prepare("DELETE FROM client_balans WHERE id = ?");
    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    query->exec();
    query->clear();
    query->exec("COMMIT");
    clear_field();
    select_table();
}

//Очищаем поля
void class_ref_client::clear_field()
{
    ui->lineEdit_name->setText("");
    ui->lineEdit_mail->setText("");
    select_tarif();
}

//Обновляем таблицу
void class_ref_client::select_table()
{
    model->setQuery("SELECT client.id, client.name, client.mail, tarif.id, tarif.name, client.t_obnal, client.t_trans_in, client.t_trans_in_s, client.t_trans_out, client.t_kred, client.t_nalic FROM client "
                    "LEFT JOIN tarif ON tarif.id = client.tarif "
                    "WHERE client.id > 0");
    ui->tableView->setModel(model);
    ui->tableView->show();
}

class_ref_client::~class_ref_client()
{
    delete ui;
}
