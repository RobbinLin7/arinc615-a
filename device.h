#ifndef DEVICE_H
#define DEVICE_H
#include<QString>
class Device{
private:
    QString hostAddress;
    QString name;
    QString hardwareType;
    QString positon;
    QString literalName;
    QString manufactureCode;
public:
    Device(QString hostAddress, QString name):hostAddress(hostAddress), name(name){}
    Device(QString hostAddress, QString name, QString hardwareType, QString position, QString literalName, QString manufactureCode):hostAddress(hostAddress), name(name){
        this->hardwareType = hardwareType;
        this->positon = position;
        this->literalName = literalName;
        this->manufactureCode = manufactureCode;
    }
    /*****************************************************************
    * 函数名称： 小于符号运算符重载
    * 功能描述： 用于比较两个device类的大小关系，方便作为树的索引
    * 参数说明： 参数说明
    * 返回值：   device1 < device2 返回device1是否小于device2
    ******************************************************************/
    bool operator < (const Device& info) const
    {
        if(this->name != info.name){
            return this->name < info.name;
        }
        return this->getHostAddress() < info.getHostAddress();
    }
    /*****************************************************************
    * 函数名称： 等于符号运算符重载
    * 功能描述： 用于比较两个device类的大小关系，方便作为树的索引
    * 参数说明： 参数说明
    * 返回值：   device1 == device2 返回device1是否等于device2
    ******************************************************************/
    bool operator == (const Device& info) const
    {
        return ((this->hostAddress == info.hostAddress) && (this->name == info.name)
                && (this->hardwareType == info.hardwareType)
                && (this->positon == info.positon)
                && (this->literalName == info.literalName)
                && (this->manufactureCode == info.manufactureCode));
    }
    /*****************************************************************
    * 函数名称： Name
    * 功能描述： 详细描述
    * 参数说明： 参数说明
    * 返回值：   返回值说明
    ******************************************************************/
    QString getHostAddress()const{return hostAddress;}
    QString getName()const{return name;}
    QString getHardwareType()const{return hardwareType;}
    QString getPosition()const{return positon;}
    QString getLiteralName()const{return literalName;}
    QString getManufactureCode()const{return manufactureCode;}
};
#endif // DEVICE_H
