#include "informationwidget.h"
#include "ui_informationwidget.h"

InformationWidget::InformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationWidget)
{
    ui->setupUi(this);


    //ui->targetInfoTreeWidget->setWindowTitle(u8"详细信息");
    //隐藏treeWidget表头
    ui->targetInfoTreeWidget->setHeaderHidden(true);

    //ui->targetInfoTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

InformationWidget::~InformationWidget()
{
    delete ui;
}

//void InformationWidget::setInformation(const QString &info)
//{
//    //ui->infoTextEdit->setText(info);

//}

//void InformationWidget::appendInformation(const QString info)
//{
//    //ui->infoTextEdit->append(info);
//}

void InformationWidget::setTargetInfoNum(const int &num, const int &partNumber)
{
    this->targetNum = num;
    this->partNum = partNumber;
}

void InformationWidget::setTargetInfo(File_LCL lcl, QString name, QString ip)
{
    char Pro_ver[3];
    memcpy(Pro_ver, lcl.Pro_ver, 2);
    Pro_ver[2] = '\0';
    ui->targetNumLabel->setText(QString("%1个, 版本%2.").arg(++hardwareNum).arg(Pro_ver));
    //ui->targetInfoTreeWidget->clear();
    //添加顶层节点
    for(int i = 0 ; i < lcl.Hw_num; i++)
    {
        QTreeWidgetItem *topItem = new QTreeWidgetItem(ui->targetInfoTreeWidget);
        topItem->setText(0, QString("%1-%2").arg(name).arg(ip));
        //topItem->setCheckState(0, Qt::Unchecked);
        topItem->setData(0, Qt::CheckStateRole, QVariant());

        ui->targetInfoTreeWidget->addTopLevelItem(topItem);

        QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
        childItem->setText(0, QString("目标码: %1").arg(lcl.Hws[i].target_code));

        QTreeWidgetItem *childItem2 = new QTreeWidgetItem(topItem);
        childItem2->setText(0, QString("序列号: %1").arg(lcl.Hws[i].serial_code));
        for(int j = 0; j < lcl.Hws[i].number_part_number; j++)
        {
            QTreeWidgetItem *childItem3 = new QTreeWidgetItem(topItem);
            childItem3->setText(0, QString("分区%1").arg(j + 1));

            QTreeWidgetItem *childItem31 =  new QTreeWidgetItem(childItem3);
            childItem31->setText(0, QString("分区名: %1").arg(lcl.Hws[i].parts[j].part_number));

            QTreeWidgetItem *childItem32 =  new QTreeWidgetItem(childItem3);
            childItem32->setText(0, QString("分区描述: %1").arg(lcl.Hws[i].parts[j].part_design));

            QTreeWidgetItem *childItem33 =  new QTreeWidgetItem(childItem3);
            childItem33->setText(0, QString("修正值: %1").arg(lcl.Hws[i].parts[j].amend));
        }

    }

}
