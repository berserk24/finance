#include "class_rss_form.h"
#include "ui_class_rss_form.h"

class_rss_form::class_rss_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_rss_form)
{
    ui->setupUi(this);

    query = new QSqlQuery;

    QValidator *validator;
    validator = new QRegExpValidator(QRegExp("[1-9]\\d{19}"), this);
    ui->lineEdit_number->setValidator(validator);

    validator = new QRegExpValidator(QRegExp("[0-9]\\d{8}"), this);
    ui->lineEdit_bik->setValidator(validator);

    validator = new QRegExpValidator(QRegExp("[1-9]\\d+,[0-9][0-9]"), this);
    ui->lineEdit_start_balans->setValidator(validator);


    model = new QSqlQueryModel;
    select_table();
    model->setHeaderData(1,Qt::Horizontal, "Название");
    model->setHeaderData(2,Qt::Horizontal, "БИК банка");
    model->setHeaderData(3,Qt::Horizontal, "Номер счёта");
    model->setHeaderData(4,Qt::Horizontal, "Организация");
    model->setHeaderData(6,Qt::Horizontal, "Начальный баланс");
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnHidden(5,true);
    ui->tableView->setColumnWidth(6, 150);


    firm_query = new QSqlQuery;

    select_firm();

    connect(ui->pushButton_add_edit, SIGNAL(clicked()),SLOT(slot_add_rs()));

    connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));
    connect(ui->lineEdit_number, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));
    connect(ui->lineEdit_bik, SIGNAL(textChanged(QString)), SLOT(slot_enable_add()));

    //Включаем выключеам кнопку удалить
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_del()));

    //Удаляем РС
    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_del_rs()));

}

//Добавляем или изменяем РС
void class_rss_form::slot_add_rs()
{
    QString id;
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    if (ui->pushButton_add_edit->text() == "Добавить")
    {
        query->prepare("INSERT INTO rss (name, bik, number, firm)"
                            "VALUES (?, ?, ?, ?)");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_bik->text());
        query->addBindValue(ui->lineEdit_number->text());
        if (ui->comboBox->currentIndex() == 0)
        {
            query->addBindValue("0");
        }
        else
        {
            query->addBindValue(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());
        }
        query->exec();
        query->clear();
        query->exec("SELECT seq FROM sqlite_sequence WHERE name = 'rss'");
        query->first();
        id = query->value(0).toString();
        query->clear();
        query->prepare("INSERT INTO pp (rs_id, type, type_doc, date, sum, num, date_oper, payer_count, payer_inn, receiver_count, receiver_inn) "
                       "VALUES (?, '2', '100', ?, ?, 0, ?, '', '', '', '')");
        query->addBindValue(id);
        query->addBindValue(QString::number(QDate::currentDate().toJulianDay()));
        query->addBindValue(ui->lineEdit_start_balans->text());
        query->addBindValue(QString::number(QDate::currentDate().toJulianDay()));
        query->exec();
        query->clear();
    }
    if (ui->pushButton_add_edit->text() == "Изменить")
    {
        query->prepare("UPDATE rss SET name=?, bik=?, number=?, firm=?"
                            "WHERE id=?");
        query->addBindValue(ui->lineEdit_name->text());
        query->addBindValue(ui->lineEdit_bik->text());
        query->addBindValue(ui->lineEdit_number->text());
        query->addBindValue(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt());
        query->exec();
        query->clear();
        query->prepare("UPDATE pp SET sum = ? "
                       "WHERE pp.rs_id = ? AND pp.type_doc = '100'");
        query->addBindValue(ui->lineEdit_start_balans->text());
        query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt());
        query->exec();
        query->clear();
    }
    query->exec("COMMIT");
    clear_field();
    select_table();
}



//Включаем кнопку Изменить/Добавить
void class_rss_form::slot_enable_add()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 8)
    {
        if (ui->lineEdit_name->text() != "" and ui->lineEdit_bik->text().length() == 9 and ui->lineEdit_number->text().length() == 20)
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

//Включаем выключаем кнопку удалить
void class_rss_form::slot_enable_del()
{
    if (ui->tableView->selectionModel()->selectedIndexes().size() < 8)
    {
        int index = 0;
        ui->pushButton_del->setEnabled(ui->tableView->selectionModel()->hasSelection());
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->lineEdit_name->setText(ui->tableView->selectionModel()->selectedIndexes().at(1).data().toString());
            ui->lineEdit_bik->setText(ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString());
            ui->lineEdit_number->setText(ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString());
            for (index; ui->comboBox->itemData(index).toInt() != ui->tableView->selectionModel()->selectedIndexes().at(5).data().toInt();index++)
            {
            }
            ui->comboBox->setCurrentIndex(index);
            ui->lineEdit_start_balans->setText(ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString());
        }
        else
        {
            ui->lineEdit_name->setText("");
            ui->lineEdit_bik->setText("");
            ui->lineEdit_number->setText("");
            ui->lineEdit_start_balans->setText("");
            ui->comboBox->setCurrentIndex(0);
        }
        if (ui->lineEdit_name->text() != "" and ui->lineEdit_number->text().length() == 20  and ui->lineEdit_bik->text().length() == 9 and ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_add_edit->setEnabled(true);
        }
        else
        {
            ui->pushButton_add_edit->setEnabled(false);
        }
        if (ui->pushButton_add_edit->isEnabled())
        {
            ui->pushButton_add_edit->setText("Изменить");
        }
        else
        {
            ui->pushButton_add_edit->setText("Добавить");
        }
    }
    else
    {
        ui->lineEdit_name->setText("");
        ui->lineEdit_bik->setText("");
        ui->lineEdit_number->setText("");
        ui->comboBox->setCurrentIndex(0);
        ui->lineEdit_start_balans->setText("");
        ui->pushButton_add_edit->setEnabled(false);
        ui->pushButton_del->setEnabled(false);
    }
}

//Заполняем комбобокс назаниями организаций
void class_rss_form::select_firm()
{
    ui->comboBox->clear();
    firm_query->exec("BEGIN IMMEDIATE TRANSACTION");
    firm_query->exec("SELECT id, name FROM firm ORDER BY id");
    firm_query->first();
    ui->comboBox->addItem(firm_query->value(1).toString(), QVariant(firm_query->value(0).toInt()));
    while (firm_query->next())
    {
        ui->comboBox->addItem(firm_query->value(1).toString(), QVariant(firm_query->value(0).toInt()));
    }
    firm_query->exec("COMMIT");
}

//Заполняем таблицу
void class_rss_form::select_table()
{
    model->setQuery("SELECT rss.id, rss.name, rss.bik, rss.number, firm.name, firm.id, CAST(pp.sum AS TEXT) "
                    "FROM rss "
                    "LEFT JOIN firm "
                    "LEFT JOIN pp ON rss.id = pp.rs_id "
                                 "AND pp.type_doc = '100'"
                    "WHERE rss.firm = firm.id");
    ui->tableView->setModel(model);
    ui->tableView->show();
}

//Очищаем все поля
void class_rss_form::clear_field()
{
    ui->lineEdit_name->setText("");
    ui->lineEdit_bik->setText("");
    ui->lineEdit_number->setText("");
    ui->lineEdit_start_balans->setText("");
    ui->comboBox->setCurrentIndex(0);
}

//Удаляем РС
void class_rss_form::slot_del_rs()
{
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                        tr("Удаление платежей необходимо только при отзыве платежа.\nВы уверены что хотите удалить платёжное поручение?"),
                                        QMessageBox::Yes
                                        | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        switch (ret) {
           case QMessageBox::Yes:
                query->exec("BEGIN IMMEDIATE TRANSACTION");
                query->prepare("DELETE FROM rss WHERE id=?");
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt());
                query->exec();
                query->clear();
                query->prepare("DELETE FROM rss_balans WHERE id=?");
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt());
                query->exec();
                query->clear();
                query->exec("COMMIT");
                clear_field();
                select_table();
                break;
            case QMessageBox::Cancel:
                return;
                break;
        }
}

class_rss_form::~class_rss_form()
{
    delete this;
}
