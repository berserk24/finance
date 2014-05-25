#include "class_ref_tarif.h"
#include "ui_class_ref_tarif.h"

class_ref_tarif::class_ref_tarif(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_tarif)
{
    ui->setupUi(this);

    model = new QSqlQueryModel;
    query = new QSqlQuery;

    id_column = 0;

    select_table();
    model->setHeaderData(1,Qt::Horizontal, "Название");
    model->setHeaderData(2,Qt::Horizontal, "По умолчанию");
    model->setHeaderData(3,Qt::Horizontal, "% за забор");
    model->setHeaderData(4,Qt::Horizontal, "% за транзит вход");
    model->setHeaderData(5,Qt::Horizontal, "% за транзит вход\nстройка");
    model->setHeaderData(6,Qt::Horizontal, "% за транзит выход");
    model->setHeaderData(7,Qt::Horizontal, "% в долг");
    model->setHeaderData(8,Qt::Horizontal, "% за орехи");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnWidth(3, 100);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnWidth(5, 150);
    ui->tableView->setColumnWidth(6, 150);
    ui->tableView->setColumnWidth(7, 100);
    ui->tableView->setColumnWidth(8, 100);

    QValidator *validator = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9].[0-9][0-9]|[0-9][0-9].[0-9][0-9]|[0-9][0-9].[0-9]"), this);
    ui->lineEdit_perc_dolg->setValidator(validator);
    ui->lineEdit_perc_nal->setValidator(validator);
    ui->lineEdit_perc_obnal->setValidator(validator);
    ui->lineEdit_perc_trans_out->setValidator(validator);
    ui->lineEdit_perc_trans_in->setValidator(validator);
    ui->lineEdit_perc_trans_in_s->setValidator(validator);

    //Включем/выключаем кнопку добавить/изменить
    {
        connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), SLOT(slot_enable_add_edit()));
    }

    //Добавляем/изменяем тариф
    connect(ui->pushButton_add_edit, SIGNAL(clicked()), SLOT(slot_add_edit_tarif()));

    //Включаем кнопку удалить, заполняем поля
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_del()));

    //Удаляем тариф
    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_del_tarif()));

    //Сортировка
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slot_sort_pp(int)));
}

//Включем/выключаем кнопку добавить/изменить
void class_ref_tarif::slot_enable_add_edit()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 10)
    {
        if (ui->lineEdit_name->text() !=  "")
        {
            ui->pushButton_add_edit->setEnabled(true);
        }
        else
        {
            ui->pushButton_add_edit->setEnabled(false);
        }
    }
    else
    {
        ui->pushButton_add_edit->setEnabled(false);
    }
}

//Добавляем/изменяем тариф
void class_ref_tarif::slot_add_edit_tarif()
{
    if (ui->pushButton_add_edit->text() == "Добавить")
    {
        if (ui->checkBox_def->isChecked())
        {
            clear_default();
        }
        query->prepare("INSERT INTO tarifs (name, def, t_obnal, t_trans_in, t_trans_in_s, t_trans_out, t_kred, t_nalic)"
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        if (ui->checkBox_def->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->lineEdit_perc_obnal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_obnal->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_in->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_in_s->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in_s->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_out->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_out->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_dolg->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_dolg->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_nal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_nal->text().replace(",", "."));
        }
    }
    else
    {
        if (ui->checkBox_def->isChecked())
        {
            clear_default();
        }
        query->prepare("UPDATE tarifs SET name = ?, def = ?, t_obnal = ?, t_trans_in = ?, t_trans_in_s = ?, t_trans_out = ?, t_kred = ?, t_nalic = ? WHERE id = ?");
        query->addBindValue(ui->lineEdit_name->text());
        if (ui->checkBox_def->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        if (ui->lineEdit_perc_obnal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_obnal->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_in->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_in_s->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_in_s->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_trans_out->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_trans_out->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_dolg->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_dolg->text().replace(",", "."));
        }
        if (ui->lineEdit_perc_nal->text() == "")
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->lineEdit_perc_nal->text().replace(",", "."));
        }
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    }
    query->exec();
    query->clear();
    clear_field();
    select_table();
}

//Очищаем поля
void class_ref_tarif::clear_field()
{
    ui->lineEdit_name->setText("");
    ui->checkBox_def->setChecked(false);
    ui->lineEdit_perc_dolg->setText("");
    ui->lineEdit_perc_nal->setText("");
    ui->lineEdit_perc_obnal->setText("");
    ui->lineEdit_perc_trans_in->setText("");
    ui->lineEdit_perc_trans_in_s->setText("");
    ui->lineEdit_perc_trans_out->setText("");
}

//Сортировка по столбцу
void class_ref_tarif::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    select_table();
}

//Обновляем таблицу
void class_ref_tarif::select_table()
{
    query_str = "SELECT tarifs.id, tarifs.name, yn.data, tarifs.t_obnal, "
                "tarifs.t_trans_in, tarifs.t_trans_in_s, tarifs.t_trans_out, "
                "tarifs.t_kred, tarifs.t_nalic "
                "FROM tarifs "
                "LEFT JOIN yes_no yn ON yn.id = tarifs.def "
                "WHERE tarifs.id > 0 ";
    if (id_column == 0) query_str += "ORDER BY tarifs.name ";
    if (id_column == 1) query_str += "ORDER BY tarifs.name ";
    if (id_column == 2) query_str += "ORDER BY yn.data ";
    if (id_column == 3) query_str += "ORDER BY tarifs.t_obnal ";
    if (id_column == 4) query_str += "ORDER BY tarifs.t_trans_in ";
    if (id_column == 5) query_str += "ORDER BY tarifs.t_trans_in_s ";
    if (id_column == 6) query_str += "ORDER BY tarifs.t_trans_out ";
    if (id_column == 7) query_str += "ORDER BY tarifs.t_kred ";
    if (id_column == 8) query_str += "ORDER BY tarifs.t_nalic ";
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
    ui->tableView->show();
}

//Включаем кнопку удалить, заполняем поля
void class_ref_tarif::slot_enable_del()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 10)
    {
        ui->pushButton_del->setEnabled(ui->tableView->selectionModel()->hasSelection());
        ui->pushButton_add_edit->setEnabled(ui->tableView->selectionModel()->hasSelection());
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_add_edit->setText("Изменить");
            ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString());
            if (ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString() == "Да")
            {
                ui->checkBox_def->setChecked(true);
            }
            else
            {
                ui->checkBox_def->setChecked(false);
            }
            ui->lineEdit_perc_obnal->setText(ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString());
            ui->lineEdit_perc_trans_in->setText(ui->tableView->selectionModel()->selectedIndexes().at(4).data().toString());
            ui->lineEdit_perc_trans_in_s->setText(ui->tableView->selectionModel()->selectedIndexes().at(5).data().toString());
            ui->lineEdit_perc_trans_out->setText(ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString());
            ui->lineEdit_perc_dolg->setText(ui->tableView->selectionModel()->selectedIndexes().at(7).data().toString());
            ui->lineEdit_perc_nal->setText(ui->tableView->selectionModel()->selectedIndexes().at(8).data().toString());
        }
        else
        {
            ui->pushButton_add_edit->setText("Добавить");
            clear_field();
        }
    }
    else
    {
        ui->pushButton_del->setEnabled(false);
        ui->pushButton_add_edit->setEnabled(false);
        ui->checkBox_def->setChecked(false);
        clear_field();
    }
}

//Удаляем тариф
void class_ref_tarif::slot_del_tarif()
{
    query->prepare("SELECT id FROM clients WHERE tarif = ?");
    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    query->exec();
    if (query->first())
    {
        QMessageBox::critical(this, "Ошибка", "Этот тариф установлен для некоторых клиентов.");
    }
    else
    {
        query->prepare("DELETE FROM tarifs WHERE id = ?");
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
        query->exec();
        query->clear();
        clear_field();
        select_table();
    }
}

//Убираем с тарифа атрибут по умолчанию
void class_ref_tarif::clear_default()
{
    query_def = new QSqlQuery;
    query_def->prepare("SELECT id FROM tarifs WHERE def = '1'");
    query_def->exec();
    if (query_def->first())
    {
        int id = query_def->value(0).toInt();
        query->clear();
        query_def->prepare("UPDATE tarifs SET def = 0 WHERE id = ?");
        query_def->addBindValue(id);
        query_def->exec();
        query_def->clear();
    }
}

class_ref_tarif::~class_ref_tarif()
{
    delete ui;
}
