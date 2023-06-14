#include "successdialog.h"
#include "ui_successdialog.h"

SuccessDialog::SuccessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SuccessDialog)
{
    ui->setupUi(this);
}

SuccessDialog::~SuccessDialog()
{
    delete ui;
}
