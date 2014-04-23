#ifndef UPDATE_CLIENT_BALANS_H
#define UPDATE_CLIENT_BALANS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>
#include <QSqlError>
#include <QDebug>

class update_client_balans : public QObject
{
    Q_OBJECT
public:
    explicit update_client_balans(QObject *parent = 0);

signals:

public slots:
    void slot_update_balans(QString id, QString type, QString summ, QString pp_id, bool nal, QString about, bool no_margin, QString margin_, QString to_client_id);
    void slot_cancel_pay(QString client_id, QString pp_id, QString type, QString summ, QString margin, QString oper_id, QString to_client_id);
    //void slot_write_log();

private:
    QSqlQuery *query;

};

#endif // UPDATE_CLIENT_BALANS_H
