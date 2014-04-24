#include "class_ref_pp.h"
#include "ui_class_ref_pp.h"

class_ref_pp::class_ref_pp(QWidget *parent, int u_id) :
    QWidget(parent),
    ui(new Ui::class_ref_pp)
{
    ui->setupUi(this);

    client_id = u_id;
    slot_get_access();
    ui->pushButton_add_client->setEnabled(access);

    ui->dateEdit_date_po->setDate(QDate::currentDate());
    ui->dateEdit_date_s->setDate(QDate::currentDate().addDays(-1));

    /*{
        ui->comboBox_rs->setMinimumHeight(27);
        ui->comboBox_type->setMinimumHeight(27);
        ui->comboBox_type_doc->setMinimumHeight(27);
        ui->lineEdit_firm->setMinimumHeight(27);
        ui->comboBox_client->setMinimumHeight(27);
        ui->lineEdit_margin->setMaximumHeight(27);
    }*/

    QValidator *validator2 = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9],[0-9][0-9]|[0-9][0-9],[0-9][0-9]|[0-9][0-9],[0-9]"), this);
    ui->lineEdit_margin->setValidator(validator2);

    ucb = new update_client_balans;

    id_column = 0;

    model = new QSqlQueryModel;
    ui->tableView->setModel(model);

    load_rss();
    slot_select_client();
    slot_select_client_filter();
    slot_select_pp();
    slot_set_margin();

    QHeaderView *pH = ui->tableView->horizontalHeader();

    //Фильтр
    connect(ui->comboBox_rs, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->dateEdit_date_s, SIGNAL(dateChanged(QDate)), SLOT(slot_select_pp()));
    connect(ui->dateEdit_date_po, SIGNAL(dateChanged(QDate)), SLOT(slot_select_pp()));
    connect(ui->comboBox_type, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->comboBox_type_doc, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->lineEdit_firm, SIGNAL(textChanged(QString)), SLOT(slot_select_pp()));
    connect(ui->comboBox_client_filter, SIGNAL(activated(int)), SLOT(slot_select_pp()));

    //Включаем выключаем кнопки
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_enable_button()));

    //Присваеваем платёжку клиенту
    connect(ui->pushButton_to_client, SIGNAL(clicked()), SLOT(slot_pp_to_client()));

    //Заполняем поле с коммисией
    connect(ui->comboBox_client, SIGNAL(currentIndexChanged(int)), SLOT(slot_set_margin()));
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_set_margin()));

    //Удаление ПП
    connect(ui->pushButton_del, SIGNAL(clicked()), SLOT(slot_cancel_pp()));

    //Печатаем платёжку
    connect(ui->comboBox_print_save, SIGNAL(activated(QString)), SLOT(slot_print_pp(QString)));

    //Сортировка
    connect(pH, SIGNAL(sectionClicked(int)), this, SLOT(slot_sort_pp(int)));

    connect(ui->pushButton_add_client, SIGNAL(clicked()), SLOT(slot_send_show_ref_client()));

    connect(ui->comboBox_register, SIGNAL(activated(QString)), SLOT(slot_show_print_registr(QString)));
}

//Заполняем форму настроек отображения колонок
void class_ref_pp::slot_write_settings_view()
{
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT value FROM settings WHERE name = 'pp_collumn'");
    query->first();
    QString str = query->value(0).toString();
    for(int i = 0; i < 10; i++)
    {
        settings_table->slot_set_checkbox(model->headerData(i+1, Qt::Horizontal).toString(), str_to_bool(str.mid(i,1)));
    }
    query->clear();
    query->exec("COMMIT");
}

bool class_ref_pp::str_to_bool(QString str)
{
    if (str == "1")
    {
        return true;
    }
    else
    {
        return false;
    }
}

//
void class_ref_pp::slot_set_settings_header(QList<bool> list)
{
    QString str = "";
    for (int i = 0; i < list.size(); i++)
    {
        if(list.at(i))
        {
            str += "1";
        }
        else
        {
            str += "0";
        }
    }
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT 'value' FROM settings WHERE name = 'pp_collumn'");
    query->first();

    if (query->value(0).toString() == "")
    {
        query->clear();
        query->prepare("INSERT INTO settings ('value', name) VALUES (?, ?)");
    }
    else
    {
        query->clear();
        query->prepare("UPDATE settings SET 'value' = ? WHERE name = ?");
    }
    query->addBindValue(str);
    query->addBindValue("pp_collumn");
    query->exec();
    query->clear();
    query->exec("COMMIT");
    slot_select_pp();
}

//Показываем настройки
void class_ref_pp::slot_show_settings_table()
{
    settings_table = new class_setings_table;
    slot_write_settings_view();
    connect(settings_table, SIGNAL(signal_set_collumn_state(QList<bool>)), SLOT(slot_set_settings_header(QList<bool>)));
}

//Сортировка по столбцу
void class_ref_pp::slot_sort_pp(int sort_id)
{
    id_column = sort_id;
    slot_select_pp();
}

//Проверяем права доступа
void class_ref_pp::slot_get_access()
{
    query = new QSqlQuery;
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->prepare("SELECT work_pp FROM users_access WHERE id = ?");
    query->addBindValue(client_id);
    query->exec();
    query->first();
    access = query->value(0).toBool();
    query->clear();
    query->exec("COMMIT");
}

//Заполняем выпадайку расчётных счетов
void class_ref_pp::load_rss()
{
    int id = 0;
    QString str;
    str = ui->comboBox_rs->currentText();
    if (ui->comboBox_rs->currentText() != "")
        id = ui->comboBox_rs->currentIndex();
    ui->comboBox_rs->clear();
    ui->comboBox_rs->addItem("Все", 0);
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT id, name FROM rss ORDER BY id");
    query->first();
    ui->comboBox_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    while(query->next())
    {
        ui->comboBox_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    }
    query->clear();
    query->exec("COMMIT");
    ui->comboBox_rs->setCurrentIndex(id);
}

//Показываем платёжки
void class_ref_pp::slot_select_pp()
{
    QString query_str = "SELECT pp.id, strftime('%d.%m.%Y', date(julianday(pp.date_oper))), CAST(pp.sum AS TEXT), pp_in_out.name, IFNULL(pf.name, IFNULL(pp.payer1 , pp.payer)) AS payerr, IFNULL(rf.name, IFNULL(pp.receiver1, pp.receiver)) AS receiverr, client.name cn, pp.dest_pay, pp.num, type.name, rss.name, client.id, firm.stroy "
            "FROM pp "
            "LEFT JOIN rss ON pp.rs_id = rss.id "
            "LEFT JOIN pp_in_out ON pp.type = pp_in_out.id "
            "LEFT JOIN pp_to_client ptc ON pp.id = ptc.pp_id "
            "LEFT JOIN client ON ptc.client_id = client.id "
            "LEFT JOIN pp_type type ON pp.type_doc = type.id "
            "LEFT JOIN firm pf ON pf.inn = pp.payer_inn "
            "LEFT JOIN firm rf ON rf.inn = pp.receiver_inn "
            "LEFT JOIN firm ON firm.id = rss.firm "
            "WHERE pp.date_oper >= " + QString::number(ui->dateEdit_date_s->date().toJulianDay())
            + " AND pp.date_oper <= " + QString::number(ui->dateEdit_date_po->date().toJulianDay())
            + " AND pp.type_doc < '100' ";
            //+ " AND pp.type_doc = '" + QString::number(ui->comboBox_type_doc->currentIndex()) + "'";
    switch (ui->comboBox_type_doc->currentIndex())
    {
        case 0:
            query_str += " AND pp.type_doc = '0'";
            break;
        case 1:
            query_str += " AND (pp.type_doc == '1' OR pp.type_doc == '2')";
            break;
    }
    switch (ui->comboBox_client_filter->currentIndex())
    {
        case 0:
            break;
        case 1:
            query_str += " AND cn IS NULL";
            break;
        case 2:
            query_str += " AND cn IS NOT NULL";
            break;
        default:
            query_str += " AND cn = '" + ui->comboBox_client_filter->currentText() + "'";
            break;
    }
    if (ui->lineEdit_firm->text() != "")
            query_str += " AND (payerr LIKE '%" + ui->lineEdit_firm->text() + "%' OR receiverr LIKE '%" + ui->lineEdit_firm->text() + "%')";
    if (ui->comboBox_type->currentIndex() > 0) query_str = query_str + " and pp.type = " + QString::number(ui->comboBox_type->currentIndex());
    if (ui->comboBox_rs->currentIndex() > 0) query_str = query_str + " and pp.rs_id = " + ui->comboBox_rs->itemData(ui->comboBox_rs->currentIndex()).toString();
    if (id_column == 0) query_str += " ORDER BY pp.date_oper";
    if (id_column == 1) query_str += " ORDER BY pp.date_oper";
    if (id_column == 2) query_str += " ORDER BY pp.sum";
    if (id_column == 3) query_str += " ORDER BY pp_in_out.name";
    if (id_column == 4) query_str += " ORDER BY pp.payer1";
    if (id_column == 5) query_str += " ORDER BY pp.receiver1";
    if (id_column == 6) query_str += " ORDER BY client.name";
    if (id_column == 7) query_str += " ORDER BY pp.dest_pay";
    if (id_column == 8) query_str += " ORDER BY pp.num";
    if (id_column == 9) query_str += " ORDER BY pp.type_doc";
    if (id_column == 10) query_str += " ORDER BY rss.name";
    //qDebug() << query_str << endl;
    model->setQuery(query_str);
    ui->tableView->setModel(model);

    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query->exec("SELECT value FROM settings WHERE name = 'pp_collumn'");
    query->first();
    QString str = query->value(0).toString();
    query->clear();
    query->exec("COMMIT");

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(1, !str_to_bool(str.mid(0,1)));
    ui->tableView->setColumnHidden(2, !str_to_bool(str.mid(1,1)));
    ui->tableView->setColumnHidden(3, !str_to_bool(str.mid(2,1)));
    ui->tableView->setColumnHidden(4, !str_to_bool(str.mid(3,1)));
    ui->tableView->setColumnHidden(5, !str_to_bool(str.mid(4,1)));
    ui->tableView->setColumnHidden(6, !str_to_bool(str.mid(5,1)));
    ui->tableView->setColumnHidden(7, !str_to_bool(str.mid(6,1)));
    ui->tableView->setColumnHidden(8, !str_to_bool(str.mid(7,1)));
    ui->tableView->setColumnHidden(9, !str_to_bool(str.mid(8,1)));
    ui->tableView->setColumnHidden(10, !str_to_bool(str.mid(9,1)));
    model->setHeaderData(1,Qt::Horizontal, "Дата\nдокумента");
    model->setHeaderData(2,Qt::Horizontal, "Сумма");
    model->setHeaderData(3,Qt::Horizontal, "Движение");
    model->setHeaderData(4,Qt::Horizontal, "Плательщик");
    model->setHeaderData(5,Qt::Horizontal, "Получатель");
    model->setHeaderData(6,Qt::Horizontal, "Клиент");
    model->setHeaderData(7,Qt::Horizontal, "Назначение");
    model->setHeaderData(8,Qt::Horizontal, "Номер\nдокумента");
    model->setHeaderData(9,Qt::Horizontal, "Тип документа");
    model->setHeaderData(10,Qt::Horizontal, "Расчётный счёт");
    ui->tableView->setColumnHidden(11, true);
    ui->tableView->setColumnHidden(12, true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->setColumnWidth(3, 100);
    ui->tableView->setColumnWidth(4, 170);
    ui->tableView->setColumnWidth(5, 170);
    ui->tableView->setColumnWidth(6, 100);
    ui->tableView->setColumnWidth(7, 300);
    ui->tableView->setColumnWidth(8, 100);
    ui->tableView->setColumnWidth(9, 120);
    ui->tableView->setColumnWidth(10, 250);
    show();
    ui->tableView->resizeRowsToContents();
    ui->tableView->setAlternatingRowColors(true);
}

//Включаем выключаем кнопки
void class_ref_pp::slot_enable_button()
{
    //qDebug() << ui->tableView->selectionModel()->selectedRows() << endl;
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 13)
    {
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_del->setEnabled(true);
            ui->comboBox_print_save->setEnabled(true);
            ui->pushButton_to_client->setEnabled(access);
        }
        else
        {
            //ui->pushButton_del->setEnabled(false);
            ui->comboBox_print_save->setEnabled(false);
            //ui->pushButton_to_client->setEnabled(false);
        }
    }
    else if (ui->tableView->selectionModel()->selectedIndexes().size() > 12)
    {
        ui->pushButton_del->setEnabled(false);
        ui->comboBox_print_save->setEnabled(false);
        ui->pushButton_to_client->setEnabled(access);
    }
    else
    {
        ui->pushButton_del->setEnabled(false);
        ui->comboBox_print_save->setEnabled(false);
        ui->pushButton_to_client->setEnabled(false);
    }
}

//Заполняем комбобокс в фильтре клиентами
void class_ref_pp::slot_select_client_filter()
{
    QSqlQuery *query_cl;
    int id = 0;
    QString str;
    str = ui->comboBox_client_filter->currentText();
    query_cl = new QSqlQuery;
    if (ui->comboBox_client_filter->currentText() != "")
        id = ui->comboBox_client_filter->currentIndex();
    ui->comboBox_client_filter->clear();
    ui->comboBox_client_filter->addItem("Все");
    ui->comboBox_client_filter->addItem("Незаполнено");
    ui->comboBox_client_filter->addItem("Заполнено");
    query_cl->exec("BEGIN IMMEDIATE TRANSACTION");
    query_cl->exec("SELECT id, name FROM client");
    query_cl->first();
    ui->comboBox_client_filter->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    while (query_cl->next())
    {
        ui->comboBox_client_filter->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    }
    query_cl->clear();
    query_cl->exec("COMMIT");
    ui->comboBox_client_filter->setCurrentIndex(id);
    delete query_cl;
}

//Заполняем комбобокс клиентами
void class_ref_pp::slot_select_client()
{
    QSqlQuery *query_cl;
    int id = 0;
    QString str;
    str = ui->comboBox_client->currentText();
    if (ui->comboBox_client->currentText() != "")
        id = ui->comboBox_client->currentIndex();
    query_cl = new QSqlQuery;
    ui->comboBox_client->clear();
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    query_cl->exec("SELECT id, name FROM client");
    query_cl->first();
    ui->comboBox_client->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    while (query_cl->next())
    {
        ui->comboBox_client->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    }
    query_cl->clear();
    query_cl->exec("COMMIT");
    ui->comboBox_client->setCurrentIndex(id);
    delete query_cl;
}

//Присваеваем платёжку клиенту
void class_ref_pp::slot_pp_to_client()
{
    for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
    {
        if (ui->tableView->selectionModel()->selectedRows(6).at(i).data().toString() != "")
        {
            int rez = QMessageBox::warning(this, tr("Внимание"),
                                            tr("Платёжное поручение привязано к контрагенту.\nОтменить привязку к контрагенту?"),
                                            QMessageBox::Yes
                                            | QMessageBox::Cancel,
                                            QMessageBox::Yes);
            switch (rez) {
                case QMessageBox::Yes:
                query->exec("BEGIN IMMEDIATE TRANSACTION");
                query->prepare("SELECT id, margin FROM client_operations WHERE id_pp = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedRows(0).at(i).data().toString());
                query->exec();
                query->first();
                ucb->slot_cancel_pay(ui->tableView->selectionModel()->selectedRows(11).at(i).data().toString(),
                                     ui->tableView->selectionModel()->selectedRows(0).at(i).data().toString(),
                                     ui->tableView->selectionModel()->selectedRows(3).at(i).data().toString(),
                                     ui->tableView->selectionModel()->selectedRows(2).at(i).data().toString(),
                                     query->value(1).toString(),
                                     query->value(0).toString(),
                                     ""
                            );
                query->clear();

                query->prepare("SELECT pio.name, pp.sum FROM pp LEFT JOIN pp_in_out pio ON pp.type = pio.id  WHERE pp.id = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedRows().at(i).data().toString());
                query->exec();
                query->first();
                //qDebug() << ui->tableView->selectionModel()->selectedRows().at(i).data().toString() << query->value(0).toString() << query->value(1).toString() << endl;
                ucb->slot_update_balans(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString(),
                                query->value(0).toString(),
                                query->value(1).toString(),
                                ui->tableView->selectionModel()->selectedRows().at(i).data().toString(),
                                false,
                                "",
                                false,
                                ui->lineEdit_margin->text(),
                                ""
                                );
                query->clear();
                query->exec("COMMIT");
                break;
           case QMessageBox::Cancel:
               return;
               break;
            }
        }
        else
        {
            query->exec("BEGIN IMMEDIATE TRANSACTION");
            query->prepare("SELECT pio.name, pp.sum FROM pp LEFT JOIN pp_in_out pio ON pp.type = pio.id  WHERE pp.id = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedRows().at(i).data().toString());
            query->exec();
            query->first();
            ucb->slot_update_balans(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString(),
                            query->value(0).toString(),
                            query->value(1).toString(),
                            ui->tableView->selectionModel()->selectedRows().at(i).data().toString(),
                            false,
                            "",
                            false,
                            ui->lineEdit_margin->text(),
                            ""
                            );
            query->clear();
            query->exec("COMMIT");
        }
    }
    slot_clear_form();
    slot_select_pp();
}

//Очищаем форму
void class_ref_pp::slot_clear_form()
{
    ui->tableView->clearSelection();
    slot_set_margin();
}

//Заполняем поле с коммисией
void class_ref_pp::slot_set_margin()
{
    if (ui->tableView->selectionModel()->hasSelection())
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        query->prepare("SELECT t_trans_in, t_trans_in_s, t_trans_out FROM client WHERE id = ?");
        query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString());
        query->exec();
        query->first();
        if (ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString() == "Расход")
        {

            ui->lineEdit_margin->setText(query->value(2).toString());
        }
        else
        {
            if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "true")
            {
                ui->lineEdit_margin->setText(query->value(1).toString());
            }
            if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "false")
            {
                ui->lineEdit_margin->setText(query->value(0).toString());
            }
        }
        query->clear();
        query->exec("COMMIT");
    }
    else
    {
        ui->lineEdit_margin->setText("");
    }
}

//Отменяем платёжку
void class_ref_pp::slot_cancel_pp()
{
    QString rs_id, type;
    double sum;
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Удаление платежей необходимо только при отзыве платежа.\nВы уверены что хотите удалить платёжное поручение?"),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    switch (ret) {
       case QMessageBox::Yes:
            if (ui->tableView->selectionModel()->selectedIndexes().at(6).data().toString() != "")
            {
                int rez = QMessageBox::warning(this, tr("Внимание"),
                                                tr("Платёжное поручение привязано к контрагенту.\nОтменить привязку к контрагенту?"),
                                                QMessageBox::Yes
                                                | QMessageBox::Cancel,
                                                QMessageBox::Yes);
                switch (rez) {
                    case QMessageBox::Yes:
                    query->exec("BEGIN IMMEDIATE TRANSACTION");
                    query->prepare("SELECT id, margin FROM client_operations WHERE id_pp = ?");
                    query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
                    query->exec();
                    query->first();
                    ucb->slot_cancel_pay(ui->tableView->selectionModel()->selectedIndexes().at(11).data().toString(),
                                         ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString(),
                                         ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString(),
                                         ui->tableView->selectionModel()->selectedIndexes().at(2).data().toString(),
                                         query->value(1).toString(),
                                         query->value(0).toString(),
                                         ""
                                );
                    query->clear();
                    query->exec("COMMIT");
                    break;
               case QMessageBox::Cancel:
                   return;
                   break;
                }
            }

            query->exec("BEGIN IMMEDIATE TRANSACTION");
            query->exec("SELECT rs_id, type FROM pp WHERE id = '" + ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString() + "'");
            query->first();
            rs_id = query->value(0).toString();
            type = query->value(1).toString();
            query->clear();

            query->prepare("SELECT balans FROM rss_balans WHERE id = ?");
            query->addBindValue(rs_id);
            query->exec();
            query->first();
            sum = query->value(0).toDouble();
            query->clear();

            //qDebug() << sum << endl;

            query->prepare("UPDATE rss_balans SET balans = ? WHERE id = ?");
            if (type == "1")
            {
                query->addBindValue(QString::number(sum + ui->tableView->selectionModel()->selectedIndexes().at(2).data().toDouble(), 'f', 2));
            }
            if (type == "2")
            {
                query->addBindValue(QString::number(sum - ui->tableView->selectionModel()->selectedIndexes().at(2).data().toDouble(), 'f', 2));
            }
            query->addBindValue(rs_id);
            query->exec();

            if (!query->lastError().isValid())
            {
                query->first();
                query->clear();
                query->prepare("DELETE FROM pp WHERE id = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(0).data().toString());
                query->exec();
                query->first();
                query->clear();
            }
            query->exec("COMMIT");
            slot_select_pp();
            break;
       case QMessageBox::Cancel:
            query->exec("COMMIT");
           return;
           break;
     }
    query->exec("COMMIT");
}

void class_ref_pp::slot_print_pp(QString str)
{
    print_pp = new class_print_pp(this, ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt(), str);
}

void class_ref_pp::slot_send_show_ref_client()
{
    emit this->show_ref_client();
}

void class_ref_pp::slot_show_print_registr(QString str)
{
    printer = new QPrinter;
    //printer->setResolution(QPrinter::HighResolution);
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    printer->setFullPage(true);

    if (str == "Реестр печатать")
    {
        preview = new QPrintPreviewDialog(printer);
        preview->setWindowFlags ( Qt::Window );

        connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slot_print_preview(QPrinter*)));

        preview->show();
    }
    else
    {
        slot_save_register();
    }
}

void class_ref_pp::slot_save_register()
{
    open_dialog = new QFileDialog;
    QString str_file = open_dialog->getSaveFileName(this, "Сохранить как", "", "CSV файл (*.csv)");
    if (str_file.right(4) != ".csv")
        str_file += ".csv";
    file = new QFile(str_file);
    delete open_dialog;
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
             return;
    QTextStream out(file);
    QList<int> list;
    int num_summ;
    for (int i = 0; i < ui->tableView->horizontalHeader()->count(); i++)
    {
        if (!ui->tableView->horizontalHeader()->isSectionHidden(i))
        {
            list.append(i);
            out << model->headerData(i,Qt::Horizontal).toString().simplified() + ";";
            if (model->headerData(i,Qt::Horizontal).toString().simplified() == "Сумма") num_summ = i;
        }
    }
    out << endl;
    for (int i = 0; i < ui->tableView->model()->rowCount(); i++)
    {
        for (int j = 0; j < list.count(); j++)
        {
            if (j == num_summ)
            {
                out << ui->tableView->model()->data(ui->tableView->model()->index(i, list.at(j), QModelIndex()), Qt::DisplayRole).toString().simplified().replace(".", ",") + ";";
            }
            else
            {
                out << ui->tableView->model()->data(ui->tableView->model()->index(i, list.at(j), QModelIndex()), Qt::DisplayRole).toString().simplified() + ";";
            }
        }
        out << endl;
    }
    file->close();
    delete file;
}

void class_ref_pp::slot_print_preview(QPrinter *printer)
{
    QString strStream;
    QTextStream out(&strStream);

    const int rowCount = ui->tableView->model()->rowCount();
    const int columnCount = ui->tableView->model()->columnCount();

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n")
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++)
        if (!ui->tableView->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (!ui->tableView->isColumnHidden(column)) {
                QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
        "</body>\n"
        "</html>\n";

    QTextDocument *document = new QTextDocument();
    document->setHtml(strStream);

    document->print(printer);

    delete document;

}


class_ref_pp::~class_ref_pp()
{
    delete ui;
}