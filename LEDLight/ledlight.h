#ifndef LEDLIGHT_H
#define LEDLIGHT_H

#include <QLabel>
/*
 *   用Label实现状态灯
 *
 *
 */

class LEDLight
{
public:
    LEDLight();

    LEDLight(const QString &colour, const int &size);

    void setLED(QLabel *label);
    void setLED(QLabel *label, const QString &colour, const int &size);


private:

    int mSize;
    QString mColour;

};

#endif // LEDLIGHT_H
