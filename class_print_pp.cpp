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

QString class_print_pp::int_to_string(double n)
{
    if ( n > 999999999999 )
        return QString::fromUtf8("Очень много денег ...");

    if ( n < 0 )
        return QString::fromUtf8("Отрицательная сумма !");

    static QString cap[4][10] =
    {
        {
            "",
            QString::fromUtf8("один "),
            QString::fromUtf8("два "),
            QString::fromUtf8("три "),
            QString::fromUtf8("четыре "),
            QString::fromUtf8("пять "),
            QString::fromUtf8("шесть "),
            QString::fromUtf8("семь "),
            QString::fromUtf8("восемь "),
            QString::fromUtf8("девять ")
        },
        {
            "",
            QString::fromUtf8("одна "),
            QString::fromUtf8("две "),
            QString::fromUtf8("три "),
            QString::fromUtf8("четыре "),
            QString::fromUtf8("пять "),
            QString::fromUtf8("шесть "),
            QString::fromUtf8("семь "),
            QString::fromUtf8("восемь "),
            QString::fromUtf8("девять ")
        },
        {
            "",
            "",
            QString::fromUtf8("двадцать "),
            QString::fromUtf8("тридцать "),
            QString::fromUtf8("сорок "),
            QString::fromUtf8("пятьдесят "),
            QString::fromUtf8("шестьдесят "),
            QString::fromUtf8("семьдесят "),
            QString::fromUtf8("восемьдесят "),
            QString::fromUtf8("девяносто ")
        },
        {
            "",
            QString::fromUtf8("сто "),
            QString::fromUtf8("двести "),
            QString::fromUtf8("триста "),
            QString::fromUtf8("четыреста "),
            QString::fromUtf8("пятьсот "),
            QString::fromUtf8("шестьсот "),
            QString::fromUtf8("семьсот "),
            QString::fromUtf8("восемьсот "),
            QString::fromUtf8("девятьсот ")
        }
    };

    static QString cap2[10] =
    {
        QString::fromUtf8("десять "),
        QString::fromUtf8("одиннадцать "),
        QString::fromUtf8("двенадцать "),
        QString::fromUtf8("тринадцать "),
        QString::fromUtf8("четырнадцать "),
        QString::fromUtf8("пятнадцать "),
        QString::fromUtf8("шестнадцать "),
        QString::fromUtf8("семнадцать "),
        QString::fromUtf8("восемнадцать "),
        QString::fromUtf8("девятнадцать ")
    };

    static QString cap3[5][3] =
    {
        {
            QString::fromUtf8("копейка "),
            QString::fromUtf8("копейки "),
            QString::fromUtf8("копеек ")
        },
        {
            QString::fromUtf8("рубль, "),
            QString::fromUtf8("рубля, "),
            QString::fromUtf8("рублей, ")
        },
        {
            QString::fromUtf8("тысяча "),
            QString::fromUtf8("тысячи "),
            QString::fromUtf8("тысяч ")
        },
        {
            QString::fromUtf8("миллион "),
            QString::fromUtf8("милиона "),
            QString::fromUtf8("миллионов ")
        },
        {
            QString::fromUtf8("миллиард "),
            QString::fromUtf8("миллиарда "),
            QString::fromUtf8("миллиардов ")
        }
    };

    QString s;

    qlonglong nn = ((qlonglong)(n)*10000 + (qlonglong)(n*1000) % 1000 + 5 ) / 10;

    qlonglong lo = 0;
    qlonglong hi = nn;
    int r[3];
    int rod;

    for ( int nPor = 0; true; ++nPor )
    {
        QString olds = s;
        s = "";

        lo = hi%1000;
        hi = hi/1000;

        if ((nPor == 1) && (lo == 0) && (hi == 0))
        {
            s = QString::fromUtf8("ноль ") + cap3[1][2] + olds;
            break;
        }

        for ( int nRaz = 0; nRaz < 3; ++nRaz )
        {
            r[nRaz] = lo%10;
            lo      = lo/10;
        }

        switch ( nPor )
        {
            case 0:
            case 1:
            case 2:
                rod = 1;
                break;
            default:
                rod = 0;
        }

        QString capt;
        if (r[1] != 1)
        {
            if (r[0] == 1)
                capt = cap3[nPor][0];
            else if ((r[0] > 1 ) && (r[0] < 5))
                capt = cap3[nPor][1];
        }
        if (capt.isEmpty())
            capt = cap3[nPor][2];

        if (nPor == 0)
        {
            s = QString::number(r[1]) + QString::number(r[0]) + " " + capt;
            continue;
        }
        if (nPor == 1)
            rod = 0;

        s += cap[2+1][r[2]];
        s += ((r[1] == 1) ? cap2[r[0]] : cap[1+1][r[1]]+cap[0+rod][r[0]])+capt;
        s += olds;

        if ((hi == 0) && (nPor >= 1))
            break;
    }
    QString up(s[0]);
    s.replace(0,1,up);
    return s;
}

void class_print_pp::slot_print_preview(QPrinter *pr)
{
    painter = new QPainter(pr);

    painter->setPen(Qt::black);

    painter->drawLine(74, 55.5, 203.5, 55.5);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(80, 65, "Поступ. в банк плат.");
    if (query->value(26).toDate().toString("dd.MM.yyyy") != "01.01.1900")
        painter->drawText(100, 50, query->value(26).toDate().toString("dd.MM.yyyy"));

    painter->drawLine(240, 55.5, 371.5, 55.5);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(245, 65, "Списано со сч. плат.");
    if (query->value(13).toDate().toString("dd.MM.yyyy") != "01.01.1900")
        painter->drawText(265, 50, query->value(13).toDate().toString("dd.MM.yyyy"));

    painter->setFont(QFont("Sans",12,0,0));
    painter->drawRect(660, 35, 83, 20.5);
    painter->drawText(666, 50, "0401060");

    painter->setFont(QFont("Sans",10,0,0));
    painter->drawText(76, 120, "ПЛАТЕЖНОЕ ПОРУЧЕНИЕ № " + query->value(5).toString());

    painter->drawLine(400, 120, 529.5, 120);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(445, 130, "Дата");
    painter->setFont(QFont("Sans",10,0,0));
    painter->drawText(425, 115, query->value(7).toDate().toString("dd.MM.yyyy"));

    painter->drawLine(560, 120, 689.5, 120);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(590, 130, "Вид платежа");
    painter->drawText(590, 115, query->value(38).toString());

    painter->drawRect(717, 94, 26, 26);


    painter->drawLine(148, 150, 148, 205);
    painter->drawLine(74, 205, 743, 205);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 160, "Сумма");
    painter->drawText(76, 180, "прописью");
    painter->drawText(160, 150, 500, 30, Qt::TextWordWrap, int_to_string(query->value(8).toDouble()));

    painter->drawLine(444, 205, 444, 538);
    painter->drawLine(500, 205, 500, 538);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 220, "ИНН " + query->value(15).toString());

    painter->setFont(QFont("Sans",8,0,0));
    if (query->value(49).toString() != "0")
    {
        painter->drawText(261, 220, "КПП " + query->value(49).toString());
    }
    else
    {
        painter->drawText(261, 220, "КПП");
    }

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 220, "Сумма");
    if (query->value(8).toString().indexOf(",") > 0 or query->value(8).toString().indexOf(".") > 0)
    {
        painter->drawText(510, 220, query->value(8).toString().replace(",", " - ").replace(".", " - "));
    }
    else
    {
        painter->drawText(510, 220, query->value(8).toString() + " - 00");
    }

    painter->drawLine(74, 227, 444, 227);
    painter->drawLine(259, 205, 259, 227);
    painter->drawLine(444, 260, 743, 260);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 285, "Сч. №");
    painter->drawText(510, 285, query->value(12).toString());

    painter->drawLine(74, 316, 500, 316);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 310, "Плательщик");
    if (query->value(14).toString() != "")
    {
        painter->drawText(76, 230, 360, 70, Qt::TextWordWrap, query->value(14).toString());
    }
    else
    {
        painter->drawText(76, 230, 360, 70, Qt::TextWordWrap, query->value(16).toString());
    }

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 330, "БИК");
    painter->drawText(510, 330, query->value(23).toString());
    painter->drawLine(444, 335, 500, 335);

    painter->drawLine(74, 371, 743, 371);
    painter->drawText(447, 350, "Сч. №");
    painter->drawText(510, 350, query->value(24).toString());
    painter->drawText(76, 365, "Банк плательщика");
    painter->drawText(76, 320, 360, 40, Qt::TextWordWrap, query->value(21).toString());

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 385, "БИК");
    painter->drawText(510, 385, query->value(36).toString());
    painter->drawText(447, 405, "Сч. №");
    painter->drawText(510, 405, query->value(37).toString());
    painter->drawLine(444, 390, 500, 390);

    painter->drawLine(74, 427, 500, 427);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 422, "Банк получателя");
    painter->drawText(76, 375, 360, 40, Qt::TextWordWrap, query->value(34).toString());

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 442, "ИНН " + query->value(28).toString());

    painter->setFont(QFont("Sans",8,0,0));
    if (query->value(50).toString() != "0")
    {
        painter->drawText(261, 442, "КПП " + query->value(50).toString());
    }
    else
    {
        painter->drawText(261, 442, "КПП");
    }

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(447, 442, "Сч. №");
    painter->drawText(510, 442, query->value(37).toString());

    painter->drawLine(74, 449, 444, 449);
    painter->drawLine(259, 427, 259, 449);
    painter->drawLine(444, 482, 743, 482);

    painter->drawLine(650, 482, 650, 538);
    painter->drawLine(576, 482, 576, 538);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 496, "Вид оп.");
    painter->drawText(510, 496, query->value(39).toString());
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 496, "Срок плат.");
    if (query->value(61).toDate().toString("dd.MM.yyyy") != "01.01.1900")
        painter->drawText(657, 496, query->value(61).toDate().toString("dd.MM.yyyy"));
    painter->drawLine(444, 501, 500, 501);
    painter->drawLine(576, 501, 650, 501);

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 514, "Наз. пл.");
    //painter->drawText(510, 514, query->value(39).toString());
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 514, "Очер. плат.");
    painter->drawText(657, 514, query->value(58).toString());
    painter->drawLine(444, 519, 500, 519);
    painter->drawLine(576, 519, 650, 519);
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(446, 531, "Код");
    painter->drawText(510, 531, query->value(40).toString());
    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(578, 531, "Рез поле.");

    painter->setFont(QFont("Sans",8,0,0));
    painter->drawText(76, 533, "Получатель");
    if (query->value(27).toString() != "")
    {
        painter->drawText(76, 452, 360, 70, Qt::TextWordWrap, query->value(27).toString());
    }
    else
    {
        painter->drawText(76, 452, 360, 70, Qt::TextWordWrap, query->value(29).toString());
    }
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
    painter->drawText(76, 565, 670, 70, Qt::TextWordWrap, query->value(41).toString());
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
