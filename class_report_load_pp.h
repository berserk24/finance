#ifndef CLASS_REPORT_LOAD_PP_H
#define CLASS_REPORT_LOAD_PP_H

#include <QWidget>

namespace Ui {
class class_report_load_pp;
}

class class_report_load_pp : public QWidget
{
    Q_OBJECT

public:
    explicit class_report_load_pp(QWidget *parent = 0);
    void slot_set_text(QString);
    ~class_report_load_pp();

private slots:

private:
    Ui::class_report_load_pp *ui;
};

#endif // CLASS_REPORT_LOAD_PP_H
