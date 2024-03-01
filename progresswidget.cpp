#include "progresswidget.h"
#include "ui_progresswidget.h"
#include <QDebug>

ProgressWidget::ProgressWidget(QString fileName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget),
    fileName(fileName)
{
    ui->setupUi(this);
    ui->fileNameLabel->setText(fileName);
    ui->progressBar->setValue(0);
    ui->statusLabel->setText("上传未开始");
}

void ProgressWidget::setStatusLabel(QString status)
{
    ui->statusLabel->setText(status);
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

const QString &ProgressWidget::getFileName() const
{
    return fileName;
}

void ProgressWidget::updateProgressBar(int rate)
{
    qDebug() << fileName << " " << rate;
    ui->progressBar->setValue(rate);
    if(rate > 0){
        if(rate == 100){
            ui->statusLabel->setText(QString("上传完成"));
        }
        else if(rate > 0 && ui->progressBar->value() == 0){
            ui->statusLabel->setText(QString("上传中"));
        }
    }

}

