#include "createtopologyxml.h"

#define DISTANCE_X (300)
#define DISTANCE_Y (600)

CreateTopologyXML::CreateTopologyXML()
{

}

/**
 * @brief CreateTopologyXML::createTopologyXMLFile  生成网络拓扑描述XML文件
 * @param filename  要保存的文件名,包含路径
 * @param devList  设备列表
 * @return 生成XML是否成功?我这没返回值了，也不知道生没生成功
 */
bool CreateTopologyXML::createTopologyXMLFile(const QString &filename,
                                              a615_targets_find_list_t devList,
                                              const QString &hostIPAddr)
{
    qDebug()<<"enter createTopologyXMLFile successful"<<endl;
    QFile file(filename);
    bool ret = file.open(QIODevice::WriteOnly);
    if(!ret) {
        return false;
    }

    QXmlStreamWriter xswWriter(&file);
    xswWriter.setAutoFormatting(true);
    xswWriter.writeStartDocument("1.0");
    // 根节点开始
    xswWriter.writeStartElement("Topology");

    // NetworkDevices 节点
    xswWriter.writeStartElement("NetworkDevices");

    // 主控机信息
    xswWriter.writeStartElement("device");
    xswWriter.writeAttribute("type", "MC");
    xswWriter.writeAttribute("ip", hostIPAddr);
    xswWriter.writeAttribute("id", "0");
    xswWriter.writeAttribute("hardwareID", "MC");
    xswWriter.writeAttribute("hardwareType", "MainComputer");
    xswWriter.writeAttribute("info", "主控机");
    xswWriter.writeAttribute("posX", "100");
    xswWriter.writeAttribute("posY", "100");
    xswWriter.writeEndElement();

    uint posX = 100, posY = 400;
    uint posSW_X = 150, posSW_Y = 800;
    for(uint i = 0; i < devList.device_num; i++) {
        // 目前没有分辨 端系统 和 交换机
        xswWriter.writeStartElement("device");
        QString literalName = devList.targets_info[i].literal_name;
        QString devType = QString();
        if(literalName.contains("ES"))
        {
            devType = "ES";
        }else if(literalName.contains("SW"))
        {
            devType = "SW";
        }else
        {
            devType = "**";
        }

        xswWriter.writeAttribute("type", devType);
        xswWriter.writeAttribute("ip", devList.targets_info[i].ip_addr);
        xswWriter.writeAttribute("id", QString("%1").arg(i + 1));
        xswWriter.writeAttribute("hardwareID", devList.targets_info[i].identifier);
        xswWriter.writeAttribute("hardwareType", devList.targets_info[i].type_name);
        xswWriter.writeAttribute("info", QString("%1%2 在线").arg(devType).arg(i + 1)); // **是端系统或者交换机

        if("SW" == devType)
        {
            xswWriter.writeAttribute("posX", QString("%1").arg(posSW_X));
            xswWriter.writeAttribute("posY", QString("%1").arg(posSW_Y));
            posSW_X += DISTANCE_X + 100;
            if(posSW_X >= 1500)
            {
                posSW_X = 150;
                posSW_Y += DISTANCE_Y;
            }
            //posY += DISTANCE_Y;
        }
        else
        {
            xswWriter.writeAttribute("posX", QString("%1").arg(posX));
            xswWriter.writeAttribute("posY", QString("%1").arg(posY));
            posX += DISTANCE_X;
            if(posX >= 1800)
            {
                posX = 100;
                posY += DISTANCE_Y;
            }
            //posY += DISTANCE_Y;
        }

        xswWriter.writeEndElement();

    }

    xswWriter.writeEndElement();

    // links 节点
    xswWriter.writeStartElement("links");

//    for(uint i = 0; i < devList.device_num; i++) {
//        // 目前没有分辨 端系统 和 交换机
//        xswWriter.writeStartElement("link");
//        xswWriter.writeAttribute("beginItemID", "0");
//        xswWriter.writeAttribute("endItemID", QString("%1").arg(i + 1));
//        xswWriter.writeEndElement();
//    }


    xswWriter.writeEndElement();

    xswWriter.writeEndElement();

    //结束文档
    xswWriter.writeEndDocument();

    file.close();
    return true;
}
