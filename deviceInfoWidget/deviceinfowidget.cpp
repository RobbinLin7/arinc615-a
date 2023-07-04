#include "deviceinfowidget.h"
#include "ui_deviceinfowidget.h"
#include<QDebug>

DeviceInfoWidget::DeviceInfoWidget(QWidget *parent, const Device* device) :
    QWidget(parent), ui(new Ui::DeviceInfoWidget),
    device(device)
{
    ui->setupUi(this);
    initDeviceInfoWidget();
    this->setDeviceInfo(QString("%1").arg(device->getName()), QString("%1").arg(device->getHostAddress()));
    ui->deviceIPLineEdit->setReadOnly(true);
    ui->deviceNameLineEdit->setReadOnly(true);
    connect(ui->deviceRadioBtn, &QRadioButton::toggled, this, [=](bool checked){
        emit(radioBtnChecked(checked, device));
    });
}

DeviceInfoWidget::~DeviceInfoWidget()
{
    delete ui;
}

void DeviceInfoWidget::setRadioBtn(bool status)
{
    if(status != ui->deviceRadioBtn->isChecked()){
        ui->deviceRadioBtn->setChecked(status);
    }
}

void DeviceInfoWidget::setProgress(const int &progress)
{
    static int colour = 0; //控制蓝色深浅
    LEDLight led;
    //设置进度
    ui->operationProgressLabel->setText(QString("%1%").arg(progress));

    //设置灯的颜色,设计为蓝色深浅变化

    //若进度达到100%
    if(100 == progress)
    {
        //颜色改为绿色
        led.setLED(ui->statusLabel, "green", 30);
    }
    else
    {
        //否则函数调用一次改变一次颜色
        if(colour % 2 == 0)
            led.setLED(ui->statusLabel, "rgb(198,220,236)", 30);
        else
            led.setLED(ui->statusLabel, "rgb(0,0,255)", 30);
    }
    //颜色标志自增
    colour++;
    if(colour == MAX_ADD_VALUE)
        colour = 0;
}

void DeviceInfoWidget::setProgress(const int &progress, bool flag)
{
    LEDLight led;
    if(flag) setProgress(progress);
    else{
        led.setLED(ui->statusLabel, "red", 30);
    }
}

void DeviceInfoWidget::setDeviceInfo(const QString &deviceName, const QString &deviceIP)
{
    ui->deviceNameLineEdit->setText(deviceName);
    ui->deviceIPLineEdit->setText(deviceIP);
}

void DeviceInfoWidget::initDeviceInfoWidget(){
    //初始化进度为0
    ui->operationProgressLabel->setText("0");

    ui->statusLabel->setText(" ");

    //初始化状态灯颜色为灰色
    LEDLight led;

    led.setLED(ui->statusLabel, "gray", 30);

}

QString DeviceInfoWidget::getDeviceIP()
{
    return ui->deviceIPLineEdit->text();
}

bool DeviceInfoWidget::checkedOrNot(){
    return ui->deviceRadioBtn->isChecked();
}

const Device* DeviceInfoWidget::getDevice(){
    return device;
}


