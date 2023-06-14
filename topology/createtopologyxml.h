#ifndef CREATETOPOLOGYXML_H
#define CREATETOPOLOGYXML_H

#include "protocal/arinc615_global_var_define.h"
#include "protocal/arinc615_struct_define.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>

class CreateTopologyXML
{
public:
    CreateTopologyXML();

    static bool createTopologyXMLFile(const QString &filename, a615_targets_find_list_t devList, const QString &hostIPAddr);
};

#endif // CREATETOPOLOGYXML_H
