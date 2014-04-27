#include "class_load_pp.h"
#include "ui_class_load_pp.h"

class_load_pp::class_load_pp(QWidget *parent, QSqlDatabase *db1) :
    QWidget(parent),
    ui(new Ui::class_load_pp)
{
    ui->setupUi(this);

    db = db1;

    query = new QSqlQuery;

    ucb = new update_client_balans;

    set_path_to_pp();
    if (ui->lineEdit_path->text() != "")
        ui->pushButton_load_db->setEnabled(true);

    //Открываем окно выбора файла
    connect(ui->comboBox_openfile, SIGNAL(activated(int)), SLOT(slot_open_file_dialog()));

    //Проверяем платёжки
    connect(ui->pushButton_load, SIGNAL(clicked()), SLOT(slot_load_pp()));

    //Загружаем платёжки в базу
    connect(ui->pushButton_load_db, SIGNAL(clicked()), SLOT(slot_load_pp_db()));
}

//Получаем путь к папке с пп из настроек
void class_load_pp::set_path_to_pp()
{
    query->exec("SELECT value FROM settings WHERE name = 'path_to_pp_dir'");
    query->first();
    ui->lineEdit_path->setText(query->value(0).toString());
    query->clear();
}

//Открываем окно выбора файла
void class_load_pp::slot_open_file_dialog()
{
    ui->pushButton_load->setEnabled(false);
    ui->pushButton_load_db->setEnabled(false);
    clear_window();
    if (ui->comboBox_openfile->currentText() == "Открыть файл")
    {
        ui->lineEdit_path->setText(QFileDialog::getOpenFileName(this, "Открыть выписку", "", "(*.*)"));
    }
    else
    {
        ui->lineEdit_path->setText(QFileDialog::getExistingDirectory(this, "Выбрать папку", ""));
    }
    slot_set_enable_load();
}

//Включаем кнопку загрузить
void class_load_pp::slot_set_enable_load()
{
    if (ui->comboBox_openfile->currentIndex() != 0)
    {
        if (ui->lineEdit_path->text() != "")
        {
            ui->pushButton_load->setEnabled(true);
        }
        else
        {
            ui->pushButton_load->setEnabled(false);
        }
    }
    else
    {
        if (ui->lineEdit_path->text() != "")
        {
            ui->pushButton_load_db->setEnabled(true);
        }
        else
        {
            ui->pushButton_load_db->setEnabled(false);
        }
    }
}


void class_load_pp::slot_load_pp()
{
    if (verify_pp(ui->lineEdit_path->text()))
    {
        ui->pushButton_load_db->setEnabled(true);
        ui->pushButton_load->setEnabled(false);
    }
}

//Проверка платёжек выписки
bool class_load_pp::verify_pp(QString file)
{
    //qDebug() << file << endl;
    int i = 0;
    date_start = date_end = rs = start_balans = all_in = all_out = end_balans = inn = firm_name = "";

    vector_pp = new QVector<class_pp>;

    file_pp = new QFile;
    file_pp->setFileName(file);
    if (!file_pp->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка", "Не удаётся прочитать файл:\n" + file);
        return false;
    }
    QTextStream *in;
    in = new QTextStream(file_pp);

    in->seek(0);
    QString charset = set_codec(in);
    if (charset != "")
    {
        in->setCodec(charset.toUtf8());
    }
    else
    {
        QMessageBox::critical(this, "Ошибка", "Некорректный формат файла:\n" + file);
        file_pp->close();
        return false;
    }
    in->seek(0);
    bik = get_bik(in);
    in->seek(0);
    QString line, var, value;
    pp_count = date_balans = 0;
    line = in->readLine();
    while (!line.isNull())
    {
        var = line.mid(0,line.indexOf("="));
        value = line.mid(line.indexOf("=")+1,line.size()-1);
        if (var == "СекцияРасчСчет")
        {
            line = in->readLine();
            var = line.mid(0,line.indexOf("="));
            value = line.mid(line.indexOf("=")+1,-1);
            while (var != "КонецРасчСчет" and i < 8)
            {
                if (var == "ДатаНачала") date_start = QString::number(QDate::fromString(value, "dd.MM.yyyy").toJulianDay());
                if (var == "ДатаКонца") date_end = QString::number(QDate::fromString(value, "dd.MM.yyyy").toJulianDay());
                if (var == "РасчСчет") rs = value.replace(" ", "");
                if (var == "НачальныйОстаток") start_balans = QString::number(value.replace(",", ".").toDouble(), 'f', 2);
                if (var == "ВсегоПоступило") all_in = QString::number(value.replace(",", ".").toDouble(), 'f', 2);
                if (var == "ВсегоСписано") all_out = QString::number(value.replace(",", ".").toDouble(), 'f', 2);
                if (var == "КонечныйОстаток") end_balans = QString::number(value.replace(",", ".").toDouble(), 'f', 2);
                line = in->readLine();
                var = line.mid(0,line.indexOf("="));
                value = line.mid(line.indexOf("=")+1,-1);
                i++;
            }
            if (i > 7)
            {
                //ui->textEdit->append("Ошибка формата файла в строке " + QString::number(file_pp->handle()));
                return false;
            }
            summ = "";
            summ_in = summ_out = 0;
        }
        i = 0;
        if (var == "СекцияДокумент")
        {
            pp = new class_pp;
            if (value == "Платежное поручение") pp->type_doc = "0";
            if (value == "Мемориальный ордер") pp->type_doc = "1";
            if (value == "Банковский ордер") pp->type_doc = "2";
            while (var != "КонецДокумента" and i < 64)
            {
                if (var == "Номер") pp->num = value.replace(" ", "");;
                if (var == "Дата") pp->date = value;
                if (var == "Сумма") pp->sum = value.replace(" ", "");;
                if (var == "КвитанцияДата") pp->ticket_date = value;
                if (var == "КвитанцияВремя") pp->ticket_time = value;
                if (var == "КвитанцияСодержание") pp->ticket_value = value;
                if (var == "ПлательщикСчет") pp->payer_count = value.replace(" ", "");;
                if (var == "ДатаСписано" and (pp->date_oper == "" or pp->date_oper == "0" or pp->date_oper.isNull()) and !value.isNull() and !value.isEmpty())
                    pp->date_oper = value;
                if (var == "ДатаПоступило" and (pp->date_oper == "" or pp->date_oper == "0" or pp->date_oper.isNull()) and !value.isNull() and !value.isEmpty())
                    pp->date_oper = value;
                if (var == "Плательщик") pp->payer = value;
                if (var == "ПлательщикИНН") pp->payer_inn = value.replace(" ", "");;
                if (var == "Плательщик1" and value != "") pp->payer1 = value;
                if (var == "Плательщик2") pp->payer2 = value;
                if (var == "Плательщик3") pp->payer3 = value;
                if (var == "Плательщик4") pp->payer4 = value;
                if (var == "ПлательщикРасчСчет") pp->payer_rs = value.replace(" ", "");;
                if (var == "ПлательщикБанк1") pp->payer_bank1 = value;
                if (var == "ПлательщикБанк2") pp->payer_bank2 = value;
                if (var == "ПлательщикБИК") pp->payer_bik = value.replace(" ", "");;
                if (var == "ПлательщикКорсчет") pp->payer_ks = value.replace(" ", "");;
                if (var == "ПолучательСчет") pp->receiver_count = value.replace(" ", "");;
                if (var == "Получатель") pp->receiver = value;
                if (var == "ПолучательИНН") pp->receiver_inn = value.replace(" ", "");;
                if (var == "Получатель1") pp->receiver1 = value;
                if (var == "Получатель2") pp->receiver2 = value;
                if (var == "Получатель3") pp->receiver3 = value;
                if (var == "Получатель4") pp->receiver4 = value;
                if (var == "ПолучательРасчСчет") pp->receiver_rs = value.replace(" ", "");;
                if (var == "ПолучательБанк1") pp->receiver_bank1 = value;
                if (var == "ПолучательБанк2") pp->receiver_bank2 = value;
                if (var == "ПолучательБИК") pp->receiver_bik = value.replace(" ", "");;
                if (var == "ПолучательКорсчет") pp->receiver_ks = value.replace(" ", "");;
                if (var == "ВидПлатежа") pp->type_pay = value;
                if (var == "ВидОплаты") pp->type_trans = value;
                if (var == "Код") pp->code = value;
                if (var == "НазначениеПлатежа") pp->dest_pay = value;
                if (var == "НазначениеПлатежа1") pp->dest_pay1 = value;
                if (var == "НазначениеПлатежа2") pp->dest_pay2 = value;
                if (var == "НазначениеПлатежа3") pp->dest_pay3 = value;
                if (var == "НазначениеПлатежа4") pp->dest_pay4 = value;
                if (var == "НазначениеПлатежа5") pp->dest_pay5 = value;
                if (var == "НазначениеПлатежа6") pp->dest_pay6 = value;
                if (var == "СтатусСоставителя") pp->state_sender = value;
                if (var == "ПлательщикКПП") pp->payer_kpp = value.replace(" ", "");;
                if (var == "ПолучательКПП") pp->receiver_kpp = value.replace(" ", "");;
                if (var == "ПоказательКБК") pp->pokazatel_kbk = value.replace(" ", "");;
                if (var == "ОКАТО") pp->okato = value.replace(" ", "");;
                if (var == "ПоказательОснования") pp->pokazatel_osnovaniya = value;
                if (var == "ПоказательПериода") pp->pokazatel_period = value;
                if (var == "ПоказательНомера") pp->pokazatel_num = value;
                if (var == "ПоказательДаты") pp->pokazatel_date = value;
                if (var == "ПоказательТипа") pp->pokazatel_type = value;
                if (var == "Очередность") pp->ocherednost = value;
                if (var == "СрокАкцепта") pp->srok_accepta = value;
                if (var == "ВидАккредитива") pp->type_akkred = value;
                if (var == "СрокПлатежа") pp->srok_pay = value;
                if (var == "УсловиеОплаты1") pp->usl_pay1 = value;
                if (var == "УсловиеОплаты2") pp->usl_pay2 = value;
                if (var == "УсловиеОплаты3") pp->usl_pay3 = value;
                if (var == "ПлатежПоПредст") pp->pay_po_predst = value;
                if (var == "ДополнУсловия") pp->dop_usl = value;
                if (var == "НомерСчетаПоставщика") pp->num_scheta_postav = value;
                if (var == "ДатаОтсылкиДок") pp->date_send_doc = value;
                line = in->readLine();
                var = line.mid(0,line.indexOf("="));
                value = line.mid(line.indexOf("=")+1,line.size()-1);
                i++;
            }
            if (pp->date_oper == "" or pp->date_oper == "0"  or pp->date_oper.isNull()) pp->date_oper = pp->date;

            if (pp->payer_rs == rs or pp->payer_count == rs)
            {
                pp->rs_id = get_id_rs(rs, pp->payer_bik);
                pp->type = "1";
                summ_out = summ_out + pp->sum.replace(",", ".").toDouble();
                if (inn == "")
                {
                    inn = pp->payer_inn;
                }
                if (firm_name == "")
                {
                    if (pp->payer1 == "" or pp->payer1.isNull() or pp->payer1.isEmpty())
                    {
                        firm_name = pp->payer;
                    }
                    else
                    {
                        firm_name = pp->payer1;
                    }
                }
            }
            else if (pp->receiver_rs == rs or pp->receiver_count == rs)
            {
                pp->rs_id = get_id_rs(rs, pp->receiver_bik);
                pp->type = "2";
                summ_in = summ_in + pp->sum.replace(",", ".").toDouble();
                if (inn == "")
                {
                    inn = pp->receiver_inn;
                }
                if (firm_name == "")
                {
                    if (pp->receiver1 == "" or pp->receiver1.isNull() or pp->receiver1.isEmpty())
                    {
                        firm_name = pp->receiver;
                    }
                    else
                    {
                        firm_name = pp->receiver1;
                    }
                }
            }
            else
            {
                message_pp_type = new QMessageBox;
                message_pp_type->setText("Платёжное поручение №" + pp->num + " в файле " + file + " не опознано.\nНазначение платежа:\n" + pp->dest_pay + "\nЗапсиать эту платёжку как приход или расход?");
                message_pp_type->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                message_pp_type->setDefaultButton(QMessageBox::Yes);
                message_pp_type->setButtonText(QMessageBox::Yes, trUtf8("Приход"));
                message_pp_type->setButtonText(QMessageBox::No, trUtf8("Расход"));
                message_pp_type->setButtonText(QMessageBox::Cancel, trUtf8("Пропустить"));
                int ret = message_pp_type->exec();
                if (ret == QMessageBox::Yes)
                {
                    pp->rs_id = get_id_rs(rs, pp->receiver_bik);
                    pp->type = "2";
                    summ_in = summ_in + pp->sum.replace(",", ".").toDouble();
                }
                else if (ret == QMessageBox::No)
                {
                    pp->rs_id = get_id_rs(rs, pp->payer_bik);
                    pp->type = "1";
                    summ_out = summ_out + pp->sum.replace(",", ".").toDouble();
                }
                delete message_pp_type;
            }
            vector_pp->append(*pp);
            delete pp;
            if (i > 63 or i < 23)
            {
                QMessageBox::critical(this, "Ошибка", "Некорректный файл:\n" + file);
                return false;

            }
            if (ui->comboBox_openfile->currentText() == "Открыть файл")
            {

            }
            if (ui->comboBox_openfile->currentIndex() != 0)
            {
                ui->tableWidget->setColumnWidth(1, 250);
                            ui->tableWidget->setColumnWidth(2, 150);
                            ui->tableWidget->setColumnWidth(4, 500);
                            ui->tableWidget->insertRow(pp_count);
                            ui->tableWidget->setItem(pp_count, 0, new QTableWidgetItem(vector_pp->at(pp_count).num, 0));
                            ui->tableWidget->setItem(pp_count, 1, new QTableWidgetItem(rs, 0));
                            if (vector_pp->at(pp_count).type == "2") ui->tableWidget->setItem(pp_count, 2, new QTableWidgetItem("Поступление", 0));
                            if (vector_pp->at(pp_count).type == "1") ui->tableWidget->setItem(pp_count, 2, new QTableWidgetItem("Списание", 0));
                            ui->tableWidget->setItem(pp_count, 3, new QTableWidgetItem(vector_pp->at(pp_count).date_oper, 0));
                            ui->tableWidget->setItem(pp_count, 4, new QTableWidgetItem(vector_pp->at(pp_count).dest_pay, 0));
                            ui->tableWidget->setItem(pp_count, 5, new QTableWidgetItem(vector_pp->at(pp_count).sum, 0));
            }
            pp_count++;
        }
        line = in->readLine();
    }
    if (QString::number(summ_in,'f',2) != all_in)
    {
        QMessageBox::critical(this, "Внимание", "Не сходится приход. В файле:\n" + file);
        //file_pp->close();
        //return false;
    }
    if (QString::number(summ_out,'f',2) != all_out)
    {
        QMessageBox::critical(this, "Внимание", "Не сходится расход. В файле:\n" + file);
        //file_pp->close();
        //return false;
    }
    //qDebug() << start_balans.toDouble() << summ_in << summ_out << end_balans << endl;
    if (QString::number(start_balans.toDouble() + (summ_in - summ_out), 'f', 2) != end_balans)
    {
        //qDebug() << QString::number(start_balans.toDouble() + (summ_in - summ_out), 'f', 2) << end_balans;
        QMessageBox::critical(this, "Внимание", "Не сходится конечный баланс. В файле:\n" + file);
        //file_pp->close();
        //return false;
    }
    file_pp->close();
    delete file_pp;
    delete in;
    return true;
}

void class_load_pp::load_pp(QString file)
{
    summ_in = summ_out = 0;
    QString delta;

    qDebug() << get_firm_id(inn) << inn << endl;

    if (get_firm_id(inn) == "")
    {
        query->prepare("INSERT INTO firms (name, inn, stroy) "
                                "VALUES (?, ?, 0)");
        query->addBindValue(firm_name);
        query->addBindValue(inn);
        query->exec();
        qDebug() << query->lastError().text() << endl;
        query->clear();
    }

    if (get_id_rs(rs, bik) == "")
    {
        QString firm_id = get_firm_id(inn);
        int ret = QMessageBox::warning(this, tr("Внимание"),
                                        tr("Не существует такого расчётного счёта:\n" + rs.toUtf8() + "\nСоздать?"),
                                        QMessageBox::Yes
                                        | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        switch (ret)
        {
           case QMessageBox::Yes:
                query->prepare("INSERT INTO rss (name, bik, number, firm, start_balans)"
                               "VALUES (?, ?, ?, ?, ?)");
                query->addBindValue(bik + "/" + rs);
                query->addBindValue(bik);
                query->addBindValue(rs);
                query->addBindValue(firm_id);
                query->addBindValue(start_balans);
                query->exec();
                query->first();
                query->clear();
                break;
           case QMessageBox::Cancel:
               return;
               break;
         }
    }
    /*
    query->exec("SELECT date FROM rss_balans WHERE id = " + get_id_rs(rs, bik));
    query->first();
    date_balans = query->value(0).toInt();
*/
    query->exec("SELECT date FROM rss_balans WHERE id = " + get_id_rs(rs, bik));
    query->first();
    //double old_balans = query->value(1).toDouble();
    int old_date = query->value(0).toInt();
    //qDebug() << old_balans << endl;
    for(int i = 0; i != vector_pp->size(); i++)
    {
        query->prepare("INSERT INTO pp (rs_id, client_id, type_pp, type_doc, num_pp, date_pp, date_oper, sum_pp, ticket_date, ticket_time, ticket_value, payer_count,"
                                        "payer, payer_inn, payer1, payer2, payer3, payer4, payer_rs, payer_bank1,"
                                        "payer_bank2, payer_bik, payer_ks, receiver_count, receiver, receiver_inn, receiver1, receiver2, receiver3,"
                                        "receiver4, receiver_rs, receiver_bank1, receiver_bank2, receiver_bik, receiver_ks, type_pay,"
                                        "type_trans, code, dest_pay, dest_pay1, dest_pay2, dest_pay3, dest_pay4, dest_pay5, dest_pay6,"
                                        "state_sender, payer_kpp, receiver_kpp, pokazatel_kbk, okato, pokazatel_osnovaniya, pokazatel_period,"
                                        "pokazatel_num, pokazatel_date, pokazatel_type, ocherednost, srok_accepta, type_akkred, srok_pay,"
                                        "usl_pay1, usl_pay2, usl_pay3, pay_po_predst, dop_usl, num_scheta_postav, date_send_doc)"
                       "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        query->addBindValue(get_id_rs(rs, bik));
        query->addBindValue(vector_pp->at(i).client_id);
        query->addBindValue(vector_pp->at(i).type);
        query->addBindValue(vector_pp->at(i).type_doc);
        query->addBindValue(vector_pp->at(i).num);
        query->addBindValue(vector_pp->at(i).date);
        query->addBindValue(vector_pp->at(i).date_oper);
        query->addBindValue(vector_pp->at(i).sum);
        query->addBindValue(vector_pp->at(i).ticket_date);
        query->addBindValue(vector_pp->at(i).ticket_time);
        query->addBindValue(vector_pp->at(i).ticket_value);
        query->addBindValue(vector_pp->at(i).payer_count);
        query->addBindValue(vector_pp->at(i).payer);
        query->addBindValue(vector_pp->at(i).payer_inn);
        query->addBindValue(vector_pp->at(i).payer1);
        query->addBindValue(vector_pp->at(i).payer2);
        query->addBindValue(vector_pp->at(i).payer3);
        query->addBindValue(vector_pp->at(i).payer4);
        query->addBindValue(vector_pp->at(i).payer_rs);
        query->addBindValue(vector_pp->at(i).payer_bank1);
        query->addBindValue(vector_pp->at(i).payer_bank2);
        query->addBindValue(vector_pp->at(i).payer_bik);
        query->addBindValue(vector_pp->at(i).payer_ks);
        query->addBindValue(vector_pp->at(i).receiver_count);
        query->addBindValue(vector_pp->at(i).receiver);
        query->addBindValue(vector_pp->at(i).receiver_inn);
        query->addBindValue(vector_pp->at(i).receiver1);
        query->addBindValue(vector_pp->at(i).receiver2);
        query->addBindValue(vector_pp->at(i).receiver3);
        query->addBindValue(vector_pp->at(i).receiver4);
        query->addBindValue(vector_pp->at(i).receiver_rs);
        query->addBindValue(vector_pp->at(i).receiver_bank1);
        query->addBindValue(vector_pp->at(i).receiver_bank2);
        query->addBindValue(vector_pp->at(i).receiver_bik);
        query->addBindValue(vector_pp->at(i).receiver_ks);
        query->addBindValue(vector_pp->at(i).type_pay);
        query->addBindValue(vector_pp->at(i).type_trans);
        query->addBindValue(vector_pp->at(i).code);
        query->addBindValue(vector_pp->at(i).dest_pay);
        query->addBindValue(vector_pp->at(i).dest_pay1);
        query->addBindValue(vector_pp->at(i).dest_pay2);
        query->addBindValue(vector_pp->at(i).dest_pay3);
        query->addBindValue(vector_pp->at(i).dest_pay4);
        query->addBindValue(vector_pp->at(i).dest_pay5);
        query->addBindValue(vector_pp->at(i).dest_pay6);
        query->addBindValue(vector_pp->at(i).state_sender);
        query->addBindValue(vector_pp->at(i).payer_kpp);
        query->addBindValue(vector_pp->at(i).receiver_kpp);
        query->addBindValue(vector_pp->at(i).pokazatel_kbk);
        query->addBindValue(vector_pp->at(i).okato);
        query->addBindValue(vector_pp->at(i).pokazatel_osnovaniya);
        query->addBindValue(vector_pp->at(i).pokazatel_period);
        query->addBindValue(vector_pp->at(i).pokazatel_num);
        query->addBindValue(vector_pp->at(i).pokazatel_date);
        query->addBindValue(vector_pp->at(i).pokazatel_type);
        query->addBindValue(vector_pp->at(i).ocherednost);
        query->addBindValue(vector_pp->at(i).srok_accepta);
        query->addBindValue(vector_pp->at(i).type_akkred);
        query->addBindValue(vector_pp->at(i).srok_pay);
        query->addBindValue(vector_pp->at(i).usl_pay1);
        query->addBindValue(vector_pp->at(i).usl_pay2);
        query->addBindValue(vector_pp->at(i).usl_pay3);
        query->addBindValue(vector_pp->at(i).pay_po_predst);
        query->addBindValue(vector_pp->at(i).dop_usl);
        query->addBindValue(vector_pp->at(i).num_scheta_postav);
        query->addBindValue(vector_pp->at(i).date_send_doc);
        query->exec();
        if (!query->lastError().isValid())
        {
            if (vector_pp->at(i).type.toInt() == 1)
            {
                pp_out_count++;
            }
            else
            {
                pp_in_count++;
            }
            if (vector_pp->at(i).date_oper.toInt() > old_date)
            {
                delta = vector_pp->at(i).sum;
                if (vector_pp->at(i).type.toInt() == 1)
                {
                    summ_out += delta.replace(",", ".").toDouble();
                }
                if (vector_pp->at(i).type.toInt() == 2)
                {
                    summ_in += delta.replace(",", ".").toDouble();
                }
                //qDebug() << summ_out << summ_in << delta.replace(",", ".").toDouble() << i << endl;
            }

            if (vector_pp->at(i).type_doc.toInt() > 0)
            {
                if (vector_pp->at(i).type.toInt() == 1)
                query->prepare("SELECT seq FROM sqlite_sequence WHERE name = 'pp'");
                query->exec();
                query->first();
                if (vector_pp->at(i).type.toInt() == 1)
                    ucb->slot_update_balans("0",
                                        "Расход",
                                        vector_pp->at(i).sum,
                                        query->value(0).toString(),
                                        false,
                                        vector_pp->at(i).dest_pay,
                                        true,
                                        "",
                                        ""
                            );
                /*if (vector_pp->at(i).type.toInt() == 2)
                    ucb->slot_update_balans("0",
                                        "Приход",
                                        vector_pp->at(i).sum,
                                        query->value(0).toString(),
                                        false,
                                        vector_pp->at(i).dest_pay,
                                        true,
                                        "",
                                        ""
                            );*/
            }
        }
        else
        {
            //qDebug() << vector_pp->at(i).num << query->lastError().text() << query->lastQuery() << endl;
        }
        query->clear();
        query->exec("COMMIT");
        ui->tableWidget->removeRow(0);
    }
    //qDebug() << old_balans << QString::number(old_balans - summ_out + summ_in, 'f', 2) << summ_out << summ_in << endl;
    if (date_balans <= date_end.toInt())
    {
        query->exec("BEGIN IMMEDIATE TRANSACTION");
        //query->exec("UPDATE rss_balans SET id = " + get_id_rs(rs, bik) + ", date = " + date_end + ", balans = '" + QString::number(old_balans - summ_out + summ_in, 'f', 2) + "' WHERE id = " + get_id_rs(rs, bik));
        query->exec("UPDATE rss_balans SET id = " + get_id_rs(rs, bik) + ", date = " + date_end + ", balans = '" + end_balans + "' WHERE id = " + get_id_rs(rs, bik));
        query->first();
        query->clear();
        query->exec("COMMIT");
    }
    delete vector_pp;
    report += "<tr><td>" + file.right(file.length() - file.lastIndexOf('/') - 1) + "</td><td>" + QString::number(pp_in_count, 'f', 0) + "</td><td>" + QString::number(pp_in_count, 'f', 0) + "</td><td>" + QString::number(pp_in_count + pp_out_count, 'f', 0) + "</td></tr>";
}

//Очищаем окно
void class_load_pp::clear_window()
{
    ui->tableWidget->clearContents();
    while (ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
}

//Устанавливем кодировку
QString class_load_pp::set_codec(QTextStream *in)
{
    codepage.append("CP1251");
    codepage.append("CP866");
    codepage.append("UTF-8");
    QString line;
    for (int i=0;i<3;i++)
    {
        file_pp->seek(0);
        in->setCodec(codepage.at(i).toUtf8());
        line = in->readLine();
        while (!line.isNull())
        {
            if (line.mid(0,line.indexOf("=")) == "Кодировка") return codepage.at(i);
            line = in->readLine();
        }
    }
    delete in;
    return "";
}


//Определяем бик
QString class_load_pp::get_bik(QTextStream *in)
{
    QString line;
    QString rs;
    QString bik;
    line = in->readLine();
    while (line.mid(0,line.indexOf("=")) != "КонецДокумента")
    {
        if (line.mid(0,line.indexOf("=")) == "РасчСчет") rs = line.mid(line.indexOf("=")+1,-1);
        //qDebug() << line.mid(line.indexOf("=")+1,-1) << endl;
        if ((line.mid(0,line.indexOf("=")) == "ПлательщикРасчСчет" or line.mid(0,line.indexOf("=")) == "ПлательщикСчет") and line.mid(line.indexOf("=")+1,-1) == rs)
        {
            bik = "ПлательщикБИК";
        }
        if ((line.mid(0,line.indexOf("=")) == "ПолучательРасчСчет" or line.mid(0,line.indexOf("=")) == "ПолучательСчет") and line.mid(line.indexOf("=")+1,-1) == rs)
        {
            bik = "ПолучательБИК";
        }
        //if (line.mid(0,line.indexOf("=")) == bik) return line.mid(line.indexOf("=")+1,-1);
        line = in->readLine();
    }
    //qDebug() << rs << bik << endl;
    in->seek(0);
    line = in->readLine();
    while (line.mid(0,line.indexOf("=")) != "КонецДокумента")
    {
        if (line.mid(0,line.indexOf("=")) == bik) return line.mid(line.indexOf("=")+1,-1);
        line = in->readLine();
    }
}


//Загрузка платёжек в базу
void class_load_pp::slot_load_pp_db()
{
    if (ui->comboBox_openfile->currentIndex() != 0)
    {
        report = "<b>Загружено платёжек в базу</b><br><table><tr><td><b>Файл</b></td><td><b>Приход</b></td><td><b>Расход</b></td><td><b>Сумма</b></td></tr>";
        pp_in_count = pp_out_count = 0;
        load_pp(ui->lineEdit_path->text());
        report += "</table>";
        report_pp = new class_report_load_pp;
        report_pp->slot_set_text(report);
        report_pp->show();
        ui->tableWidget->clearContents();
    }
    else
    {
        QDir dir(ui->lineEdit_path->text());
        QStringList filters;
        filters << "*.txt";
        dir.setNameFilters(filters);
        QStringList list_file = dir.entryList();
        report = "<b>Загружено платёжек в базу</b><br><table><tr><td><b>Файл</b></td><td><b>Приход</b></td><td><b>Расход</b></td><td><b>Сумма</b></td></tr>";
        pp_in_count = pp_out_count = 0;
        for (int i = 0; i < list_file.size(); i++)
        {
            pp_in_count = pp_out_count = 0;
            //qDebug() << i << endl;
            if (verify_pp(ui->lineEdit_path->text() + "/" + list_file.at(i)))
            {
                load_pp(ui->lineEdit_path->text() + "/" + list_file.at(i));
            }
            else
            {
                //qDebug() << "111111111111111111111111" << endl;
            }
        }
        report += "</table>";
        report_pp = new class_report_load_pp;
        report_pp->slot_set_text(report);
        report_pp->show();
    }
    ui->lineEdit_path->setText("");
    ui->pushButton_load_db->setEnabled(false);
}

//Получаем id фирмы по ИНН
QString class_load_pp::get_firm_id(QString inn)
{
    query->prepare("SELECT id FROM firms WHERE inn = ?");
    query->addBindValue(inn);
    query->exec();
    query->first();
    QString f_id = query->value(0).toString();
    query->clear();
    return f_id;
}

//Получаем id расчётного счёта
QString class_load_pp::get_id_rs(QString rs, QString bik)
{
    QSqlQuery *query1 = new QSqlQuery;
    query1->prepare("SELECT id FROM rss WHERE bik = ? and number = ?");
    query1->addBindValue(bik);
    query1->addBindValue(rs);
    query1->exec();
    query1->first();
    QString str = query1->value(0).toString();
    query1->clear();
    delete query1;
    return str;
}

//Получаем номер расчётного счёта по id
QString class_load_pp::get_rs_id(QString id)
{
    query->prepare("SELECT name FROM rss WHERE id = ?");
    query->addBindValue(id);
    query->exec();
    query->first();
    return query->value(0).toString();
}

class_load_pp::~class_load_pp()
{
    delete ui;
}
