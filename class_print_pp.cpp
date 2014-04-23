#include "class_print_pp.h"

class_print_pp::class_print_pp(QObject *parent, int id_pp, QString str) :
    QObject(parent)
{
    query = new QSqlQuery;

    query->prepare("SELECT * FROM pp WHERE id = ?");
    query->addBindValue(id_pp);
    query->exec();
    query->first();

    printer = new QPrinter;
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);
    printer->setFullPage(true);

    if (str == "Печатать ПП")
    {
        preview = new QPrintPreviewDialog(printer);
        preview->setWindowFlags ( Qt::Window );

        connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slot_print_preview(QPrinter*)));

        preview->exec();
    }
    else
    {
        open_dialog = new QFileDialog;
        printer->setOutputFileName(open_dialog->getSaveFileName(0, "Сохранить как", "", "PDF файл (*.pdf)"));
        delete open_dialog;
        slot_print_preview(printer);
    }
}

void class_print_pp::slot_print_preview(QPrinter *pr)
{
    painter = new QPainter(pr);

    painter->setPen(Qt::black);

    painter->drawLine(74, 55.5, 203.5, 55.5);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(80, 65, "Поступ. в банк плат.");

    painter->drawLine(240, 55.5, 371.5, 55.5);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(245, 65, "Списано со сч. плат.");

    painter->setFont(QFont("Sans",12,0,0));
    painter->drawRect(660, 35, 83, 20.5);
    painter->drawText(666, 50, "0401060");

    painter->setFont(QFont("Sans",10,0,0));
    painter->drawText(76, 120, "ПЛАТЕЖНОЕ ПОРУЧЕНИЕ № " + query->value(5).toString());

    painter->drawLine(400, 120, 529.5, 120);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(445, 130, "Дата");
    painter->setFont(QFont("Sans",10,0,0));
    painter->drawText(425, 110, QDate::fromJulianDay(query->value(7).toInt()).toString("dd.MM.yyyy"));

    painter->drawLine(560, 120, 689.5, 120);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(590, 130, "Вид платежа");

    painter->drawRect(717, 94, 26, 26);


    painter->drawLine(148, 150, 148, 205);
    painter->drawLine(74, 205, 743, 205);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 160, "Сумма");
    painter->drawText(76, 180, "прописью");

    painter->drawLine(444, 205, 444, 538);
    painter->drawLine(500, 205, 500, 538);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 220, "ИНН");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(261, 220, "КПП");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 220, "Сумма");

    painter->drawLine(74, 227, 444, 227);
    painter->drawLine(259, 205, 259, 227);
    painter->drawLine(444, 260, 743, 260);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 285, "Сч. №");

    painter->drawLine(74, 316, 500, 316);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 310, "Плательщик");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 330, "БИК");
    painter->drawLine(444, 335, 500, 335);

    painter->drawLine(74, 371, 743, 371);
    painter->drawText(447, 350, "Сч. №");
    painter->drawText(76, 365, "Банк плательщика");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 385, "БИК");
    painter->drawText(447, 405, "Сч. №");
    painter->drawLine(444, 390, 500, 390);

    painter->drawLine(74, 427, 500, 427);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 422, "Банк получателя");

    painter->setFont(QFont("Sans",10,0,0));
    painter->drawText(76, 442, "ИНН");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(261, 442, "КПП");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 442, "Сч. №");

    painter->drawLine(74, 449, 444, 449);
    painter->drawLine(259, 427, 259, 449);
    painter->drawLine(444, 482, 743, 482);

    painter->drawLine(650, 482, 650, 538);
    painter->drawLine(576, 482, 576, 538);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 496, "Вид оп.");
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 496, "Срок плат.");
    painter->drawLine(444, 501, 500, 501);
    painter->drawLine(576, 501, 650, 501);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 514, "Наз. пл.");
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 514, "Очер. плат.");
    painter->drawLine(444, 519, 500, 519);
    painter->drawLine(576, 519, 650, 519);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 531, "Код");
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 531, "Рез поле.");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 533, "Получатель");
    painter->drawLine(74, 538, 743, 538);

    painter->drawLine(74, 557, 743, 557);

    painter->drawLine(241, 538, 241, 557);
    painter->drawLine(352, 538, 352, 557);
    painter->drawLine(389, 538, 389, 557);
    painter->drawLine(481, 538, 481, 557);
    painter->drawLine(611, 538, 611, 557);
    painter->drawLine(703, 538, 703, 557);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 663, "Назначение платежа");
    painter->drawLine(74, 668, 743, 668);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(390, 680, "Подписи");
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(600, 680, "Отметки банка");

    painter->drawLine(320, 724, 542, 724);
    painter->drawLine(320, 779, 542, 779);

    painter->drawText(150, 750, "М.П");

    painter->end();
    delete painter;
}
