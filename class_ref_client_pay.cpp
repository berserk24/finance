#include "class_ref_client_pay.h"
#include "ui_class_ref_client_pay.h"

class_ref_client_pay::class_ref_client_pay(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_ref_client_pay)
{
    ui->setupUi(this);

    model = new QSqlQueryModel;
    query = new QSqlQuery;

    db = db1;
    ui->tableView->setModel(model);

    ucb = new update_client_balans(0, db);

    //ui->comboBox_client->setMinimumHeight(27);

    ui->dateEdit_po->setDate(QDate::currentDate());
    ui->dateEdit_s->setDate(QDate::currentDate().addDays(-1));

    //Фильтр
    connect(ui->comboBox_client, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_table()));
    connect(ui->dateEdit_s, SIGNAL(dateChanged(QDate)), SLOT(slot_select_table()));
    connect(ui->dateEdit_po, SIGNAL(dateChanged(QDate)), SLOT(slot_select_table()));
    connect(ui->lineEdit_summ, SIGNAL(textChanged(QString)), SLOT(slot_select_table()));

    //Включаем/Выключаем кнопку отмены транзакции
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_cancel_button()));

    //Отменяем платёж
    connect(ui->pushButton_cancel, SIGNAL(clicked()), SLOT(slot_cancel_pay()));

    select_client();
    slot_select_table();
}

//Включаем\Выключаем кнопку отмены транзакции
void class_ref_client_pay::slot_enable_cancel_button()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 14)
    {
        ui->pushButton_cancel->setEnabled(ui->tableView->selectionModel()->hasSelection());
    }
    else
    {
        ui->pushButton_cancel->setEnabled(false);
    }
}

//Отменяем платёж
void class_ref_client_pay::slot_cancel_pay()
{
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Вы уверены что хотите отменить платёж?"),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    switch (ret) {
       case QMessageBox::Yes:
            ucb->slot_cancel_pay(ui->tableView->selectionModel()->selectedIndexes().at(10).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(11).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(8).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(9).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString(),
                                 ui->tableView->selectionModel()->selectedIndexes().at(13).data().toString()
                                 );
            ui->tableView->selectionModel()->clearSelection();
            slot_select_table();
            break;
       case QMessageBox::Cancel:
           return;
           break;
     }
}

//Заполняем список клиентов
void class_ref_client_pay::select_client()
{
    ui->comboBox_client->clear();
    ui->comboBox_client->addItem("Все", 0);
    query->exec("SELECT id, name FROM clients");
    query->first();
    ui->comboBox_client->addItem(query->value(1).toString(), query->value(0).toInt());
    while (query->next())
    {
        ui->comboBox_client->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    query->clear();
}

//Заполняем таблицу
void class_ref_client_pay::slot_select_table()
{
    ui->tableView->resizeRowsToContents();
    QString str_query = "SELECT clients.name, oper.date_oper, pp.num, rss.name, COALESCE(pp.payer1 , pp.payer), COALESCE(pp.receiver1, pp.receiver), pio.data, oper.text, CAST(oper.summ AS VARCHAR(18)), CAST(oper.margin AS VARCHAR(18)), clients.id, pp.id, oper.id, oper.to_client_id "
                        "FROM clients_operations oper "
                        "LEFT JOIN clients ON oper.id_client = clients.id "
                        "LEFT JOIN pp ON oper.id_pp = pp.id "
                        "LEFT JOIN pp_in_out pio ON oper.type_pp = pio.id "
                        "LEFT JOIN rss ON pp.rs_id = rss.id "
                        "WHERE oper.date_oper <= '" + ui->dateEdit_po->date().toString("dd.MM.yyyy") +
                        "' AND oper.date_oper >= '" + ui->dateEdit_s->date().toString("dd.MM.yyyy") + "'";
    if (ui->comboBox_client->currentIndex() > 0)
        str_query += " AND client.id = " + ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString();
    if (ui->lineEdit_summ->text() != "")
        str_query += " AND oper.summ LIKE '%" + ui->lineEdit_summ->text() + "%'";
    model->setQuery(str_query);
    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    model->setHeaderData(0,Qt::Horizontal, "Контрагент");
    model->setHeaderData(1,Qt::Horizontal, "Дата");
    model->setHeaderData(2,Qt::Horizontal, "Номер\nплатёжки");
    model->setHeaderData(3,Qt::Horizontal, "Расчётный\nсчёт");
    model->setHeaderData(4,Qt::Horizontal, "Отправитель");
    model->setHeaderData(5,Qt::Horizontal, "Получатель");
    model->setHeaderData(6,Qt::Horizontal, "Тип");
    model->setHeaderData(7,Qt::Horizontal, "Назначение\nплатежа");
    model->setHeaderData(8,Qt::Horizontal, "Сумма");
    model->setHeaderData(9,Qt::Horizontal, "Комиссия");
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setColumnHidden(12,true);
    ui->tableView->setColumnHidden(13,true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 100);
    ui->tableView->setColumnWidth(5, 100);
    ui->tableView->setColumnWidth(6, 100);
    ui->tableView->setColumnWidth(7, 400);
    ui->tableView->setColumnWidth(8, 100);
    ui->tableView->setColumnWidth(9, 100);
    show();
    ui->tableView->resizeRowsToContents();
    ui->tableView->setAlternatingRowColors(true);
}


class_ref_client_pay::~class_ref_client_pay()
{
    delete ui;
}
