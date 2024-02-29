#include "progressdialog.h"
#include "ui_progressdialog.h"
#include "progresswidget.h"
#include <QListWidgetItem>
#include <QDebug>

ProgressDialog::ProgressDialog(QStringList fileList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    for(int i = 0; i < fileList.size(); ++i){
        ProgressWidget* progressWidget = new ProgressWidget(fileList[i].mid(fileList[i].lastIndexOf('/') + 1), this);
        QListWidgetItem* widgetItem = new QListWidgetItem();
        ui->listWidget->addItem(widgetItem);
        ui->listWidget->setItemWidget(widgetItem, progressWidget);
        widgetItem->setSizeHint(QSize(600, 70));
    }
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(Hfile_info_LUS *hfiles, int num)
{
    for(int i = 0; i < num; ++i){
        //qDebug() << hfiles[i].Hfile_name;
        int progress = 0;
        for(int j = 0; j < 3; ++j){
            progress = progress * 10 + hfiles[i].load_ratio[j] - '0';
        }
        for(int j = 0; j < ui->listWidget->count(); ++j){
            ProgressWidget* progressWidget = (ProgressWidget*)ui->listWidget->itemWidget(ui->listWidget->item(j));
            qDebug() << hfiles[i].Hfile_name << progressWidget->getFileName();
            if(progressWidget->getFileName().toStdString() == std::string(hfiles[i].Hfile_name)){
                progressWidget->updateProgressBar(progress);
                break;
            }
        }
    }
}
