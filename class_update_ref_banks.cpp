#include "class_update_ref_banks.h"
#include "ui_class_update_ref_banks.h"

class_update_ref_banks::class_update_ref_banks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::class_update_ref_banks)
{
    ui->setupUi(this);
    this->show();

    connect(ui->pushButton, SIGNAL(clicked()), SLOT(update_ref()));
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
        QFile file(QApplication::applicationDirPath() + "/BNKSEEK.DBF");
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
        update_reference_bik();
    }
}

void class_update_ref_banks::update_reference_bik()
{
    QDbf::QDbfTable table;
    if (!table.open("BNKSEEK.DBF")) {
        qDebug() << "file open error";
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    while (table.next()) {
        QString output;
        QDbf::QDbfRecord record = table.record();
        for (int i = 0; i < record.count(); ++i) {
            output.append(record.fieldName(i));
            output.append(QLatin1String(": "));
            output.append(codec->toUnicode(record.value(i).toByteArray()).trimmed());
            output.append(QLatin1String("; "));
        }
        qDebug() << output;
    }

}

class_update_ref_banks::~class_update_ref_banks()
{
    delete ui;
}
