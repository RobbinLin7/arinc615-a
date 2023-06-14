#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QWidget>

#include "globalDefine.h"
//#include "device.h"
#include "protocal/file_define.h"

using namespace GlobalDefine;
namespace Ui {
class InformationWidget;
}

class InformationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InformationWidget(QWidget *parent = nullptr);
    ~InformationWidget(); 
//    void setInformation(const QString &info);
//    void appendInformation(const QString info);

    void setTargetInfo(File_LCL lcl);

    void setTargetInfoNum(const int &num, const int &partNumber);


private:
    Ui::InformationWidget *ui;

    size_t targetNum;

    size_t partNum;

    size_t hardwareNum = 0;


};

#endif // INFORMATIONWIDGET_H
