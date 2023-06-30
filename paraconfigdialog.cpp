#include "paraconfigdialog.h"
#include "ui_paraconfigdialog.h"

#include <QDebug>
#include <QPalette>
#include <QMessageBox>
#include <QPushButton>

ParaConfigDialog::ParaConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParaConfigDialog)
{
    ui->setupUi(this);
    //设置为用户上一次设置的保存在XML文件中的值
    ui->findMaxResponseTimeSpinBox->setValue(max_find_response_time_ms / 1000);
    ui->maxRetransTimesSpinBox->setValue(max_retrans_times);
    ui->timeoutSpinBox->setValue(wait_time_ms);
    ui->stateFileSendIntervalSpinBox->setValue(state_file_send_interval);
    //ui->buttonBox->setEnabled(false);
    //QRegExp regx("[1-9][0-9]{0,2}");
    //ui->lineEdit->setValidator(new QRegExpValidator(regx, this));
    //最大重传次数设置输入范围：10~30
    //validator1 = new QIntValidator(10, 30, this);
    ui->maxRetransTimesSpinBox->setRange(10, 30);
    //ui->maxRetransTimesSpinBox->setValidator(validator1);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确认"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
    //设置超时重传参数范围：1000~3000
    //validator2 = new QIntValidator(1000, 3000, this);
    ui->timeoutSpinBox->setRange(1000, 3000);
    //ui->timeoutSpinBox->setValidator(validator2);

    //设置FIND操作最大响应时间范围
    ui->findMaxResponseTimeSpinBox->setRange(1, 10);

    //设置状态文件发送间隔范围
    ui->stateFileSendIntervalSpinBox->setRange(1, 10);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=](){
        emit(sendParas(ui->findMaxResponseTimeSpinBox->text().toULongLong(),
                       ui->maxRetransTimesSpinBox->text().toULongLong(),
                       ui->timeoutSpinBox->text().toULongLong(),
                       ui->stateFileSendIntervalSpinBox->text().toULongLong()));
        emit(sendLog(QString(tr("参数配置成功"))));
        qDebug() << ui->maxRetransTimesSpinBox->text() << ui->timeoutSpinBox->text();
    });
    connect(this, &ParaConfigDialog::rejected, this, [=](){
        emit(sendLog(QString(tr("退出参数配置界面，配置失败"))));
    });
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    //ui->label_3->setPalette(pe);
    //ui->label_4->setPalette(pe);
}

ParaConfigDialog::~ParaConfigDialog()
{
    delete ui;
}

void ParaConfigDialog::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "keyPressEvent";
}

void ParaConfigDialog::keyReleaseEvent(QKeyEvent *event)
{
//    //ui->
//    QString input1 = ui->maxRetransTimesSpinBox->text();
//    QString input2 = ui->timeoutSpinBox->text();
//    int pos = 0;
//    if(validator1->validate(input1, pos) == QValidator::Acceptable && validator2->validate(input2, pos) == QValidator::Acceptable){
//        ui->buttonBox->setEnabled(true);
//    }
//    else{
//        ui->buttonBox->setEnabled(false);
//    }
    QDialog::keyReleaseEvent(event);
}

