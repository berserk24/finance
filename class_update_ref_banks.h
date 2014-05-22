#ifndef CLASS_UPDATE_REF_BANKS_H
#define CLASS_UPDATE_REF_BANKS_H

#include <QWidget>
#include <QTimer>
#include <QSqlDatabase>
#include <QTextCodec>
#include "qdbftable.h"
#include "qdbfrecord.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "downloadmanager.h"
#include "private/qzipreader_p.h"

namespace Ui {
class class_update_ref_banks;
}

class class_update_ref_banks : public QWidget
{
    Q_OBJECT

public:
    explicit class_update_ref_banks(QWidget *parent = 0);
    ~class_update_ref_banks();

public slots:
    bool download_file();
    void update_ref();
    void slot_unzip_file(QString);
    void update_reference_bik();

private:
    Ui::class_update_ref_banks *ui;
    DownloadManager *download_update;
    QZipReader *zip;
};

#endif // CLASS_UPDATE_REF_BANKS_H
