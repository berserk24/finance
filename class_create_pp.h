#ifndef CLASS_CREATE_PP_H
#define CLASS_CREATE_PP_H

#include <QWidget>

namespace Ui {
class class_create_pp;
}

class class_create_pp : public QWidget
{
    Q_OBJECT
public:
    explicit class_create_pp(QWidget *parent = 0);

signals:

public slots:
    void slot_show_info(bool state);
    void slot_show_payer(bool state);
    void slot_show_receiver(bool state);
    void slot_show_dest_pay(bool state);
    void slot_show_budget(bool state);

private:
    Ui::class_create_pp *ui;

};

#endif // CLASS_CREATE_PP_H
