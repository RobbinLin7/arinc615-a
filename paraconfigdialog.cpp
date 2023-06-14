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
    ui->buttonBox->setEnabled(false);
    //QRegExp regx("[1-9][0-9]{0,2}");
    //ui->lineEdit->setValidator(new QRegExpValidator(regx, this));
    //最大重传次数设置输入范围：10~30
    validator1 = new QIntValidator(10, 30, this);

    ui->lineEdit->setValidator(validator1);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确认"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
    //设置超时重传参数范围：1000~3000
    validator2 = new QIntValidator(1000, 3000, this);

    ui->lineEdit_2->setValidator(validator2);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=](){
        emit(sendParas(ui->lineEdit->text().toULongLong(), ui->lineEdit_2->text().toULongLong()));
        emit(sendLog(QString(tr("参数配置成功"))));
        qDebug() << ui->lineEdit->text() << ui->lineEdit_2->text();
    });
    connect(this, &ParaConfigDialog::rejected, this, [=](){
        emit(sendLog(QString(tr("退出参数配置界面，配置失败"))));
    });
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    ui->label_3->setPalette(pe);
    ui->label_4->setPalette(pe);
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
    QString input1 = ui->lineEdit->text();
    QString input2 = ui->lineEdit_2->text();
    int pos = 0;
    if(validator1->validate(input1, pos) == QValidator::Acceptable && validator2->validate(input2, pos) == QValidator::Acceptable){
        ui->buttonBox->setEnabled(true);
    }
    else{
        ui->buttonBox->setEnabled(false);
    }
    QDialog::keyReleaseEvent(event);
}

