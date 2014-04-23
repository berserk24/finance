#include "update_client_balans.h"

update_client_balans::update_client_balans(QObject *parent) :
    QObject(parent)
{
    query = new QSqlQuery;
}

//Обновляем баланс клиента
void update_client_balans::slot_update_balans(QString id, QString type, QString summ, QString pp_id, bool nal, QString about, bool no_margin, QString margin_, QString to_client_id)
{
    double balans, margin = 1, sum_margin = 0;
    double m_obn = 0, m_nal = 0, m_trans_out = 0, m_trans_in = 0, m_trans_in_s = 0;
    QString stroy = "";
    //qDebug() << "defcefecececec" << pp_id << endl;

    query->exec("BEGIN IMMEDIATE TRANSACTION");
    //Получаем баланс клиента
    {
        query->prepare("SELECT balans FROM client_balans WHERE id = ?");
        query->addBindValue(id);
        query->exec();
        query->first();
        balans = query->value(0).toDouble();
        query->clear();
    }

    //Получаем комиссию клиента
    {
        query->prepare("SELECT t_obnal, t_nalic, t_trans_in, t_trans_in_s, t_trans_out "
                           "FROM client "
                           "WHERE id = ?");
        query->addBindValue(id);
        query->exec();
        query->first();
        m_obn = query->value(0).toInt();
        m_nal = query->value(1).toInt();
        m_trans_in = query->value(2).toInt();
        m_trans_in_s = query->value(3).toInt();
        m_trans_out = query->value(4).toInt();
        query->clear();
    }

    if (pp_id == "")
    {
        if (type == "Приход")
        {
            if (margin_ == "")
            {
                margin = 1 + m_nal/100;
            }
            else
            {
                margin = 1 + margin_.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            if (about == "") about = "Пополнение счёта наличными";
        }
        if (type == "Расход")
        {
            if (margin_ == "")
            {
                margin = 1 - m_obn/100;
            }
            else
            {
                margin = 1 - margin_.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()/margin - summ.replace(",", ".").toDouble();
            if (about == "") about = "Выдача наличных со счёта";
        }
        if (type == "Перевод")
        {
            if (margin_ == "")
            {
                margin = 1;
            }
            else
            {
                margin = 1 + margin_.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            if (about == "")
            {
                query->prepare("SELECT name FROM client WHERE id = ?");
                query->addBindValue(to_client_id);
                query->exec();
                query->first();
                about = "Перевод клиенту " + query->value(0).toString();
                query->clear();
            }
        }
    }
    else
    {
        //Коммисия за транзит
        if (margin_ == "")
        {
            query->prepare("SELECT firm.stroy FROM pp "
                           "LEFT JOIN rss ON pp.rs_id = rss.id "
                           "LEFT JOIN firm ON firm.id = rss.firm "
                           "WHERE pp.id = ?");
            query->addBindValue(pp_id);
            query->exec();
            query->first();
            stroy = query->value(0).toString();
            query->clear();
            if (type == "Расход")
            {
                margin = 1 + m_trans_out/100;
                sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            }
            if (type == "Приход")
            {
                if (stroy == "false")
                {
                    margin = 1 - m_trans_in/100;
                }
                else
                {
                    margin = 1 - m_trans_in_s/100;
                }
                sum_margin = summ.replace(",", ".").toDouble() - summ.replace(",", ".").toDouble()*margin;
            }
        }
        else
        {
            if (type == "Расход")
            {
                margin = 1 + margin_.replace(",", ".").toDouble()/100;
                sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            }
            if (type == "Приход")
            {
                if (stroy == "false")
                {
                    margin = 1 - margin_.replace(",", ".").toDouble()/100;
                }
                else
                {
                    margin = 1 - margin_.replace(",", ".").toDouble()/100;
                }
                sum_margin = summ.replace(",", ".").toDouble() - summ.replace(",", ".").toDouble()*margin;
            }
        }
    }

    //Добавляем коментарий к платежу из платёжки
    if (pp_id != "")
    {
        query->prepare("SELECT dest_pay FROM pp WHERE id = ?");
        query->addBindValue(pp_id);
        query->exec();
        query->first();
        about = query->value(0).toString();
        query->clear();
    }

    //Обновляем баланс клиента
    if (type == "Приход")
    {
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
        query->addBindValue(QString::number(balans + summ.replace(",", ".").toDouble()*margin, 'f', 2));
        query->addBindValue(id);
        query->exec();
        query->clear();
    }
    if (type == "Расход")
    {
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
        if (pp_id == "")
        {
            query->addBindValue(QString::number(balans - summ.replace(",", ".").toDouble()/margin, 'f', 2));
        }
        else
        {
            query->addBindValue(QString::number(balans - summ.replace(",", ".").toDouble()*margin, 'f', 2));
        }
        query->addBindValue(id);
        query->exec();
        query->clear();
    }
    if (type == "Перевод")
    {
        //Списываем с клиента
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
        query->addBindValue(QString::number(balans - summ.replace(",", ".").toDouble()*margin, 'f', 2));
        query->addBindValue(id);
        query->exec();
        query->clear();

        //Получаем баланс второго клиента
        query->prepare("SELECT balans FROM client_balans WHERE id = ?");
        query->addBindValue(to_client_id);
        query->exec();
        query->first();
        balans = query->value(0).toDouble();
        query->clear();

        //Пополняем баланс второго клиента
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
        query->addBindValue(QString::number(balans + summ.replace(",", ".").toDouble(), 'f', 2));
        query->addBindValue(to_client_id);
        query->exec();
        query->clear();
    }

    //Записываем платёжку на клиента
    if (pp_id != "")
    {
        query->prepare("INSERT INTO pp_to_client (pp_id, client_id) VALUES(?, ?)");
        query->addBindValue(pp_id);
        query->addBindValue(id);
        query->exec();
        query->clear();
    }

    //Обновляем баланс счёта хозяина
    if (margin != 1)
    {
        query->exec("SELECT balans FROM client_balans WHERE id = 0");
        query->first();
        balans = query->value(0).toDouble();
        query->clear();
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = 0");
        if (type == "Приход")
        {
            if (pp_id == "")
            {
                query->addBindValue(QString::number(balans - sum_margin, 'f', 2));
            }
            else
            {
                query->addBindValue(QString::number(balans + sum_margin, 'f', 2));
            }
        }
        if (type == "Расход")
        {
            query->addBindValue(QString::number(balans + sum_margin, 'f', 2));
        }
        if (type == "Перевод")
        {
            query->addBindValue(QString::number(balans + sum_margin, 'f', 2));
        }
        query->exec();
    }

    //Записываем лог
    {
        query->prepare("INSERT INTO client_operations (id_client, id_pp, date, summ, type, to_client_id, margin, text) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        query->addBindValue(id);
        query->addBindValue(pp_id);
        query->addBindValue(QString::number(QDate::currentDate().toJulianDay()));
        query->addBindValue(summ);
        if (type == "Расход")
        {
            query->addBindValue(1);
            query->addBindValue("");
        }
        if (type == "Приход")
        {
            query->addBindValue(2);
            query->addBindValue("");
        }
        if (type == "Перевод")
        {
            query->addBindValue(3);
            query->addBindValue(to_client_id);
        }
        query->addBindValue(QString::number(sum_margin, 'f', 2));
        query->addBindValue(about);
        query->exec();
        query->clear();
    }
    query->exec("COMMIT");
}

void update_client_balans::slot_cancel_pay(QString client_id, QString pp_id, QString type, QString summ, QString margin, QString oper_id, QString to_client_id)
{
    double balans;

    //Удаляем привязку платёжки к клиенту
    query->exec("BEGIN IMMEDIATE TRANSACTION");
    if (pp_id != "")
    {
        query->prepare("DELETE FROM pp_to_client WHERE pp_id = ? AND client_id = ?");
        query->addBindValue(pp_id);
        query->addBindValue(client_id);
        query->exec();
        query->clear();
    }

    //Обновляем свой счёт
    if (margin.replace(",", ".").toDouble() != 0)
    {
        query->exec("SELECT balans FROM client_balans WHERE id = 0");
        query->first();
        balans = query->value(0).toDouble();
        query->clear();
        query->prepare("UPDATE client_balans SET balans = ? WHERE id = 0");
        if (type == "Приход")
        {
            if (pp_id == "")
            {
                query->addBindValue(QString::number(balans + margin.replace(",", ".").toDouble(), 'f', 2));
            }
            else
            {
                query->addBindValue(QString::number(balans - margin.replace(",", ".").toDouble(), 'f', 2));
            }
        }
        else
        {
            query->addBindValue(QString::number(balans - margin.replace(",", ".").toDouble(), 'f', 2));
        }
        query->exec();
    }

    //Обновляем счёт клиента
    {
        //Получаем баланс клиента
        {
            query->prepare("SELECT balans FROM client_balans WHERE id = ?");
            query->addBindValue(client_id);
            query->exec();
            query->first();
            balans = query->value(0).toDouble();
            query->clear();
        }

        if (type == "Приход")
        {
            query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
            if (pp_id == "")
            {
                query->addBindValue(QString::number(balans - (summ.replace(",", ".").toDouble() + margin.replace(",", ".").toDouble()), 'f', 2));
            }
            else
            {
                query->addBindValue(QString::number(balans - (summ.replace(",", ".").toDouble() - margin.replace(",", ".").toDouble()), 'f', 2));
            }
            query->addBindValue(client_id);
            query->exec();
            query->clear();
        }
        if (type == "Расход")
        {
            query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
            query->addBindValue(QString::number(balans + (summ.replace(",", ".").toDouble() + margin.replace(",", ".").toDouble()), 'f', 2));
            query->addBindValue(client_id);
            query->exec();
            query->clear();
        }
        if (type == "Перевод")
        {
            query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
            query->addBindValue(QString::number(balans + summ.replace(",", ".").toDouble() + margin.replace(",", ".").toDouble(), 'f', 2));
            query->addBindValue(client_id);
            query->exec();
            query->clear();

            query->prepare("SELECT balans FROM client_balans WHERE id = ?");
            query->addBindValue(to_client_id);
            query->exec();
            query->first();
            balans = query->value(0).toDouble();
            query->clear();

            query->prepare("UPDATE client_balans SET balans = ? WHERE id = ?");
            query->addBindValue(QString::number(balans - summ.replace(",", ".").toDouble(), 'f', 2));
            query->addBindValue(to_client_id);
            query->exec();
            query->clear();
        }
    }

    //Удаляем запись из лога
    {
        query->prepare("DELETE FROM client_operations WHERE id = ?");
        query->addBindValue(oper_id);
        query->exec();
        query->clear();
    }
    query->exec("COMMIT");
}
