#include "progressdialog.h"
#include "ui_progressdialog.h"
#include "progresswidget.h"
#include <QListWidgetItem>

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
