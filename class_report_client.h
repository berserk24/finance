#ifndef CLASS_REPORT_CLIENT_H
#define CLASS_REPORT_CLIENT_H

#include <QWidget>
#include <QSqlQuery>
#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QFile>
#include <QFileDialog>
#include <QSqlError>
#include <QDebug>

namespace Ui {
class class_report_client;
}

class class_report_client : public QWidget
{
    Q_OBJECT

public:
    explicit class_report_client(QWidget *parent = 0);
    ~class_report_client();

public slots:
    void slot_select_client();
    void slot_gen_report();
    void slot_print_save();
    void slot_print_preview(QPrinter*);
    void slot_save_report();

private:
    QSqlQuery *query;
    QPrinter *printer;
    QPainter *painter;
    QPrintPreviewDialog *preview;
    QFileDialog *open_dialog;
    QFile *file;
    Ui::class_report_client *ui;
};

#endif // CLASS_REPORT_CLIENT_H
