#include "odownloaddialog.h"
#include "ui_odownloaddialog.h"

ODownloadDialog::ODownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ODownloadDialog)
{
    ui->setupUi(this);
}

ODownloadDialog::~ODownloadDialog()
{
    delete ui;
}
