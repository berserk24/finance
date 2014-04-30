#ifndef CLASS_REF_PP_H
#define CLASS_REF_PP_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QValidator>
#include <QVariant>
#include <QMessageBox>
#include <QHeaderView>
#include "update_client_balans.h"
#include "class_print_pp.h"
#include "class_setings_table.h"
#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QTextBrowser>
#include <QPrintDialog>
#include <QFile>
#include <QSettings>
#include <QDebug>

namespace Ui {
class class_ref_pp;
}

class class_ref_pp : public QWidget
{
    Q_OBJECT

public:
    explicit class_ref_pp(QWidget *parent, QSqlDatabase *db1);
    ~class_ref_pp();

public slots:
    void slot_select_pp();
    void slot_enable_button();
    void slot_select_client();
    void slot_select_client_filter();
    void slot_pp_to_client();
    void slot_clear_form();
    void slot_set_margin();
    void slot_cancel_pp();
    void slot_print_pp(QString str);
    void slot_sort_pp(int sort_id);
    void load_rss();
    void slot_send_show_ref_client();
    void slot_show_print_registr(QString str);
    void slot_print_preview(QPrinter*);
    void slot_save_register();
    void slot_get_access();
    void slot_show_settings_table();
    void slot_write_settings_view();
    void slot_set_settings_header(QList<bool>);
    void slot_sum_balans_rs();

private:
    Ui::class_ref_pp *ui;

    QSqlQuery *query;
    QSqlQueryModel *model;
    update_client_balans *ucb;
    class_print_pp *print_pp;
    int id_column, client_id;
    bool access;
    QPrinter *printer;
    QPainter *painter;
    QPrintPreviewDialog *preview;
    QFileDialog *open_dialog;
    QFile *file;
    class_setings_table *settings_table;
    QSettings *settings;
    QHeaderView *pH;
    QSqlDatabase *db;
    QString get_settings();

    bool str_to_bool(QString);

signals:
    void show_ref_client();
    void signal_send_sum_pp(QString);
};

#endif // CLASS_REF_PP_H
