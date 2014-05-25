#include "class_update_ref_banks.h"
#include "ui_class_update_ref_banks.h"

class_update_ref_banks::class_update_ref_banks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_update_ref_banks)
{
    ui->setupUi(this);
    set_last_update();
    this->show();

    connect(ui->pushButton, SIGNAL(clicked()), SLOT(update_ref()));
}

void class_update_ref_banks::set_last_update()
{
    query = new QSqlQuery;
    query->exec("SELECT data FROM update_date WHERE id = 'bik_update'");
    query->first();
    ui->label->setText("Последнее обновление:  " + query->value(0).toDate().toString("dd.MM.yyyy"));
    if (query->value(0).toDate() == QDate::currentDate()) ui->pushButton->setEnabled(false);
    query->clear();
}

void class_update_ref_banks::update_ref()
{
    download_file();
}

bool class_update_ref_banks::download_file()
{
    //bool t;
    download_update = new DownloadManager;
    QTimer::singleShot(0, download_update, SLOT(execute()));
    connect(download_update, SIGNAL(signal_end_load(QString)), SLOT(slot_unzip_file(QString)));
    //delete download_update;
    return true;
}

void class_update_ref_banks::slot_unzip_file(QString file)
{
    zip = new QZipReader(file);
    if (zip->exists())
    {
        // распаковка архива по указанному пути
        QByteArray data = zip->fileData("BNKSEEK.DBF");
        QFile file_db(QApplication::applicationDirPath() + "/BNKSEEK.DBF");
        file_db.open(QIODevice::WriteOnly);
        file_db.write(data);
        file_db.close();
        update_reference_bik();
        file_db.remove();
        QFile file_arch(file);
        file_arch.remove();
        set_last_update();
    }
}

void class_update_ref_banks::update_reference_bik()
{
    QDbf::QDbfTable table;
    if (!table.open("BNKSEEK.DBF")) {
        qDebug() << "file open error";
        return;
    }

    int j = 0;

    query = new QSqlQuery;

    query->exec("SELECT data FROM update_date WHERE id = 'bik_update'");
    query->first();

    if (query->value(0).toDate() < QDate::currentDate())
    {
        query->clear();
        query->exec("DELETE FROM ref_banks");

        while (table.next()) {
            query->prepare("INSERT INTO ref_banks (id, index, sity, address, name, bik, phone, okpo, ks) "
                           "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
            query->addBindValue(j);
            QDbf::QDbfRecord record = table.record();
            for (int i = 0; i < record.count(); ++i) {
                if (i == 5 or i == 7 or i == 8  or i == 10 or i == 12 or i == 18 or i == 20 or i == 23)
                    query->addBindValue(record.value(i).toString().trimmed());
            }
            query->exec();
            query->clear();
            j++;
        }
        query->exec("UPDATE update_date SET data = now()::date WHERE id = 'bik_update'");
    }
    delete query;

}

class_update_ref_banks::~class_update_ref_banks()
{
    delete ui;
}
