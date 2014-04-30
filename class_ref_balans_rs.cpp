#include "class_ref_balans_rs.h"
#include "ui_class_ref_balans_rs.h"

class_ref_balans_rs::class_ref_balans_rs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_balans_rs)
{
    ui->setupUi(this);
    model = new QSqlQueryModel;
    id_column = 0;

    slot_select_table();
    model->setHeaderData(0,Qt::Horizontal, "Организация");
    model->setHeaderData(1,Qt::Horizontal, "Расчётный счёт");
    model->setHeaderData(2,Qt::Horizontal, "Дата");
    model->setHeaderData(3,Qt::Horizontal, "Баланс");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->setColumnWidth(3, 100);

    slot_sum_balans_rs();

    //Считаем сумму выделенных строк
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_sum_balans_rs()));

    //Сортировка
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slot_sort_pp(int)));
}

//Сортировка по столбцу
void class_ref_balans_rs::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    slot_select_table();
}

void class_ref_balans_rs::slot_select_table()
{
    query_str = "SELECT firms.name, rss.name, balans.last_date, CAST(balans.balans + rss.start_balans AS VARCHAR(20)) "
                "FROM rss_balans balans "
                "LEFT JOIN rss ON balans.id = rss.id "
                "LEFT JOIN firms ON rss.firm = firms.id ";
    if (id_column == 0) query_str += "ORDER BY firms.name ";
    if (id_column == 1) query_str += "ORDER BY rss.name ";
    if (id_column == 2) query_str += "ORDER BY balans.last_date ";
    if (id_column == 3) query_str += "ORDER BY (balans.balans + rss.start_balans) ";
    if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    {
        query_str += " DESC";
    }
    else
    {
        query_str += " ASC";
    }

    model->setQuery(query_str);

    ui->tableView->setModel(model);
}

//Считаем сумму выделенных строк
void class_ref_balans_rs::slot_sum_balans_rs()
{
    double sum = 0;
    if (ui->tableView->selectionModel()->hasSelection())
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            sum += ui->tableView->selectionModel()->selectedRows(3).at(i).data().toDouble();
        }
    emit signal_send_sum_rs("Сумма по выделенным расчётным счетам: " + QString::number(sum, 'f', 2));
}

class_ref_balans_rs::~class_ref_balans_rs()
{
    delete ui;
}
