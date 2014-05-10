#include "class_ref_auto_actions.h"
#include "ui_class_ref_auto_actions.h"

class_ref_auto_actions::class_ref_auto_actions(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_ref_auto_actions)
{
    ui->setupUi(this);

    db = db1;

    model = new QSqlQueryModel;
    query = new QSqlQuery;

    QValidator *validator = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9].[0-9][0-9]|[0-9][0-9].[0-9][0-9]|[0-9][0-9].[0-9]"), this);
    ui->lineEdit_margin->setValidator(validator);

    slot_select_table();

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_button()));

    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_del_actions()));

    connect(ui->pushButton_update, SIGNAL(clicked()), SLOT(slot_update_action()));

    //Сортировка
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slot_sort_pp(int)));
}

//Обновляем комиссию
void class_ref_auto_actions::slot_update_action()
{
    QString str = QString::null;
    query->prepare("UPDATE save_actions SET tarif = ? WHERE id = ?");
    if (ui->lineEdit_margin->text() != "")
    {
        query->addBindValue(ui->lineEdit_margin->text());
    }
    else
    {
        query->addBindValue(str);
    }
    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    query->exec();
    query->clear();
    ui->lineEdit_margin->setText("");
    ui->tableView->clearSelection();
    slot_select_table();
}

//Сортировка по столбцу
void class_ref_auto_actions::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    slot_select_table();
}


//Заполняем таблицу
void class_ref_auto_actions::slot_select_table()
{
    QString query_str = "SELECT * FROM action_ ";
    if (id_column == 0) query_str += "ORDER BY action_.id ";
    if (id_column == 1) query_str += "ORDER BY action_.client_name_ ";
    if (id_column == 2) query_str += "ORDER BY action_.inn_ ";
    if (id_column == 3) query_str += "ORDER BY action_.firm_name_ ";
    if (id_column == 4) query_str += "ORDER BY action_.pp_type_ ";
    if (id_column == 5) query_str += "ORDER BY action_.tarif_ ";
    if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    {
        query_str += " DESC";
    }
    else
    {
        query_str += " ASC";
    }
    model->setQuery(query_str);
    model->setHeaderData(1,Qt::Horizontal, "Контрагент");
    model->setHeaderData(2,Qt::Horizontal, "ИНН");
    model->setHeaderData(3,Qt::Horizontal, "Название фирмы");
    model->setHeaderData(4,Qt::Horizontal, "Тип ПП");
    model->setHeaderData(5,Qt::Horizontal, "Тариф %");
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 150);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnWidth(3, 300);
    ui->tableView->setColumnWidth(4, 100);
    ui->tableView->setColumnWidth(5, 100);
    ui->tableView->setColumnHidden(6,true);
}

//Включаем выключаем кнопки
void class_ref_auto_actions::slot_enable_button()
{
    ui->pushButton_del->setEnabled(true);
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 7)
    {
        ui->pushButton_update->setEnabled(true);
        ui->lineEdit_margin->setText(ui->tableView->selectionModel()->selectedIndexes().at(5).data().toString());
    }
    else
    {
        ui->pushButton_update->setEnabled(false);
        ui->lineEdit_margin->setText("");
    }
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 0)
    {
        ui->pushButton_del->setEnabled(false);
    }
}

//Удаляем сохранённые действия
void class_ref_auto_actions::slot_del_actions()
{
    QString str;
    if (ui->tableView->selectionModel()->selectedIndexes().size() > 7)
    {
        str = "я?";
    }
    else
    {
        str = "е?";
    }
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Вы уверены что хотите удалить действи" + str.toUtf8()),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    switch (ret) {
        case QMessageBox::Yes:
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            query->prepare("DELETE FROM save_actions WHERE id = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedRows(0).at(i).data().toString());
            query->exec();
            query->clear();
        }
    }
    ui->tableView->clearSelection();
    slot_select_table();
}

class_ref_auto_actions::~class_ref_auto_actions()
{
    delete ui;
}
