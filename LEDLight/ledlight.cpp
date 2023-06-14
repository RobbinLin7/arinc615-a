#include "ledlight.h"

LEDLight::LEDLight()
{
    this->mSize = 10;
    this->mColour = "gray";
}

LEDLight::LEDLight(const QString &colour, const int &size)
{
    this->mColour = colour;
    this->mSize = size;

    //setLED();
}

void LEDLight::setLED(QLabel *label){
    //设置矩形大小
    //若ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度
    QString minWidth = QString("min-width:%1px;").arg(this->mSize);
    QString minHeight = QString("min-height:%1px;").arg(this->mSize);
    //若ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度
    QString maxWidth = QString("max-width:%1px;").arg(this->mSize);
    QString maxHeight = QString("max-height:%1px;").arg(this->mSize);

    //设置边界形状及边框
    QString borderRadius = QString("border-radius:%1px;").arg(this->mSize / 2);
    QString border = QString("border:1px solid black;");

    //最后设置背景颜色
    QString background = "background-color:";

    if(this->mColour == "gray")
    {
        //灰色
        background += "rgb(190,190,190)";
    }
    else if(this->mColour == "red")
    {
        //红色
        background += "rgb(255,0,0)";
    }
    else if(this->mColour == "green")
    {
        //绿色
        background += "rgb(0,255,0)";
    }
    else if(this->mColour == "green")
    {
        //黄色
        background += "rgb(255,255,0)";
    }
    else
    {
        //默认灰色
        background += "rgb(190,190,190)";
    }

    const QString sheetStyle = minWidth + minHeight
            + maxWidth + maxHeight + borderRadius + border + background;

    label->setStyleSheet(sheetStyle);

}


void LEDLight::setLED(QLabel *label, const QString &colour, const int &size){

    this->mSize = size;

    this->mColour = colour;

    setLED(label);
}

