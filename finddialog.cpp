#include "finddialog.h"
#include "ui_finddialog.h"
#include <QPushButton>

FindDialog::FindDialog(QWidget *parent, QList<QNetworkAddressEntry>* entryList) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setToolTip(QString(tr("确定")));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setToolTip(QString(tr("取消")));
    int cnt = 0;
    for(QNetworkAddressEntry entry: *entryList){
        ui->comboBox->insertItem(cnt++, entry.ip().toString());
    }
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this](){
        emit on_accpetBtn_clicked(ui->comboBox->currentIndex());
    });
}

bool FindDialog::updateEntryList(QList<QNetworkAddressEntry> *entryList)
{
    if(entryList->size() != ui->comboBox->count()){
        ui->comboBox->clear();
        int cnt = 0;
        for(QNetworkAddressEntry entry: *entryList){
            ui->comboBox->insertItem(cnt++, entry.ip().toString());
        }
    }
}

FindDialog::~FindDialog()
{
    delete ui;
}
