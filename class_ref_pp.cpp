#include "class_ref_pp.h"
#include "ui_class_ref_pp.h"

class_ref_pp::class_ref_pp(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_ref_pp)
{
    ui->setupUi(this);

    db = db1;
    settings = new QSettings(QSettings::UserScope, "finance", "finance", this);

    //client_id = u_id;
    slot_get_access();
    //ui->pushButton_add_client->setEnabled(access);

    ui->dateEdit_date_po->setDate(QDate::currentDate());
    ui->dateEdit_date_s->setDate(QDate::currentDate().addDays(-1));

    QValidator *validator2 = new QRegExpValidator(QRegExp("[0-9][0-9]|[0-9],[0-9][0-9]|[0-9][0-9],[0-9][0-9]|[0-9][0-9],[0-9]"), this);
    ui->lineEdit_margin->setValidator(validator2);

    ucb = new update_client_balans(0, db);

    id_column = 0;

    model = new QSqlQueryModel;
    ui->tableView->setModel(model);

    load_rss();
    slot_select_client();
    slot_select_client_filter();
    slot_select_pp();
    slot_set_margin();

    pH = ui->tableView->horizontalHeader();

    //Фильтр
    connect(ui->comboBox_rs, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->dateEdit_date_s, SIGNAL(dateChanged(QDate)), SLOT(slot_select_pp()));
    connect(ui->dateEdit_date_po, SIGNAL(dateChanged(QDate)), SLOT(slot_select_pp()));
    connect(ui->comboBox_type, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->comboBox_type_doc, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));
    connect(ui->lineEdit_firm, SIGNAL(textChanged(QString)), SLOT(slot_select_pp()));
    connect(ui->comboBox_client_filter, SIGNAL(activated(int)), SLOT(slot_select_pp()));
    connect(ui->comboBox_status, SIGNAL(currentIndexChanged(int)), SLOT(slot_select_pp()));

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

    //Открываем справочник клиентов
    connect(ui->pushButton_add_client, SIGNAL(clicked()), SLOT(slot_send_show_ref_client()));

    //Печатаем реестр
    connect(ui->comboBox_register, SIGNAL(activated(QString)), SLOT(slot_show_print_registr(QString)));

    //Считаем сумму выделенных строк
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slot_sum_balans_rs()));
}

//Считаем сумму выделенных строк
void class_ref_pp::slot_sum_balans_rs()
{
    double sum = 0;
    if (ui->tableView->selectionModel()->hasSelection())
        for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
        {
            sum += ui->tableView->selectionModel()->selectedRows(2).at(i).data().toDouble();
        }
    emit signal_send_sum_pp("Сумма по выделенным строкам: " + QString::number(sum, 'f', 2));
}

//Заполняем форму настроек отображения колонок
void class_ref_pp::slot_write_settings_view()
{
    QString str = get_settings();
    for(int i = 0; i < 10; i++)
    {
        settings_table->slot_set_checkbox(model->headerData(i+1, Qt::Horizontal).toString(), str_to_bool(str.mid(i,1)));
    }
    settings_table->slot_set_checkbox(model->headerData(16, Qt::Horizontal).toString(), str_to_bool(str.mid(10,1)));
}

//Преобразовываем строку в bool
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

//Записываем настройки отображения таблицы
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
    settings->beginGroup("table_view");
    settings->setValue("pp_table", str);
    settings->endGroup();
    settings->sync();
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
    query->prepare("SELECT work_pp FROM users_access WHERE id = ?");
    query->addBindValue(client_id);
    query->exec();
    query->first();
    access = query->value(0).toBool();
    query->clear();
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
    query->exec("SELECT id, name FROM rss ORDER BY id");
    query->first();
    ui->comboBox_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    while(query->next())
    {
        ui->comboBox_rs->addItem(query->value(1).toString(), QVariant(query->value(0).toInt()));
    }
    query->clear();
    ui->comboBox_rs->setCurrentIndex(id);
}

//Получаем настройки
QString class_ref_pp::get_settings()
{
    settings->beginGroup("table_view");
    QString str = settings->value("pp_table", "11111111111").toString();
    settings->endGroup();
    return str;
}

//Показываем платёжки
void class_ref_pp::slot_select_pp()
{
    QString query_str = "SELECT pp.id, pp.date_oper, CAST(pp.sum_pp AS VARCHAR(18)), pp_in_out.data, COALESCE(pf.name, pp.payer1 , pp.payer) AS payerr, COALESCE(rf.name, pp.receiver1, pp.receiver) AS receiverr, clients.name, pp.dest_pay, pp.num, pp_type.data, rss.name, clients.id, firms.stroy, pp.payer_inn, pp.receiver_inn, pp.rs_id, status_pp.data "
            "FROM pp "
            "LEFT JOIN rss ON pp.rs_id = rss.id "
            "LEFT JOIN pp_in_out ON pp.type_pp = pp_in_out.id "
            "LEFT JOIN pp_to_client ptc ON pp.id = ptc.pp_id "
            "LEFT JOIN clients ON ptc.client_id = clients.id "
            "LEFT JOIN pp_type ON pp.type_doc = pp_type.id "
            "LEFT JOIN firms pf ON pf.inn = pp.payer_inn "
            "LEFT JOIN firms rf ON rf.inn = pp.receiver_inn "
            "LEFT JOIN firms ON firms.id = rss.firm "
            "LEFT JOIN status_pp ON status_pp.id = pp.status_pp "
            "WHERE pp.date_oper >= '" + ui->dateEdit_date_s->date().toString("dd.MM.yyyy")
            + "' AND pp.date_oper <= '" + ui->dateEdit_date_po->date().toString("dd.MM.yyyy") + "'";
    switch (ui->comboBox_type_doc->currentIndex())
    {
        case 0:
            query_str += " AND pp.type_doc = '0'";
            break;
        case 1:
            query_str += " AND (pp.type_doc = '1' OR pp.type_doc = '2')";
            break;
    }
    switch (ui->comboBox_client_filter->currentIndex())
    {
        case 0:
            break;
        case 1:
            query_str += " AND clients.name IS NULL";
            break;
        case 2:
            query_str += " AND clients.name IS NOT NULL";
            break;
        default:
            query_str += " AND clients.name = '" + ui->comboBox_client_filter->currentText() + "'";
            break;
    }
    if (ui->lineEdit_firm->text() != "")
            query_str += " AND (LOWER(COALESCE(pf.name, pp.payer1 , pp.payer)) LIKE '%" + ui->lineEdit_firm->text().toLower() + "%' OR LOWER(COALESCE(rf.name, pp.receiver1, pp.receiver)) LIKE '%" + ui->lineEdit_firm->text().toLower() + "%')";

    if (ui->comboBox_type->currentIndex() > 0) query_str = query_str + " and pp.type_pp = " + QString::number(ui->comboBox_type->currentIndex());
    if (ui->comboBox_rs->currentIndex() > 0) query_str = query_str + " and pp.rs_id = " + ui->comboBox_rs->itemData(ui->comboBox_rs->currentIndex()).toString();
    if (ui->comboBox_status->currentIndex() > 0) query_str = query_str + " and status_pp.data = '" + ui->comboBox_status->currentText() + "'";
    if (id_column == 0) query_str += " ORDER BY pp.date_oper";
    if (id_column == 1) query_str += " ORDER BY pp.date_oper";
    if (id_column == 2) query_str += " ORDER BY pp.sum_pp";
    if (id_column == 3) query_str += " ORDER BY pp_in_out.data";
    if (id_column == 4) query_str += " ORDER BY pp.payer1";
    if (id_column == 5) query_str += " ORDER BY pp.receiver1";
    if (id_column == 6) query_str += " ORDER BY clients.name";
    if (id_column == 7) query_str += " ORDER BY pp.dest_pay";
    if (id_column == 8) query_str += " ORDER BY pp.num";
    if (id_column == 9) query_str += " ORDER BY pp.type_doc";
    if (id_column == 10) query_str += " ORDER BY rss.name";
    if (id_column == 16) query_str += " ORDER BY status_pp.data";
    if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    {
        query_str += " DESC";
    }
    else
    {
        query_str += " ASC";
    }
    //qDebug() << query_str << endl;
    model->setQuery(query_str);
    ui->tableView->setModel(model);

    QString str = get_settings();

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
    ui->tableView->setColumnHidden(16, !str_to_bool(str.mid(10,1)));
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
    ui->tableView->setColumnHidden(13, true);
    ui->tableView->setColumnHidden(14, true);
    ui->tableView->setColumnHidden(15, true);
    model->setHeaderData(16,Qt::Horizontal, "Статус");
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
    ui->tableView->setColumnWidth(16, 100);
    show();
    ui->tableView->resizeRowsToContents();
    ui->tableView->setAlternatingRowColors(true);
}

//Включаем выключаем кнопки
void class_ref_pp::slot_enable_button()
{
    //qDebug() << ui->tableView->selectionModel()->selectedRows() << endl;
    if (ui->tableView->selectionModel()->selectedIndexes().size() == 17)
    {
        if (ui->tableView->selectionModel()->hasSelection())
        {
            ui->pushButton_del->setEnabled(true);
            ui->comboBox_print_save->setEnabled(true);
            ui->pushButton_to_client->setEnabled(true);
            query->prepare("SELECT id FROM save_actions WHERE client_id = ? AND inn = ? AND type_pp = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(11).data().toString());
            if (ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString() == "Приход")
            {
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(13).data().toString());
                query->addBindValue(2);
            }
            else
            {
                query->addBindValue(ui->tableView->selectionModel()->selectedIndexes().at(14).data().toString());
                query->addBindValue(1);
            }
            query->exec();
            query->first();
            if (query->value(0).toInt() > 0)
            {
                ui->checkBox_save_action->setEnabled(false);
                ui->checkBox_save_action->setChecked(false);
            }
            else
            {
                ui->checkBox_save_action->setEnabled(true);
            }
        }
        else
        {
            ui->comboBox_print_save->setEnabled(false);
            ui->pushButton_to_client->setEnabled(false);
            ui->checkBox_save_action->setEnabled(false);
            ui->checkBox_save_action->setChecked(false);
        }
    }
    else if (ui->tableView->selectionModel()->selectedIndexes().size() > 17)
    {
        ui->pushButton_del->setEnabled(true);
        ui->pushButton_to_client->setEnabled(true);
        ui->comboBox_print_save->setEnabled(false);
        ui->checkBox_save_action->setEnabled(false);
        ui->checkBox_save_action->setChecked(false);
    }
    else
    {
        ui->pushButton_del->setEnabled(false);
        ui->comboBox_print_save->setEnabled(false);
        ui->pushButton_to_client->setEnabled(false);
        ui->checkBox_save_action->setEnabled(false);
        ui->checkBox_save_action->setChecked(false);
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
    query_cl->exec("SELECT id, name FROM clients");
    query_cl->first();
    ui->comboBox_client_filter->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    while (query_cl->next())
    {
        ui->comboBox_client_filter->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    }
    query_cl->clear();
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
    query_cl->exec("SELECT id, name FROM clients");
    query_cl->first();
    ui->comboBox_client->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    while (query_cl->next())
    {
        ui->comboBox_client->addItem(query_cl->value(1).toString(), query_cl->value(0).toInt());
    }
    query_cl->clear();
    ui->comboBox_client->setCurrentIndex(id);
    delete query_cl;
}

//Присваеваем платёжку клиенту
void class_ref_pp::slot_pp_to_client()
{
    for (int i = 0; i < ui->tableView->selectionModel()->selectedRows().size(); i++)
    {
        QString marg;
        if (ui->lineEdit_margin->text() == "")
        {
            marg = "-1";
        }
        else
        {
            marg = ui->lineEdit_margin->text();
        }
        if (ui->tableView->selectionModel()->selectedRows(6).at(i).data().toString() != "")
        {
            int rez = QMessageBox::warning(this, tr("Внимание"),
                                            tr("Платёжное поручение привязано к контрагенту.\nОтменить привязку к контрагенту?"),
                                            QMessageBox::Yes
                                            | QMessageBox::Cancel,
                                            QMessageBox::Yes);
            switch (rez) {
                case QMessageBox::Yes:
                query->prepare("SELECT id, margin FROM clients_operations WHERE id_pp = ?");
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

                query->prepare("SELECT pio.data, pp.sum_pp FROM pp LEFT JOIN pp_in_out pio ON pp.type_pp = pio.id  WHERE pp.id = ?");
                query->addBindValue(ui->tableView->selectionModel()->selectedRows().at(i).data().toString());
                query->exec();
                query->first();
                //qDebug() << ui->tableView->selectionModel()->selectedRows().at(i).data().toString() << query->value(0).toString() << query->value(1).toString() << endl;
                ucb->slot_update_balans(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString(),
                                query->value(0).toString(),
                                query->value(1).toString(),
                                ui->tableView->selectionModel()->selectedRows().at(i).data().toString(),
                                "",
                                marg,
                                ""
                                );
                query->clear();
                break;
           case QMessageBox::Cancel:
               return;
               break;
            }
        }
        else
        {
            query->prepare("SELECT pio.data, pp.sum_pp FROM pp LEFT JOIN pp_in_out pio ON pp.type_pp = pio.id  WHERE pp.id = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedRows().at(i).data().toString());
            query->exec();
            query->first();
            ucb->slot_update_balans(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString(),
                            query->value(0).toString(),
                            query->value(1).toString(),
                            ui->tableView->selectionModel()->selectedRows().at(i).data().toString(),
                            "",
                            marg,
                            ""
                            );
            query->clear();
            if (ui->checkBox_save_action->isChecked())
            {
                //QString percent = QString::null;
                QString percent = "-1";
                query->prepare("SELECT t_trans_in, t_trans_in_s, t_trans_out FROM clients WHERE id = ?");
                query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString());
                query->exec();
                query->first();
                if (ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString() == "Расход")
                {

                    if (ui->lineEdit_margin->text() == "" or ui->lineEdit_margin->text() == query->value(2).toString())
                    {}
                    else
                    {
                        percent = ui->lineEdit_margin->text();
                    }
                }
                else
                {
                    if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "1")
                    {
                        if (ui->lineEdit_margin->text() == "" or ui->lineEdit_margin->text() == query->value(1).toString())
                        {}
                        else
                        {
                            percent = ui->lineEdit_margin->text();
                        }
                    }
                    if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "0")
                    {
                        if (ui->lineEdit_margin->text() == "" or ui->lineEdit_margin->text() == query->value(0).toString())
                        {}
                        else
                        {
                            percent = ui->lineEdit_margin->text();
                        }
                    }
                }
                query->prepare("INSERT INTO save_actions (inn, firm_name, type_pp, client_id, percent) "
                               "VALUES(?, ?, ?, ?, ?)");
                if (ui->tableView->selectionModel()->selectedRows(3).at(i).data().toString() == "Приход")
                {
                    query->addBindValue(ui->tableView->selectionModel()->selectedRows(13).at(i).data().toString());
                    query->addBindValue(ui->tableView->selectionModel()->selectedRows(4).at(i).data().toString());
                    query->addBindValue(2);
                }
                else
                {
                    query->addBindValue(ui->tableView->selectionModel()->selectedRows(14).at(i).data().toString());
                    query->addBindValue(ui->tableView->selectionModel()->selectedRows(5).at(i).data().toString());
                    query->addBindValue(1);
                }
                query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toInt());
                query->addBindValue(percent);
                query->exec();
                query->clear();
            }
        }
    }
    slot_clear_form();
    slot_select_pp();
}

//Очищаем форму
void class_ref_pp::slot_clear_form()
{
    ui->tableView->clearSelection();
    ui->checkBox_save_action->setChecked(false);
    slot_set_margin();
}

//Заполняем поле с коммисией
void class_ref_pp::slot_set_margin()
{
    if (ui->tableView->selectionModel()->hasSelection())
    {
        query->prepare("SELECT t_trans_in, t_trans_in_s, t_trans_out FROM clients WHERE id = ?");
        query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()).toString());
        query->exec();
        query->first();
        if (ui->tableView->selectionModel()->selectedIndexes().at(3).data().toString() == "Расход")
        {

            ui->lineEdit_margin->setText(query->value(2).toString());
        }
        else
        {
            if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "1")
            {
                ui->lineEdit_margin->setText(query->value(1).toString());
            }
            if (ui->tableView->selectionModel()->selectedIndexes().at(12).data().toString() == "0")
            {
                ui->lineEdit_margin->setText(query->value(0).toString());
            }
        }
        query->clear();
    }
    else
    {
        ui->lineEdit_margin->setText("");
    }
}

//Отменяем платёжку
void class_ref_pp::slot_cancel_pp()
{
    int ret = QMessageBox::warning(this, tr("Внимание"),
                                    tr("Удаление платежей необходимо только при отзыве платежа.\nВы уверены что хотите удалить платёжное поручение?"),
                                    QMessageBox::Yes
                                    | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    switch (ret) {
       case QMessageBox::Yes:
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
                    query->prepare("SELECT id, margin FROM clients_operations WHERE id_pp = ?");
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
                    break;
               case QMessageBox::Cancel:
                   return;
                   break;
                }
            }

            int status = 0;
            db->transaction();
            if (ui->tableView->selectionModel()->selectedRows(3).at(i).data().toString() == "Приход")
            {
                query->prepare("UPDATE rss_balans SET balans = (balans - ?) WHERE id = ?");
            }
            if (ui->tableView->selectionModel()->selectedRows(3).at(i).data().toString() == "Расход")
            {
                query->prepare("UPDATE rss_balans SET balans = (balans + ?) WHERE id = ?");
            }
            query->addBindValue(ui->tableView->selectionModel()->selectedRows(2).at(i).data().toString());
            query->addBindValue(ui->tableView->selectionModel()->selectedRows(15).at(i).data().toString());
            if (query->exec()) status++;
            query->clear();

            query->prepare("DELETE FROM pp WHERE id = ?");
            query->addBindValue(ui->tableView->selectionModel()->selectedRows(0).at(i).data().toString());
            if (query->exec()) status++;
            query->clear();
            if (status == 2)
            {
                db->commit();
            }
            else
            {
                db->rollback();
            }
        }
            slot_select_pp();
            break;
       case QMessageBox::Cancel:
           return;
           break;
     }
}

void class_ref_pp::slot_print_pp(QString str)
{
    print_pp = new class_print_pp(this, ui->tableView->selectionModel()->selectedIndexes().at(0).data().toInt(), str);
}

void class_ref_pp::slot_send_show_ref_client()
{
    emit this->show_ref_client();
}

//Печать реестра
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

//Сохраняем реестр
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

//Вывод превью перед печатью
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
