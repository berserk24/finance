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
    download_update = new DownloadManager;
    QTimer::singleShot(0, download_update, SLOT(execute()));
    return true;
}

class_update_ref_banks::~class_update_ref_banks()
{
    delete ui;
}
