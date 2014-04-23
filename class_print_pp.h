#ifndef CLASS_PRINT_PP_H
#define CLASS_PRINT_PP_H

#include <QWidget>
#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QSqlQuery>
#include <QVariant>
#include <QDate>
#include <QDebug>

class class_print_pp : public QObject
{
    Q_OBJECT
public:
    explicit class_print_pp(QObject *parent, int id_pp, QString str);

private:
    QPrinter *printer;
    QPainter *painter;
    QPrintPreviewDialog *preview;
    QFileDialog *open_dialog;
    QSqlQuery *query;

signals:

public slots:
    void slot_print_preview(QPrinter *pr);

};

#endif // CLASS_PRINT_PP_H
