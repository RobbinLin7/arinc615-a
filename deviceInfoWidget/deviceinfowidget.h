#ifndef DEVICEINFOWIDGET_H
#define DEVICEINFOWIDGET_H

#include <QWidget>

#include "LEDLight/ledlight.h"
#include "device.h"
#include "globalDefine.h"

namespace Ui {
class DeviceInfoWidget;
}
class DeviceInfoWidget;

class DeviceInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInfoWidget(QWidget *parent = nullptr, const Device* device = nullptr);
    ~DeviceInfoWidget();

    //void initDeviceInfoWidget();

    void setRadioBtn(bool status);

    void setDeviceInfo(const QString &deviceName, const QString &deviceIP);

    QString getDeviceIP()const;

    const Device* getDevice()const;

    bool checkedOrNot() const;
signals:
    void radioBtnChecked(bool, const Device* device);

private:
    Ui::DeviceInfoWidget *ui;
    const Device* device;
    void initDeviceInfoWidget();
public slots:
    void setProgress(const int&);
    void setProgress(const int&, bool flag);

};
typedef void (DeviceInfoWidget::*newSetProgressType)(const int&, bool);
#endif // DEVICEINFOWIDGET_H
