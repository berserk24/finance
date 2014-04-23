#include "class_ref_balans_rs.h"
#include "ui_class_ref_balans_rs.h"

class_ref_balans_rs::class_ref_balans_rs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_balans_rs)
{
    ui->setupUi(this);
    model = new QSqlQueryModel;

    ui->dateEdit->setDate(QDate::currentDate());

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

    connect(ui->dateEdit, SIGNAL(dateChanged(QDate)), SLOT(slot_select_table()));

}

void class_ref_balans_rs::slot_select_table()
{
    /*model->setQuery("   SELECT firm.name, rss.name, strftime('%d.%m.%Y', date(julianday(balans.date))), balans.balans"
                        " FROM rss_balans balans"
                        " LEFT JOIN rss ON balans.id = rss.id"
                        " LEFT JOIN firm ON rss.firm = firm.id");
    */
    model->setQuery("SELECT IFNULL(_in.firm, _out.firm) AS rs, IFNULL(_in.rs, _out.rs) AS rs, strftime('%d.%m.%Y', date(julianday(IFNULL(_in.date, _out.date)))) AS date, CAST(ROUND((IFNULL(_in.sum, 0) - IFNULL(_out.sum, 0)), 2) AS TEXT) as sum FROM( "
                        "SELECT firm.name firm, rss.name rs, MAX(pp.date) date, SUM(pp.sum) sum, pp.type type "
                        "FROM pp "
                        "LEFT JOIN rss ON rss.id = pp.rs_id "
                        "LEFT JOIN firm ON firm.id = rss.firm "
                        "WHERE pp.type = '2' "
                        "AND pp.date <= " + QString::number(ui->dateEdit->date().toJulianDay()) +
                        " GROUP BY pp.rs_id, pp.type) _in "
                    "LEFT JOIN ( "
                        "SELECT firm.name firm, rss.name rs, MAX(pp.date) date, SUM(pp.sum) sum, pp.type type "
                        "FROM pp "
                        "LEFT JOIN rss ON rss.id = pp.rs_id "
                        "LEFT JOIN firm ON firm.id = rss.firm "
                        "WHERE pp.type = '1' "
                        "AND pp.date <= " + QString::number(ui->dateEdit->date().toJulianDay()) +
                        " GROUP BY pp.rs_id, pp.type) _out "
                        "ON _in.rs = _out.rs");

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
