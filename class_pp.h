#ifndef CLASS_PP_H
#define CLASS_PP_H
#include <QString>
#include <QDate>
#include <QTime>

class class_pp
{
public:
    class_pp();
    QString rs_id, client_id, type, type_doc, num, sum, ticket_value, payer_count;
    QString payer, payer_inn, payer1, payer2, payer3, payer4, payer_rs, payer_bank1;
    QString payer_bank2, payer_bik, payer_ks, receiver_count, receiver, receiver_inn, receiver1, receiver2, receiver3;
    QString receiver4, receiver_rs, receiver_bank1, receiver_bank2, receiver_bik, receiver_ks, type_pay;
    QString type_trans, code, dest_pay, dest_pay1, dest_pay2, dest_pay3, dest_pay4, dest_pay5, dest_pay6;
    QString state_sender, payer_kpp, receiver_kpp, pokazatel_kbk, okato, pokazatel_osnovaniya, pokazatel_period;
    QString pokazatel_num, pokazatel_type, ocherednost, srok_accepta, type_akkred;
    QString usl_pay1, usl_pay2, usl_pay3, pay_po_predst, dop_usl, num_scheta_postav;
    QDate date_in, date_out, date, date_oper, ticket_date, pokazatel_date, date_send_doc, srok_pay;
    QTime ticket_time;
};

#endif // CLASS_PP_H
