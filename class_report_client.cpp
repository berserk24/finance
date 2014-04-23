#include "class_report_client.h"
#include "ui_class_report_client.h"

class_report_client::class_report_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_report_client)
{
    ui->setupUi(this);

    query = new QSqlQuery;
    ui->dateEdit_po->setDate(QDate::currentDate());
    ui->dateEdit_s->setDate(QDate::currentDate().addDays(-1));
    slot_select_client();

    connect(ui->pushButton_generate, SIGNAL(clicked()), SLOT(slot_gen_report()));

    connect(ui->comboBox_print_save, SIGNAL(activated(QString)), SLOT(slot_print_save()));
}

void class_report_client::slot_select_client()
{
    ui->comboBox_client->clear();
    query->exec("SELECT id, name FROM client");
    query->first();
    ui->comboBox_client->addItem(query->value(1).toString(), query->value(0).toInt());
    while (query->next())
    {
        ui->comboBox_client->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    query->clear();
}

void class_report_client::slot_gen_report()
{
    QString old_date, str_in_bn, str_in_nal, str_out_bn, str_out_nal, str_from, str_to, str_marg, str, about;
    old_date = str_in_bn = str_in_nal = str_out_bn = str_out_nal = str_from = str_to = str_marg = about = "";
    double in_bn, in_n, out_bn, out_n, margin, sum;
    in_bn = in_n = out_bn = out_n = margin = sum = 0;
    if (ui->comboBox_client->currentText() != "Наш счёт")
        query->prepare("SELECT strftime('%d.%m.%Y', date(julianday(oper.date))), IFNULL(pf.name, IFNULL(pp.payer1 , pp.payer)) AS payerr, IFNULL(rf.name, IFNULL(pp.receiver1, pp.receiver)) AS receiverr, type.id, CAST(oper.summ AS TEXT), oper.margin, oper.to_client_id, oper.id_client, client.name, oper.id_pp, oper.text, cl_to.name "
                   "FROM client_operations oper "
                   "LEFT JOIN pp ON oper.id_pp = pp.id "
                   "LEFT JOIN pp_in_out type ON oper.type = type.id "
                   "LEFT JOIN firm pf ON pf.inn = pp.payer_inn "
                   "LEFT JOIN firm rf ON rf.inn = pp.receiver_inn "
                   "LEFT JOIN client ON client.id = oper.id_client "
                   "LEFT JOIN client cl_to ON cl_to.id = oper.to_client_id "
                   "WHERE oper.date <= ? "
                   "AND oper.date >= ? "
                   "AND (oper.id_client = ? OR oper.to_client_id = ?) "
                   "ORDER BY oper.date");
    if (ui->comboBox_client->currentText() == "Наш счёт")
        query->prepare("SELECT strftime('%d.%m.%Y', date(julianday(oper.date))), IFNULL(pf.name, IFNULL(pp.payer1 , pp.payer)) AS payerr, IFNULL(rf.name, IFNULL(pp.receiver1, pp.receiver)) AS receiverr, type.id, CAST(oper.summ AS TEXT), oper.margin, oper.to_client_id, oper.id_client, client.name, oper.id_pp, oper.text, cl_to.name "
                   "FROM client_operations oper "
                   "LEFT JOIN pp ON oper.id_pp = pp.id "
                   "LEFT JOIN pp_in_out type ON oper.type = type.id "
                   "LEFT JOIN firm pf ON pf.inn = pp.payer_inn "
                   "LEFT JOIN firm rf ON rf.inn = pp.receiver_inn "
                   "LEFT JOIN client ON client.id = oper.id_client "
                   "LEFT JOIN client cl_to ON cl_to.id = oper.to_client_id "
                   "WHERE oper.date <= ? "
                   "AND oper.date >= ? "
                   "AND (oper.id_client = ? OR oper.to_client_id = ? OR CAST(oper.margin AS REAL) > 0) "
                   "ORDER BY oper.date");
    query->addBindValue(ui->dateEdit_po->date().toJulianDay());
    query->addBindValue(ui->dateEdit_s->date().toJulianDay());
    query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()));
    query->addBindValue(ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()));
    query->exec();
    query->first();
    old_date = query->value(0).toString();
    ui->textEdit->clear();
    ui->textEdit->setText("<meta content='text/html; charset=UTF-8' http-equiv='content-type'>");
    str = "<meta content='text/html; charset=UTF-8' http-equiv='content-type'><h3>Отчёт по клиенту " + ui->comboBox_client->currentText() + " за дату: " + query->value(0).toString() + "</h3>";
    str += "<table margin='5' border='1' bordercolor='black'><tr><td width='150'><b>От кого</b></td><td  width='150'><b>Кому</b></td><td  width='150'><b>Описание</b></td><td width='100'><b>Приход б/н</b></td><td width='100'><b>Приход нал.</b></td><td width='100'><b>Расход б/н</b></td><td width='100'><b>Расход нал.</b></td><td width='100'><b>Комиссия</b></td></tr>";
    if (query->value(1).toString() != "")
    {
        str_from = query->value(1).toString();
    }
    if (query->value(2).toString() != "")
    {
        str_to = query->value(2).toString();
    }
    if (query->value(3).toString() == "1")
    {
        if (str_from != "" and str_to != "")
        {
            about = "ПП №" + query->value(9).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != "0")
            {
                str_out_bn = "-" + query->value(4).toString();
            }
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_from = query->value(8).toString();
            }
            else
            {
                str_marg = "-" + query->value(5).toString();
            }
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0 and query->value(7).toInt() == 0)
            {
                str_out_bn = "-" + query->value(4).toString();
            }
        }
        if (str_from == "" and str_to == "")
        {
            str_from = "";
            about = query->value(10).toString();
            str_out_nal = "-" + query->value(4).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_from = "";
                str_to = query->value(8).toString();
                str_out_nal = " ";
            }
            else
            {
                str_marg = "-" + query->value(5).toString();
            }
        }
    }
    if (query->value(3).toString() == "2")
    {
        if (str_from != "" and str_to != "")
        {
            about = "ПП №" + query->value(9).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != "0")
            {
                str_in_bn = "+" + query->value(4).toString();
            }
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_to = query->value(8).toString();
            }
            else
            {
                str_marg = "-" + query->value(5).toString();
            }
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0 and query->value(7).toInt() == 0)
            {
                str_in_bn = "+" + query->value(4).toString();
            }
        }
        if (str_from == "" and str_to == "")
        {
            str_from = "";
            str_in_nal = "+" + query->value(4).toString();
            about = query->value(10).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_from = "";
                str_to = query->value(8).toString();
                str_in_nal = " ";
            }
            else
            {
                str_marg = "-" + query->value(5).toString();
            }
        }
    }

    //перевод
    if (query->value(3).toString() == "3" and query->value(6).toString() != ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()))
    {
        about = query->value(10).toString();
        str_out_bn = "-" + query->value(4).toString();
        str_from = query->value(8).toString();
        str_to = query->value(11).toString();
        if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
        {
            str_marg = "+" + query->value(5).toString();
            str_out_bn = "";
        }
        else
        {
            str_marg = "-" + query->value(5).toString();
        }
    }
    if (query->value(3).toString() == "3" and query->value(6).toString() == ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()))
    {
        about = query->value(10).toString();
        str_from = query->value(11).toString();
        str_to = query->value(8).toString();
        str_in_bn = "+" + query->value(4).toString();
        if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
        {
            str_marg = "+" + query->value(5).toString();
            str_in_bn = "";
        }
        else
        {
            str_marg = "";
        }
    }

    //if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != 0 and query->value(5).toString() != "0.00" and query->value(3).toString() != "3")
    {
        str += "<tr><td>" + str_from +
                            "</td><td>" + str_to +
                            "</td><td>" + about +
                            "</td><td>" + str_in_bn.replace(".", ",") +
                            "</td><td>" + str_in_nal.replace(".", ",") +
                            "</td><td>" + str_out_bn.replace(".", ",") +
                            "</td><td>" + str_out_nal.replace(".", ",") +
                            "</td><td>" + str_marg.replace(".", ",") + "</td></tr>";
    in_bn += str_in_bn.replace(",", ".").toDouble();
    in_n += str_in_nal.replace(",", ".").toDouble();
    out_bn += str_out_bn.replace(",", ".").toDouble();
    out_n += str_out_nal.replace(",", ".").toDouble();
    margin += str_marg.replace(",", ".").toDouble();
    }
    str_in_bn = str_in_nal = str_out_bn = str_out_nal = str_from = str_to = str_marg = about = "";
    while (query->next())
    {
        if (old_date != query->value(0).toString())
        {
            old_date = query->value(0).toString();
            sum += in_bn + in_n + out_bn + out_n + margin;
            str += "<tr><td><b>Сумма</b></td><td></td><td></td><td><b>" + QString::number(in_bn, 'f', 2).replace(".", ",") +
                    "</b></td><td><b>" + QString::number(in_n, 'f', 2).replace(".", ",") +
                    "</b></td><td><b>" + QString::number(out_bn, 'f', 2).replace(".", ",") +
                    "</b></td><td><b>" + QString::number(out_n, 'f', 2).replace(".", ",") +
                    "</b></td><td><b>" + QString::number(margin, 'f', 2).replace(".", ",") + "</b></td></tr>";
            str += "<tr><td><b>ИТОГО</b></td><td  colspan='7'><b>" + QString::number(in_bn + in_n + out_bn + out_n + margin, 'f', 2).replace(".", ",") +
                    "</td></tr></table><br><br><br>";
            in_bn = in_n = out_bn = out_n = margin = 0;
            ui->textEdit->append(str);
            str = "<h3>Отчёт по клиенту " + ui->comboBox_client->currentText() + " за дату: " + query->value(0).toString() + "</h3>";
            str += "<table margin='5' border='1' bordercolor='black'><tr><td width='150'><b>От кого</b></td><td  width='150'><b>Кому</b></td><td  width='150'><b>Описание</b><td width='100'><b>Приход б/н</b></td><td width='100'><b>Приход нал.</b></td><td width='100'><b>Расход б/н</b></td><td width='100'><b>Расход нал.</b></td><td width='100'><b>Комиссия</b></td></tr>";
        }
        else
        {
        }
        if (query->value(1).toString() != "")
        {
            str_from = query->value(1).toString();
        }
        if (query->value(2).toString() != "")
        {
            str_to = query->value(2).toString();
        }
        if (query->value(3).toString() == "1")
        {
            if (str_from != "" and str_to != "")
            {
                about = "ПП №" + query->value(9).toString();
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != "0")
                {
                    str_out_bn = "-" +  query->value(4).toString();
                }
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
                {
                    str_marg = "+" + query->value(5).toString();
                    str_from = query->value(8).toString();
                }
                else
                {
                    str_marg = "-" + query->value(5).toString();
                }
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0 and query->value(7).toInt() == 0)
                {
                    str_out_bn = "-" + query->value(4).toString();
                }
            }
            if (str_from == "" and str_to == "")
            {
                str_from = "";
                about = query->value(10).toString();
                str_out_nal = "-" + query->value(4).toString();
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
                {
                    str_marg = "+" + query->value(5).toString();
                    str_to = query->value(8).toString();
                    str_out_nal = " ";
                }
                else
                {
                    str_marg = "-" + query->value(5).toString();
                }
            }
        }
        if (query->value(3).toString() == "2")
        {
            if (str_from != "" and str_to != "")
            {
                about = "ПП №" + query->value(9).toString();
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != "0")
                {
                    str_in_bn = "+" + query->value(4).toString();
                }
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
                {
                    str_marg = "+" + query->value(5).toString();
                    str_to = query->value(8).toString();
                }
                else
                {
                    str_marg = "-" + query->value(5).toString();
                }
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0 and query->value(7).toInt() == 0)
                {
                    str_in_bn = "+" + query->value(4).toString();
                }
            }
            if (str_from == "" and str_to == "")
            {
                str_from = " ";
                str_in_nal = "+" + query->value(4).toString();
                about = query->value(10).toString();
                if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
                {
                    str_marg = "-" + query->value(5).toString();
                    str_from = "";
                    str_to = query->value(8).toString();
                    str_in_nal = " ";
                }
                else
                {
                    str_marg = "+" + query->value(5).toString();
                }
            }
        }

        //перевод
        if (query->value(3).toString() == "3" and query->value(6).toString() != ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()))
        {
            about = query->value(10).toString();
            str_from = query->value(8).toString();
            str_to = query->value(11).toString();
            str_out_bn = "-" + query->value(4).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_out_bn = "";
            }
            else
            {
                str_marg = "-" + query->value(5).toString();
            }
        }
        if (query->value(3).toString() == "3" and query->value(6).toString() == ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()))
        {
            about = query->value(10).toString();
            str_from = query->value(11).toString();
            str_to = query->value(8).toString();
            str_in_bn = "+" + query->value(4).toString();
            if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) == 0)
            {
                str_marg = "+" + query->value(5).toString();
                str_in_bn = "";
            }
            else
            {
                str_marg = "";
            }
        }

        //if (ui->comboBox_client->itemData(ui->comboBox_client->currentIndex()) != 0 and query->value(5).toString() != "0.00" and query->value(3).toString() != "3")
        {
        str += "<tr><td>" + str_from +
                                "</td><td>" + str_to +
                                "</td><td>" + about +
                                "</td><td>" + str_in_bn.replace(".", ",") +
                                "</td><td>" + str_in_nal.replace(".", ",") +
                                "</td><td>" + str_out_bn.replace(".", ",") +
                                "</td><td>" + str_out_nal.replace(".", ",") +
                                "</td><td>" + str_marg.replace(".", ",") + "</td></tr>";
        in_bn += str_in_bn.replace(",", ".").toDouble();
        in_n += str_in_nal.replace(",", ".").toDouble();
        out_bn += str_out_bn.replace(",", ".").toDouble();
        out_n += str_out_nal.replace(",", ".").toDouble();
        margin += str_marg.replace(",", ".").toDouble();
        }
        str_in_bn = str_in_nal = str_out_bn = str_out_nal = str_from = str_to = str_marg = "";
    }
    sum += in_bn + in_n + out_bn + out_n + margin;
    str += "<tr><td><b>Сумма</b></td><td></td><td></td><td><b>" + QString::number(in_bn, 'f', 2).replace(".", ",") +
            "</b></td><td><b>" + QString::number(in_n, 'f', 2).replace(".", ",") +
            "</b></td><td><b>" + QString::number(out_bn, 'f', 2).replace(".", ",") +
            "</b></td><td><b>" + QString::number(out_n, 'f', 2).replace(".", ",") +
            "</b></td><td><b>" + QString::number(margin, 'f', 2).replace(".", ",") + "</b></td></tr>";
    str += "<tr><td><b>ИТОГО</b></td><td  colspan='7'><b>" + QString::number(in_bn + in_n + out_bn + out_n + margin, 'f', 2).replace(".", ",") +
            "</td></tr></table><br><br><br>";
    str += "<h2>СУММА ЗА ПЕРИОД <b>" + QString::number(sum, 'f', 2) + "</b></h2>";
    in_bn = in_n = out_bn = out_n = margin = 0;
    ui->textEdit->append(str);
}
class_report_client::~class_report_client()
{
    delete ui;
}

void class_report_client::slot_print_save()
{
    printer = new QPrinter;
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    printer->setFullPage(true);

    if (ui->comboBox_print_save->currentText() == "Печатать")
    {
        preview = new QPrintPreviewDialog(printer);
        preview->setWindowFlags ( Qt::Window );

        connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slot_print_preview(QPrinter*)));

        preview->exec();
    }
    else
    {
        slot_save_report();
    }
}

void class_report_client::slot_print_preview(QPrinter *pr)
{
    ui->textEdit->print(pr);
}

void class_report_client::slot_save_report()
{
    open_dialog = new QFileDialog;
    QString str_file = open_dialog->getSaveFileName(this, "Сохранить как", "", "HTML файл (*.html)");
    if (str_file.right(5) != ".html")
        str_file += ".html";
    file = new QFile(str_file);
    delete open_dialog;
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
             return;
    QTextStream out(file);
    out << ui->textEdit->toHtml();
    delete file;
}
