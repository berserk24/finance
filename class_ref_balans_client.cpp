#include "class_ref_balans_client.h"
#include "ui_class_ref_balans_client.h"

class_ref_balans_client::class_ref_balans_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_balans_client)
{
    ui->setupUi(this);
    model = new QSqlQueryModel;
    query = new QSqlQuery;
    str_query = new QString;
    ucb = new update_client_balans;

    ui->lineEdit_summ->setMaximumWidth(150);
    ui->lineEdit_margin->setMaximumWidth(50);

    QValidator *validator = new QRegExpValidator(QRegExp("^[1-9]\\d{0,8}|^[1-9]\\d{0,8},[0-9][0-9]|^0,[0-9][0-9]"), this);
    ui->lineEdit_summ->setValidator(validator);
    QValidator *validator2 = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9],[0-9][0-9]|[0-9][0-9],[0-9][0-9]|[0-9][0-9],[0-9]"), this);
    ui->lineEdit_margin->setValidator(validator2);

    slot_select_client();
    slot_select_table();

    //Фильтр по имени контрагента
    connect(ui->comboBox_client, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_table()));

    //Включаем выключаем кнопку Пополнить/выдать
    connect(ui->lineEdit_summ, SIGNAL(textChanged(QString)), SLOT(slot_enable_add_button()));
    connect(ui->comboBox_action, SIGNAL(currentIndexChanged(int)), SLOT(slot_enable_add_button()));
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_add_button()));

    connect(ui->comboBox_action, SIGNAL(currentIndexChanged(int)), SLOT(slot_enable_to_client()));
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_to_client()));

    //Изменяем баланс
    connect(ui->pushButton_add, SIGNAL(clicked()), SLOT(slot_edit_balans()));

    //Считаем сумму выделенных строк
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_sum_balans_client()));

}

//Считаем сумму выделенных строк
void class_ref_balans_client::slot_sum_balans_client()
{
    double sum = 0;
    if (ui->tableView->selectionModel()->hasSelection())
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            sum += ui->tableView->selectionModel()->selectedRows(1).at(i).data().toDouble();
        }
    emit signal_send_sum_client("Сумма по выделенным счетам контрагентов: " + QString::number(sum, 'f', 2));
}

//Включаем/Выключаем клиентов для перевода
void class_ref_balans_client::slot_enable_to_client()
{
    if (ui->comboBox_action->currentText() == "Перевод")
    {
        ui->pushButton_add->setText("Перевести");
        ui->comboBox_to_client->setEnabled(true);
        ui->lineEdit_margin->setEnabled(true);

    }
    if(ui->comboBox_action->currentText() == "Пополнить")
    {
        ui->pushButton_add->setText("Пополнить");
        ui->comboBox_to_client->setEnabled(false);
        ui->lineEdit_margin->setEnabled(true);
    }
    if(ui->comboBox_action->currentText() == "Выдать")
    {
        ui->pushButton_add->setText("Выдать");
        ui->comboBox_to_client->setEnabled(false);
        ui->lineEdit_margin->setEnabled(true);
    }

}

//Заполняем комбобокс клиентами
void class_ref_balans_client::slot_select_client()
{
    ui->comboBox_client->clear();
    ui->comboBox_to_client->clear();
    ui->comboBox_client->addItem("Все", 0);
    query->exec("SELECT id, name FROM client");
    query->first();
    ui->comboBox_client->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    ui->comboBox_to_client->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    while (query->next())
    {
        ui->comboBox_client->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
        ui->comboBox_to_client->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    }
    query->clear();
}

//Заполняем таблицу
void class_ref_balans_client::slot_select_table()
{
    *str_query = "SELECT client.name, balans.balans, client.id "
                   "FROM client_balans balans "
                   "LEFT JOIN client ON client.id = balans.id "
                   "WHERE client.name LIKE '";
    if (ui->comboBox_client->currentText() == "Все")
    {
        *str_query = *str_query + "%'";
    }
    else
    {
        *str_query = *str_query + "%" + ui->comboBox_client->currentText() + "%'";
    }
    model->setQuery(*str_query);
    ui->tableView->setModel(model);
    model->setHeaderData(0,Qt::Horizontal, "Контрагент");
    model->setHeaderData(1,Qt::Horizontal, "Баланс");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnHidden(2, true);
}

//Заполняем поле коммиссии
void class_ref_balans_client::slot_set_margin()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 3)
    {
        query->prepare("SELECT t_obnal, t_nalic FROM client WHERE id = ?");
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString());
        query->exec();
        query->first();
        if (ui->comboBox_action->currentText() == "Пополнить")
        {
            ui->lineEdit_margin->setText(query->value(1).toString());
        }
        else if (ui->comboBox_action->currentText() == "Выдать")
        {
            ui->lineEdit_margin->setText(query->value(0).toString());
        }
        else
        {
            ui->lineEdit_margin->setText("");
        }
        query->clear();
    }
    else
    {
        ui->lineEdit_margin->setText("");
    }
}

//Включаем выключаем кнопку пополнить списать
void class_ref_balans_client::slot_enable_add_button()
{
    slot_set_margin();
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 3)
    {
        //if (ui->tableView->selectionModel()->selectedIndexes().at(2).data().toInt() > 0)
        {
            if (ui->comboBox_action->currentText() == "Перевод")
            {

            }
            if (ui->lineEdit_summ->text() != "" and ui->tableView->selectionModel()->hasSelection())
            {
                ui->pushButton_add->setEnabled(true);
            }
            else
            {
                ui->pushButton_add->setEnabled(false);
            }
            if(ui->comboBox_action->currentText() == "Пополнить")
            {
                ui->pushButton_add->setText("Пополнить");
            }
            if(ui->comboBox_action->currentText() == "Выдать")
            {
                ui->pushButton_add->setText("Выдать");
            }
        }
    }
    else
    {
        ui->pushButton_add->setEnabled(false);
    }
}


//Пополняем или списываем со счёта
void class_ref_balans_client::slot_edit_balans()
{
    QString type;
    if (ui->comboBox_action->currentText() != "Перевод")
    {
        if (ui->comboBox_action->currentText() == "Пополнить")
        {
            type = "Приход";
        }
        if (ui->comboBox_action->currentText() == "Выдать")
        {
            type = "Расход";
        }
        ucb->slot_update_balans(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString(),
                                type,
                                ui->lineEdit_summ->text(),
                                "",
                                true,
                                ui->lineEdit_about->text(),
                                false,
                                ui->lineEdit_margin->text(),
                                ""
                                );
    }
    else
    {
        ucb->slot_update_balans(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString(),
                                "Перевод",
                                ui->lineEdit_summ->text(),
                                "",
                                true,
                                ui->lineEdit_about->text(),
                                true,
                                ui->lineEdit_margin->text(),
                                ui->comboBox_to_client->itemData(ui->comboBox_to_client->currentIndex()).toString()
                                );
    }
    slot_clear_field();
    slot_select_table();
}

//Очищаем форму
void class_ref_balans_client::slot_clear_field()
{
    ui->comboBox_action->setCurrentIndex(0);
    ui->lineEdit_summ->clear();
    ui->lineEdit_about->clear();
    ui->lineEdit_margin->clear();
}


class_ref_balans_client::~class_ref_balans_client()
{
    delete ui;
}