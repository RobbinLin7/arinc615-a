#include "progressdialog.h"
#include "ui_progressdialog.h"
#include "progresswidget.h"
#include <QListWidgetItem>
#include <QPushButton>
#include <QDebug>

ProgressDialog::ProgressDialog(QStringList fileList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    for(int i = 0; i < fileList.size(); ++i){
        ProgressWidget* progressWidget = new ProgressWidget(fileList[i].mid(fileList[i].lastIndexOf('/') + 1), this);
        QListWidgetItem* widgetItem = new QListWidgetItem();
        ui->listWidget->addItem(widgetItem);
        ui->listWidget->setItemWidget(widgetItem, progressWidget);
        widgetItem->setSizeHint(QSize(600, 70));
    }
}

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    //只有一个进度条:下载总进度
    ProgressWidget* progressWidget = new ProgressWidget("下载总进度", this);
    QListWidgetItem* widgetItem = new QListWidgetItem();
    ui->listWidget->addItem(widgetItem);
    ui->listWidget->setItemWidget(widgetItem, progressWidget);
    widgetItem->setSizeHint(QSize(600,200));
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(Hfile_info_LUS *hfiles, int num)
{
    int count = 0;
    for(int i = 0; i < num; ++i){
        unsigned progress = 0;
        for(int j = 0; j < 3; ++j){
            progress = progress * 10 + hfiles[i].load_ratio[j] - '0';
        }
        if(progress == 100) ++count;
        for(int j = 0; j < ui->listWidget->count(); ++j){
            ProgressWidget* progressWidget = (ProgressWidget*)ui->listWidget->itemWidget(ui->listWidget->item(j));
            if(progressWidget->getFileName().toStdString() == std::string(hfiles[i].Hfile_name)){
                progressWidget->updateProgressBar(progress);
                progressWidget->updateStatus(hfiles[i].load_stat);
                break;
            }
        }
    }
    if(count == ui->listWidget->count()) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ProgressDialog::setProgress(int progress, char* stat)
{

    ProgressWidget* progressWidget = (ProgressWidget*)ui->listWidget->itemWidget(ui->listWidget->item(1));
    progressWidget->updateProgressBar(progress);
    progressWidget->updateStatus(*stat);
    if(progress == 100) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

}
