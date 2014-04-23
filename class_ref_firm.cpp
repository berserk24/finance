#include "class_ref_firm.h"
#include "ui_class_ref_firm.h"

class_ref_firm::class_ref_firm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_ref_firm)
{
    ui->setupUi(this);

    query = new QSqlQuery;

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
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    if (ui->tableView->selectionModel()->hasSelection())
    {
        query->prepare("UPDATE firm SET name = ?, inn = ?, stroy = ? WHERE id = ?");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_inn->text());
        if (ui->checkBox_stroy->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    }
    else
    {
        query->prepare("INSERT INTO firm (name, inn, stroy)"
                            "VALUES (?, ?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_inn->text());
        if (ui->checkBox_stroy->isChecked())
        {
            query->addBindValue("true");
        }
        else
        {
            query->addBindValue("false");
        }
    }
    query->exec();
    query->clear();
    query->exec("COMMIT");
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

//Обновляем форму
void class_ref_firm::refresh_tableview()
{
    model->setQuery("SELECT firm.id, firm.name, firm.inn, yn.name FROM firm "
                    "LEFT JOIN yes_no yn ON yn.id = firm.stroy "
                    "WHERE firm.id > 0");
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
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->prepare("SELECT id FROM rss WHERE firm = ?");
    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
    query->exec();
    if (query->first())
    {
        QMessageBox::critical(this, "Ошибка", "Имеются расчётные счета привязанные к этой фирме.");
    }
    else
    {
        query->prepare("DELETE FROM firm WHERE id = ?");
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
        query->exec();
        refresh_tableview();
    }
    query->exec("COMMIT");
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
