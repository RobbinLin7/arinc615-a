#include "findinfowidget.h"
#include "ui_findinfowidget.h"

#include "globalDefine.h"

#include <QDebug>

FindInfoWidget::FindInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindInfoWidget)
{
    ui->setupUi(this);

    this->mCurrentRow = 0;

    initTableWidget();
}

FindInfoWidget::~FindInfoWidget()
{
    delete ui;
}

//==================================================================
//function name: insertDeviceInfo
//author: lcy
//date: 2022-8-1
//function:向表格中插入设备的相关信息
//input arg:
//return:
//log:8-1新增
//==================================================================
void FindInfoWidget::insertDeviceInfo(const QStringList &deviceInfo)
{
    if(this->mCurrentRow == MAX_TARGET_NUMBER)
    {
        this->mCurrentRow = 0;

    }
    if(MAX_COLUMN_COUNT != deviceInfo.size())
    {
        qDebug() << "FindInfoWidget::insertDeviceInfo Input device info ERROR!";
        return;
    }
    for(int i = 0; i < deviceInfo.size(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(deviceInfo.at(i));

        ui->findInfoTableWidget->setItem(this->mCurrentRow, i, item);
    }
    this->mCurrentRow++;

}

void FindInfoWidget::clearTableWidget()
{
    if(this->mCurrentRow > 0)
    {
        for(int i = 0 ; i < this->mCurrentRow; i++)
        {
            for(int j = 0; j < MAX_COLUMN_COUNT; j++)
            {
                delete ui->findInfoTableWidget->item(i, j);
            }
        }
        //delete ui->findInfoTableWidget->
        ui->findInfoTableWidget->clear();
        this->mCurrentRow = 0;
    }

    initTableWidget();
}

//==================================================================
//function name: initTableWidget
//author: lcy
//date: 2022-8-1
//function:初始化表格的相关参数
//input arg:
//return:
//log:8-1新增
//==================================================================
void FindInfoWidget::initTableWidget()
{
    QStringList tableHeader;

    //设置表的行数和列数
    ui->findInfoTableWidget->setRowCount(MAX_TARGET_NUMBER);
    ui->findInfoTableWidget->setColumnCount(MAX_COLUMN_COUNT);

    //设置表头
    tableHeader << "IP地址" << "硬件标识" << "硬件类型" << "硬件位置" << "名称" << "制造商代码";
    ui->findInfoTableWidget->setHorizontalHeaderLabels(tableHeader);

    //设置表格编辑属性
    //表格不可编辑
    ui->findInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置单元格选中属性
    //整行选中
    ui->findInfoTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //隐藏列表头
    ui->findInfoTableWidget->verticalHeader()->setVisible(false);

    //单元格内文字对应关系

    //自动调整行列宽
    //ui->findInfoTableWidget->resizeColumnsToContents();
    //ui->findInfoTableWidget->resizeRowsToContents();

    //自适应行列宽
    ui->findInfoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->findInfoTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}


