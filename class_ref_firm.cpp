#include "class_ref_firm.h"
#include "ui_class_ref_firm.h"

class_ref_firm::class_ref_firm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_firm)
{
    ui->setupUi(this);

    query = new QSqlQuery;

    id_column = 0;

    QValidator *validator = new QRegExpValidator(QRegExp("[1-9]\\d{11}"), this);
    ui->lineEdit_inn->setValidator(validator);

    model = new QSqlQueryModel;
    refresh_tableview();

    //Включаем кнопку добавить/изменить
    connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));
    connect(ui->lineEdit_inn, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));

    //Добавляем фирму
    connect(ui->pushButton_add_firm, SIGNAL(clicked()), SLOT(slot_add_firm()));

    //Включаем выключеам кнопку удалить
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_del()));

    //Удаляем фирму
    connect(ui->pushButton_del_firm, SIGNAL(clicked()), SLOT(slot_del_firm()));

    //connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(slot_show_add_rs()));

    //Сортировка
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slot_sort_pp(int)));
}

//Включаем кнопку добавить/изменить
void class_ref_firm::slot_enable_add()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 5)
    {
        if (ui->lineEdit_name->text() != "" and (ui->lineEdit_inn->text().length() == 12 or ui->lineEdit_inn->text().length() == 10))
        {
            ui->pushButton_add_firm->setEnabled(true);
        }
        else
        {
            ui->pushButton_add_firm->setEnabled(false);
        }
    }
    else
    {
        ui->pushButton_add_firm->setEnabled(false);
    }
}

//Добавляем фирму
void class_ref_firm::slot_add_firm()
{
    if (ui->tableView->selectionModel()->hasSelection())
    {
        query->prepare("UPDATE firms SET name = ?, inn = ?, stroy = ? WHERE id = ?");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_inn->text());
        if (ui->checkBox_stroy->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    }
    else
    {
        query->prepare("INSERT INTO firms (name, inn, stroy)"
                            "VALUES (?, ?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_inn->text());
        if (ui->checkBox_stroy->isChecked())
        {
            query->addBindValue(1);
        }
        else
        {
            query->addBindValue(0);
        }
    }
    query->exec();
    query->clear();
    ui->lineEdit_name->setText("");
    ui->lineEdit_inn->setText("");
    ui->checkBox_stroy->setChecked(false);
    ui->pushButton_del_firm->setEnabled(false);
    refresh_tableview();
}


//Включаем выключаем кнопку удалить
void class_ref_firm::slot_enable_del()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 5)
    {
        ui->pushButton_del_firm->setEnabled(ui->tableView->selectionModel()->hasSelection());
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString());
            ui->lineEdit_inn->setText(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString());
            if (ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString() == "Да")
            {
                ui->checkBox_stroy->setChecked(true);
            }
            else
            {
                ui->checkBox_stroy->setChecked(false);
            }
        }
        else
        {
            ui->lineEdit_name->setText("");
            ui->lineEdit_inn->setText("");
            ui->checkBox_stroy->setChecked(false);
        }
        if (ui->lineEdit_name->text() != "" and (ui->lineEdit_inn->text().length() == 12 or ui->lineEdit_inn->text().length() == 10) and ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_add_firm->setEnabled(true);
        }
        else
        {
            ui->pushButton_add_firm->setEnabled(false);
        }
        if (ui->pushButton_add_firm->isEnabled())
        {
            ui->pushButton_add_firm->setText("Изменить");
        }
        else
        {
            ui->pushButton_add_firm->setText("Добавить");
        }
    }
    else
    {
        ui->lineEdit_name->setText("");
        ui->lineEdit_inn->setText("");
        ui->checkBox_stroy->setChecked(false);
        ui->pushButton_add_firm->setEnabled(false);
        ui->pushButton_del_firm->setEnabled(false);
    }
}

//Сортировка по столбцу
void class_ref_firm::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    refresh_tableview();
}

//Обновляем форму
void class_ref_firm::refresh_tableview()
{
    query_str = "SELECT firms.id, firms.name, firms.inn, yn.data FROM firms "
                "LEFT JOIN yes_no yn ON yn.id = firms.stroy "
                "WHERE firms.id > 0 ";
    if (id_column == 0) query_str += "ORDER BY firms.name ";
    if (id_column == 1) query_str += "ORDER BY firms.name ";
    if (id_column == 2) query_str += "ORDER BY firms.inn ";
    if (id_column == 3) query_str += "ORDER BY yn.data ";
    if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    {
        query_str += " DESC";
    }
    else
    {
        query_str += " ASC";
    }
    model->setQuery(query_str);
    model->setHeaderData(1,Qt::Horizontal, "Название");
    model->setHeaderData(2,Qt::Horizontal, "ИНН");
    model->setHeaderData(3,Qt::Horizontal, "Строй");
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnWidth(3, 100);
}

//Удаляем фирму
void class_ref_firm::slot_del_firm()
{
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                        tr("Удаление фирмы удалит все связанные расчётные счета, платёжные поручения и операции над клиентами, связанными с этой организацией.\nВы уверены что хотите удалить?"),
                                        QMessageBox::Yes
                                        | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        switch (ret) {
           case QMessageBox::Yes:
            {
            query->prepare("DELETE FROM firms WHERE id = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
            query->exec();
            refresh_tableview();
            return;
            }
        }
}

void class_ref_firm::slot_show_add_rs()
{
    int firm_id = ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt();
    {
        form_rss = new QWidget;
        form_rss->setMaximumSize(500,300);
        form_rss->setMinimumSize(500,300);
        tableView = new QTableView(form_rss);
        form_rss->setWindowModality(Qt::ApplicationModal);
        form_rss->show();
    }
}

class_ref_firm::~class_ref_firm()
{
    delete this;
}
