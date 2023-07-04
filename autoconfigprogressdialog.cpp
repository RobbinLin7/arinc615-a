#include "autoconfigprogressdialog.h"
#include "ui_autoconfigprogressdialog.h"
#include "operationInterface/autoconfigwidget.h"

AutoConfigProgressDialog::AutoConfigProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoConfigProgressDialog)
{
    ui->setupUi(this);
    QProgressBar* progressBar1 = new QProgressBar(this);
    QProgressBar* progressBar2 = new QProgressBar(this);
    ui->verticalLayout->addWidget(progressBar1);
    ui->verticalLayout->addWidget(progressBar2);
}

AutoConfigProgressDialog::~AutoConfigProgressDialog()
{
    delete ui;
}

void AutoConfigProgressDialog::addProgressBar(AutoConfigThread *autoConfigThread)
{
    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    QProgressBar* progressBar = new QProgressBar(this);
    horizontalLayout->addWidget(new QLabel(autoConfigThread->getHostAddress().toString(), this));
    horizontalLayout->addWidget(progressBar);
    ui->verticalLayout->addLayout(horizontalLayout);
}
