#include "autoconfigwidget.h"
#include "ui_autoconfigwidget.h"

#include <QPushButton>
#include <QDebug>
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QPushButton>
#include <QCryptographicHash>

AutoConfigWidget::AutoConfigWidget(QThreadPool* pool, QList<MyThread*> &threads, unsigned int& threadsCnt, QList<const Device*>* devices, QVector<DeviceInfoWidget *> *deviceList, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoConfigWidget),
    threadsCnt(threadsCnt),
    threads(threads)
{
    ui->setupUi(this);
    this->pool = pool;
    this->parent = parent;
    this->deviceList = deviceList;
    initTableWidget();
    threads.erase(threads.begin(), threads.end());
    for(int i = 0; i < devices->size(); i++){
        addRowItems(devices->at(i)->getName(), devices->at(i)->getHostAddress());
        QStringList filesList;
        this->deviceIP.append(*devices->at(i));
        this->deviceFileInfo.insert(*devices->at(i), filesList);
    }
    connect(ui->cancelOpBtn, &QPushButton::clicked, this, [=](){
        emit sendAutoConfigStatusMessage(QString(tr("退出自动化配置操作")), "default");
        emit(AutoConfigFinish());
        fileName_used.clear();
        path_number.clear();
        this->close();
    });
    ui->cfgInputBtn->hide();
    for(int i = 0; i < deviceList->size(); i++){
        deviceList->at(i)->setProgress(0);
    }
}

AutoConfigWidget::~AutoConfigWidget()
{
    delete ui;
}


void AutoConfigWidget::initTableWidget()
{
    ui->tableWidget->setColumnCount(4);
    QStringList headerText = {"设备列表","ip地址", "配置文件", "选择"};
    ui->tableWidget->setHorizontalHeaderLabels(headerText);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AutoConfigWidget::addRowItems(const QString &devInfo, const QString &ip)
{
    int currentRow = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(currentRow);


    createNewRowItem(currentRow, devInfo, ip);
}

void AutoConfigWidget::createNewRowItem(int rowNum, const QString &devName, const QString &ip)
{
    QTableWidgetItem *item1 = new QTableWidgetItem(devName);

    item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->setItem(rowNum, 0, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(ip);



    item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->setItem(rowNum, 1, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem(tr("请选择文件"));
    item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(rowNum, 2, item3);
    QPushButton *selectBtn = new QPushButton(((QWidget*)ui->tableWidget->currentItem()));

    selectBtn->setText(tr("选择"));
    selectBtn->setMinimumSize(50, 20);

    QPushButton *cancelBtn = new QPushButton(this);

    cancelBtn->setText(tr("取消"));
    cancelBtn->setMinimumSize(50, 20);

    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);

    btnLayout->addWidget(selectBtn);
    btnLayout->addWidget(cancelBtn);

    ui->tableWidget->setCellWidget(rowNum, 3, btnWidget);

    connect(selectBtn, &QPushButton::clicked, this, &AutoConfigWidget::selectFiles);

    connect(cancelBtn, &QPushButton::clicked, this, &AutoConfigWidget::cancelFiles);
}

bool AutoConfigWidget::MD5_test(QString filePath1, QString filePath2)
{
    QFile file1(filePath1), file2(filePath2);
    QCryptographicHash hash1(QCryptographicHash::Md5), hash2(QCryptographicHash::Md5);
    if(file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly)){
        hash1.addData(&file1);
        hash2.addData(&file2);
        if(hash1.result() == hash2.result()){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

void AutoConfigWidget::adjustFileNameColSize()
{
    QFont font("Arial", 12);
    QFontMetrics fontMetrics(font);
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        //QString text = ui->tableWidget->item(i, 2)->text();
        QStringList elidedTextList;
        QString elidedTexts;
        QString ip = ui->tableWidget->item(i, 1)->text(), name = ui->tableWidget->item(i, 0)->text();
        if(deviceFileInfo[Device(ip, name)].size() == 0) continue;
        for(int j = 0; j < deviceFileInfo[Device(ip, name)].size(); j++){
            QString text = deviceFileInfo[Device(ip, name)].at(j);
            QString elidedText = fontMetrics.elidedText(text, Qt::ElideLeft, ui->tableWidget->columnWidth(2));
            elidedTextList.append(elidedText);
        }
        for(int j = 0; j < elidedTextList.size(); j++){
            elidedTexts += elidedTextList[j];
            elidedTexts += j != elidedTextList.size() - 1 ? "\n" : "";
        }
        ui->tableWidget->item(i, 2)->setText(elidedTexts);
    }
}

void AutoConfigWidget::selectFiles()
{
    QPushButton *btn = (QPushButton*)sender();

    QWidget *widget = (QWidget *)btn->parent();

    int x = widget->mapToParent(QPoint(0, 0)).x();
    int y = widget->mapToParent(QPoint(0, 0)).y();

    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));

    int row = index.row();
    int column = index.column();

    qDebug() << "row: " << row << "column: " << column;

    //QStringList pathList = QFileDialog::getOpenFileNames(this, tr("选择上传文件"), tr("/home"), "*.bin");

    QFileDialog *fileDialog = new QFileDialog(this);

    fileDialog->setWindowTitle(QStringLiteral("选择文件"));
    //QString path = QProcessEnvironment::systemEnvironment().value("USERPROFILE");
    GET_DIR
    fileDialog->setDirectory(path);

    fileDialog->setNameFilter("*.bin");

    fileDialog->setViewMode(QFileDialog::Detail);

    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;
    QString files;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();

        QString deviceName;
        QString ip;

        if(fileNames.isEmpty())
        {
            //选择文件
            return;
        }
        UPDATE_DIR(fileNames)
        for(int i = 0; i < fileNames.size(); i++)
        {
            //files.clear();
            QFileInfo info(fileNames.at(i));
//            if(path_number[info.absoluteFilePath()] == 0){
//                if(fileName_used[info.fileName()] == 0){
//                    files += info.fileName();
//                    path_number[info.absoluteFilePath()] = ++fileName_used[info.fileName()];

//                }
//                else{
//                    path_number[info.absoluteFilePath()] = ++fileName_used[info.fileName()];
//                    files += info.fileName() + QString("(%1)").arg(path_number[info.absoluteFilePath()] - 1);

//                }
//            }
//            else{
//                if(path_number[info.absoluteFilePath()] == 1){
//                    files += info.fileName();
//                }
//                else{
//                    files += info.fileName() + QString("(%1)").arg(path_number[info.absoluteFilePath()] - 1);
//                }
//            }
            files += info.absoluteFilePath();
            files += i < fileNames.size() - 1 ? "\n" : "";
        }

        QTableWidgetItem *item = new QTableWidgetItem(files);

        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(row, column - 1, item);
        deviceName = ui->tableWidget->item(row, column - 3)->text();
        ip = ui->tableWidget->item(row, column - 2)->text();

        Device dev(" ", " ");
        for(int i = 0; i < this->deviceIP.size(); i++)
        {
            if(deviceName == this->deviceIP.at(i).getName() && ip == this->deviceIP.at(i).getHostAddress())
            {
                dev = this->deviceIP.at(i);
                break;
            }

        }

        //this->deviceFileInfo.value(this->deviceIP.value(deviceName), fileaNames);
        //this->deviceFileInfo.value(this->deviceIP.value(deviceName), fileNames);
        this->deviceFileInfo[dev] = fileNames;
        adjustFileNameColSize();
    }

}

void AutoConfigWidget::on_beginCfgBtn_clicked()
{

    //for test
    //AutoConfigProgressDialog* autoConfigProgressDialog = new AutoConfigProgressDialog(this);
    qDebug() << "设备数量: " << this->deviceFileInfo.size();
    this->setEnabled(false);
    QMap<Device, QStringList>::iterator it = this->deviceFileInfo.begin();
    threadsCnt = 0;
    while(it != this->deviceFileInfo.end())
    {
        qDebug() << it.key().getHostAddress();
        qDebug() << it.value();
        MyThread* thread = new AutoConfigThread(pool, it.value(), &it.key(), new TftpRequest());
        threads.append(thread);
        //threads[threadsCnt] = new AutoConfigThread(it.value(), &it.key(), new TftpRequest());
        connect((AutoConfigThread*)thread, &AutoConfigThread::autoConfigStatusMessage, this, [=](QString message){
            emit sendAutoConfigStatusMessage(message, it.key().getName());
        });
        //autoConfigProgressDialog->addProgressBar((AutoConfigThread*)thread);
        for(int i = 0; i < deviceList->size(); i++){
            if(*deviceList->at(i)->getDevice() == it.key()){
               connect((AutoConfigThread*)thread, &AutoConfigThread::autoConfigRate, deviceList->at(i), (newSetProgressType)&DeviceInfoWidget::setProgress);
               connect((AutoConfigThread*)thread, &AutoConfigThread::threadFinish, this, [=](){
                   if(++finishedThreadCnt == threads.size()){
                       emit(AutoConfigFinish());
                   }
               });
               connect((MainWindow*)this->parent, &MainWindow::mainThreadExit, (AutoConfigThread*)thread, &AutoConfigThread::mainThreadExited);
            }
        }
        pool->start(thread);
        thread->setAutoDelete(true);
        it++;
    }
    //autoConfigProgressDialog->show();
}

void AutoConfigWidget::resizeEvent(QResizeEvent *event)
{
    adjustFileNameColSize();
}

void AutoConfigWidget::cancelFiles()
{
    QPushButton *btn = (QPushButton*)sender();

    QWidget *widget = (QWidget *)btn->parent();

    int x = widget->mapToParent(QPoint(0, 0)).x();
    int y = widget->mapToParent(QPoint(0, 0)).y();

    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));

    int row = index.row();
    int column = index.column();

    qDebug() << "cancel row: " << row << "column: " << column;

    //删除表格中选择的文件名

    QTableWidgetItem *item = new QTableWidgetItem(tr("请选择文件"));

    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->setItem(row, column - 1, item);


    //更新Map中的文件
    QString deviceName = ui->tableWidget->item(row, 0)->text();
    QString ip = ui->tableWidget->item(row, 1)->text();
    Device dev(" ", " ");
    for(int i = 0; i < this->deviceIP.size(); i++)
    {
        if(deviceName == this->deviceIP.at(i).getName() && ip == this->deviceIP.at(i).getHostAddress())
        {
            dev = this->deviceIP.at(i);
            break;
        }

    }

    //this->deviceFileInfo.value(this->deviceIP.value(deviceName), fileaNames);
    //this->deviceFileInfo.value(this->deviceIP.value(deviceName), fileNames);
    this->deviceFileInfo[dev] = QStringList();
}
