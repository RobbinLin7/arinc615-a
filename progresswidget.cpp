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

void ProgressWidget::updateStatus(const unsigned short statusCode)
{
    switch (statusCode) {
    case 0x0001:
        ui->statusLabel->setText("传输未开始");
        break;
    case 0x0002:
        ui->statusLabel->setText("传输中");
        break;
    case 0x0003:
        ui->statusLabel->setText("传输完成");
        break;
    case 0x1007:
        ui->statusLabel->setText("传输异常结束");
        break;
    default:
        break;
    }
}

void ProgressWidget::updateProgressBar(int rate)
{
    //qDebug() << fileName << " " << rate;
    ui->progressBar->setValue(rate);
}

