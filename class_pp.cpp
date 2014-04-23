#include "class_pp.h"

class_pp::class_pp()
{
    rs_id = client_id = type = type_doc = num = date = date_oper = sum = ticket_date = ticket_time = ticket_value = payer_count = payer = payer_inn = QString::null;
    payer1 = payer2 = payer3 = payer4 = payer_rs = payer_bank1 = payer_bank2 = payer_bik = payer_ks = QString::null;
    receiver_count = receiver = receiver_inn = receiver1 = receiver2 = receiver3 = receiver4 = QString::null;
    receiver_rs = receiver_bank1 = receiver_bank2 = receiver_bik = receiver_ks = type_pay = type_trans = QString::null;
    code = dest_pay = dest_pay1 = dest_pay2 = dest_pay3 = dest_pay4 = dest_pay5 = dest_pay6 = state_sender = QString::null;
    payer_kpp = receiver_kpp = pokazatel_kbk = okato = pokazatel_osnovaniya = pokazatel_period = pokazatel_num = QString::null;
    pokazatel_date = pokazatel_type = ocherednost = srok_accepta = type_akkred = srok_pay = usl_pay1 = usl_pay2 = QString::null;
    usl_pay3 = pay_po_predst = dop_usl = num_scheta_postav = date_send_doc = "";
}
