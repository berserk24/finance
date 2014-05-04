#ifndef CLASS_LOAD_PP_H
#define CLASS_LOAD_PP_H

#include <QWidget>
#include <QSqlDatabase>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDate>
#include <QSettings>
#include "class_pp.h"
#include "update_client_balans.h"
#include "class_report_load_pp.h"

namespace Ui {
class class_load_pp;
}

class class_load_pp : public QWidget
{
    Q_OBJECT

public:
    explicit class_load_pp(QWidget *parent, QSqlDatabase *db1);
    ~class_load_pp();

public slots:
    void slot_open_file_dialog();
    void slot_set_enable_load();
    void slot_load_pp();
    void slot_load_pp_db();

private:
    Ui::class_load_pp *ui;
    QFileDialog *open_file;
    QFile *file_pp;
    QString *file_str;
    QList<QString> codepage;
    QString date_start, date_end, rs, start_balans, all_in, all_out, end_balans, bik, inn, firm_name;
    int pp_count, date_balans;
    QSqlQuery *query;
    class_pp *pp;
    QVector<class_pp> *vector_pp;
    QString summ, balans;
    double summ_in, summ_out;
    update_client_balans *ucb;
    QMessageBox *message_pp_type;
    QString report;
    int pp_in_count, pp_out_count;
    class_report_load_pp *report_pp;
    QSqlDatabase *db;
    QProgressDialog *pr_dialog;

    QString set_codec(QTextStream *);
    QString get_bik(QTextStream *);
    QString get_id_rs(QString rs, QString bik);
    QString get_rs_id(QString id);
    QString get_firm_id(QString inn);
    bool verify_pp(QString);
    void load_pp(QString);
    void clear_window();
    void set_path_to_pp();

};

#endif // CLASS_LOAD_PP_H
