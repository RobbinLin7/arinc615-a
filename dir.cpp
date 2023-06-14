#include "dir.h"
#include <QDomDocument>
#include <QProcessEnvironment>

bool loadxml()
{
    QFile file("615A.xml");
    if(file.open(QIODevice::ReadOnly)){
        QXmlStreamReader xmlReader(&file);
        while(!xmlReader.atEnd() && !xmlReader.hasError()){
            xmlReader.readNext();
            if(xmlReader.isStartElement()){
                if(xmlReader.name() == "path"){
                    QString path = xmlReader.readElementText();
                    if(QFile::exists(path)) memcpy(dir, path.toUtf8().data(), strlen(path.toUtf8().data()) + 1);
                    else strcpy(dir, QProcessEnvironment::systemEnvironment().value("USERPROFILE").toUtf8().data());
                    //qDebug() << "path" << path;
                }
                else if(xmlReader.name() == "max-retrans-times"){
                    bool ok;
                    uint dec = xmlReader.readElementText().toULongLong(&ok, 10);
                    if(ok){
                        max_retrans_times = dec;
                    }
                    //qDebug() << "max-retrans-times" << max_retrans_times;
                }
                else if(xmlReader.name() == "wait-time-ms"){
                    bool ok;
                    uint dec = xmlReader.readElementText().toULongLong(&ok, 10);
                    if(ok){
                        wait_time_ms = dec;
                    }
                }
            }
            else{
                continue;
            }
        }
        file.close();
    }
    else{
        strcpy(dir, QProcessEnvironment::systemEnvironment().value("USERPROFILE").toUtf8().data());
    }
    return true;
}

bool savexml()
{
    char buffer[20];
    QFile file("615A.xml");
    if(file.open(QIODevice::ReadWrite)){
        QDomDocument doc;

        // 创建XML处理类，通常用于处理第一行描述信息
        QDomProcessingInstruction instruction;

        // 创建XML头部格式
        instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");

        // 添加到XML文件中
        doc.appendChild(instruction);

        // 创建根节点
        QDomElement root = doc.createElement("paralist");


        // 添加子节点path
        QDomElement path = doc.createElement("path");
        QDomText path_text = doc.createTextNode(dir);
        path.appendChild(path_text);
        root.appendChild(path);

        //添加子节点max_retrans_times
        QDomElement max_retrans_times_element = doc.createElement("max-retrans-times");
        QDomText max_retrans_times_text = doc.createTextNode(itoa(max_retrans_times, buffer, 10));
        max_retrans_times_element.appendChild(max_retrans_times_text);
        root.appendChild(max_retrans_times_element);

        //添加子节点wait_time_ms
        QDomElement wait_time_ms_element = doc.createElement("wait-time-ms");
        QDomText wait_time_ms_text = doc.createTextNode(itoa(wait_time_ms, buffer, 10));
        wait_time_ms_element.appendChild(wait_time_ms_text);
        root.appendChild(wait_time_ms_element);


        doc.appendChild(root);

        QTextStream stream(&file);
        doc.save(stream, 4);		// 缩进四格

        file.close();


    }
    else{
        return false;
    }
    return true;
}
