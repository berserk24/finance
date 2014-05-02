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
    id_column = 0;
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

    //Сортировка
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slot_sort_pp(int)));

    //Считаем сумму выделенных строк
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_sum_client_oper()));

    select_client();
    slot_select_table();
}

//Считаем сумму выделенных строк
void class_ref_client_pay::slot_sum_client_oper()
{
    double sum = 0;
    if (ui->tableView->selectionModel()->hasSelection())
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            sum += ui->tableView->selectionModel()->selectedRows(8).at(i).data().toDouble();
        }
    emit signal_send_sum_opers("Сумма по выделенным операциям: " + QString::number(sum, 'f', 2));
}

//Включаем\Выключаем кнопку отмены транзакции
void class_ref_client_pay::slot_enable_cancel_button()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() > 13)
    {
        ui->pushButton_cancel->setEnabled(ui->tableView->selectionModel()->hasSelection());
    }
    else
    {
        ui->pushButton_cancel->setEnabled(false);
    }
}

//Сортировка по столбцу
void class_ref_client_pay::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    slot_select_table();
}

//Отменяем платёж
void class_ref_client_pay::slot_cancel_pay()
{
    QString str = "ёж?";
    if (ui->tableView->selectionModel()->selectedIndexes().size() > 14) str = "ежи?";
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Вы уверены что хотите отменить плат" + str.toUtf8()),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    switch (ret) {
        case QMessageBox::Yes:
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            ucb->slot_cancel_pay(ui->tableView->selectionModel()->selectedRows(10).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(11).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(6).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(8).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(9).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(12).at(i).data().toString(),
                                 ui->tableView->selectionModel()->selectedRows(13).at(i).data().toString()
                                 );
        }
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
        str_query += " AND oper.summ LIKE '%" + ui->lineEdit_summ->text() + "%' ";
    if (id_column == 0) str_query += "ORDER BY clients.name ";
    if (id_column == 1) str_query += "ORDER BY oper.date_oper ";
    if (id_column == 2) str_query += "ORDER BY pp.num ";
    if (id_column == 3) str_query += "ORDER BY rss.name ";
    if (id_column == 4) str_query += "ORDER BY COALESCE(pp.payer1 , pp.payer) ";
    if (id_column == 5) str_query += "ORDER BY COALESCE(pp.receiver1, pp.receiver) ";
    if (id_column == 6) str_query += "ORDER BY pio.data ";
    if (id_column == 7) str_query += "ORDER BY oper.text ";
    if (id_column == 8) str_query += "ORDER BY oper.summ ";
    if (id_column == 9) str_query += "ORDER BY oper.margin";
    if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    {
        str_query += " DESC";
    }
    else
    {
        str_query += " ASC";
    }
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
