#ifndef CLASS_UPDATE_REF_BANKS_H
#define CLASS_UPDATE_REF_BANKS_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include "downloadmanager.h"

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

private:
    Ui::class_update_ref_banks *ui;
    DownloadManager *download_update;
};

#endif // CLASS_UPDATE_REF_BANKS_H
