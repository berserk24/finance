#include "update_client_balans.h"

update_client_balans::update_client_balans(QObject *parent, QSqlDatabase *db1) :
    QObject(parent)
{
    db = db1;
    query = new QSqlQuery;
}

//Обновляем баланс клиента
void update_client_balans::slot_update_balans(QString id, QString type, QString summ, QString pp_id, QString about, QString margin_text, QString to_client_id)
{
    //double balans;
    double margin = 1, sum_margin = 0;
    double m_obn = 0, m_nal = 0, m_trans_out = 0, m_trans_in = 0, m_trans_in_s = 0;
    QString stroy = "";
    int status = 0, st = 0;

    db->transaction();
    //Получаем комиссию клиента
    {
        query->prepare("SELECT t_obnal, t_nalic, t_trans_in, t_trans_in_s, t_trans_out "
                           "FROM clients "
                           "WHERE id = ?");
        query->addBindValue(id);
        if (query->exec()) status++;
        st++;
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
            if (margin_text == "")
            {
                margin = 1 + m_nal/100;
            }
            else
            {
                margin = 1 + margin_text.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            if (about == "") about = "Пополнение счёта наличными";
        }
        if (type == "Расход")
        {
            if (margin_text == "")
            {
                margin = 1 - m_obn/100;
            }
            else
            {
                margin = 1 - margin_text.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()/margin - summ.replace(",", ".").toDouble();
            if (about == "") about = "Выдача наличных со счёта";
        }
        if (type == "Перевод")
        {
            if (margin_text == "")
            {
                margin = 1;
            }
            else
            {
                margin = 1 + margin_text.replace(",", ".").toDouble()/100;
            }
            sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            if (about == "")
            {
                query->prepare("SELECT name FROM clients WHERE id = ?");
                query->addBindValue(to_client_id);
                if (query->exec()) status++;
                st++;
                query->first();
                about = "Перевод клиенту " + query->value(0).toString();
                query->clear();
            }
        }
    }
    else
    {
        //Коммисия за транзит
        if (margin_text == "")
        {
            query->prepare("SELECT firms.stroy FROM pp "
                           "LEFT JOIN rss ON pp.rs_id = rss.id "
                           "LEFT JOIN firms ON firms.id = rss.firm "
                           "WHERE pp.id = ?");
            query->addBindValue(pp_id);
            if (query->exec()) status++;
            st++;
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
                if (stroy == "0")
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
                margin = 1 + margin_text.replace(",", ".").toDouble()/100;
                sum_margin = summ.replace(",", ".").toDouble()*margin - summ.replace(",", ".").toDouble();
            }
            if (type == "Приход")
            {
                if (stroy == "0")
                {
                    margin = 1 - margin_text.replace(",", ".").toDouble()/100;
                }
                else
                {
                    margin = 1 - margin_text.replace(",", ".").toDouble()/100;
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
        if (query->exec()) status++;
        st++;
        query->first();
        about = query->value(0).toString();
        query->clear();
    }

    //Обновляем баланс клиента
    if (type == "Приход")
    {
        query->prepare("UPDATE client_balans SET balans = (balans + ?) WHERE id = ?");
        query->addBindValue(summ.replace(",", ".").toDouble()*margin);
        query->addBindValue(id);
        if (query->exec()) status++;
        st++;
        query->clear();
    }
    if (type == "Расход")
    {
        query->prepare("UPDATE client_balans SET balans = (balans - ?) WHERE id = ?");
        if (pp_id == "")
        {
            query->addBindValue(summ.replace(",", ".").toDouble()/margin);
        }
        else
        {
            query->addBindValue(summ.replace(",", ".").toDouble()*margin);
        }
        query->addBindValue(id);
        if (query->exec()) status++;
        st++;
        query->clear();
    }
    if (type == "Перевод")
    {
        //Списываем с клиента
        query->prepare("UPDATE client_balans SET balans = (balans - ?) WHERE id = ?");
        query->addBindValue(summ.replace(",", ".").toDouble()*margin);
        query->addBindValue(id);
        if (query->exec()) status++;
        st++;
        query->clear();

        //Пополняем баланс второго клиента
        query->prepare("UPDATE client_balans SET balans = (balans + ?) WHERE id = ?");
        query->addBindValue(summ.replace(",", ".").toDouble());
        query->addBindValue(to_client_id);
        if (query->exec()) status++;
        st++;
        query->clear();
    }

    //Записываем платёжку на клиента
    if (pp_id != "")
    {
        query->prepare("INSERT INTO pp_to_client (pp_id, client_id) VALUES(?, ?)");
        query->addBindValue(pp_id);
        query->addBindValue(id);
        if (query->exec()) status++;
        st++;
        query->clear();
    }

    //Обновляем баланс счёта хозяина
    if (margin != 1)
    {
        query->prepare("UPDATE client_balans SET balans = (balans + ?) WHERE id = 0");
        if (type == "Приход")
        {
            if (pp_id == "")
            {
                query->addBindValue(-sum_margin);
            }
            else
            {
                query->addBindValue(sum_margin);
            }
        }
        if (type == "Расход")
        {
            query->addBindValue(sum_margin);
        }
        if (type == "Перевод")
        {
            query->addBindValue(sum_margin);
        }
        if (query->exec()) status++;
        st++;
    }

    //Записываем лог
    {
        query->prepare("INSERT INTO clients_operations (id_client, id_pp, date_oper, summ, type_pp, to_client_id, margin, text) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        query->addBindValue(id);
        query->addBindValue(pp_id);
        query->addBindValue(QDate::currentDate());
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
        query->addBindValue(sum_margin);
        query->addBindValue(about);
        if (query->exec()) status++;
        st++;
        query->clear();
    }
    if (status == st)
    {
        db->commit();
    }
    else
    {
        db->rollback();
    }
}

void update_client_balans::slot_cancel_pay(QString client_id, QString pp_id, QString type, QString summ, QString margin, QString oper_id, QString to_client_id)
{
    double balans;

    //Удаляем привязку платёжки к клиенту
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
}
