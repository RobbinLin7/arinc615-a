#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"
#include <qdebug.h>
#include <QFileInfo>
FileInfoWidget::FileInfoWidget(QFile *file, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FileInfoWidget)
{
    ui->setupUi(this);
    this->file = file;
    file->open(QIODevice::ReadOnly);
    QFileInfo fileInfo(*file);
    absolutePath = fileInfo.absoluteFilePath();
    QString fileName = file->fileName();
    qDebug() << fileName.lastIndexOf('/');
    //获取文件名，去除路径
    fileName = fileName.right(fileName.size() - fileName.lastIndexOf('/') - 1);
//    const unsigned int maxSize = (1 << 16) * 512;
    this->ui->lineEdit->setText(fileName);
    double size_mb = file->size() * 1.0 / 1024 / 1024;
    if(size_mb < 1.0){
        this->ui->label->setText(tr("文件大小(KB)"));
        this->ui->lineEdit_2->setText(QString::number(size_mb * 1024, 'f', 1));
    }
    else{
        this->ui->lineEdit_2->setText(QString::number(file->size() * 1.0 / 1024 / 1024, 'f', 1));
    }
    file->close();
}

FileInfoWidget::FileInfoWidget(QString fileName, QString fileDes, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FileInfoWidget)
{
    ui->setupUi(this);
    this->ui->label->setText(tr("文件类型"));
    this->ui->lineEdit->setText(fileName);
    this->ui->lineEdit_2->setText(fileDes);
}

FileInfoWidget::~FileInfoWidget()
{
    delete ui;
}

void FileInfoWidget::setSelectRadio(bool checked){
    if(ui->radioButton->isChecked() != checked){
        ui->radioButton->setChecked(checked);
    }
}

bool FileInfoWidget::getChecked(){
    return ui->radioButton->isChecked();
}

QString FileInfoWidget::getFileName()
{
    return this->ui->lineEdit->text();
}

QString FileInfoWidget::getFileAbsolutePath(){
    return this->absolutePath;
}

void FileInfoWidget::on_radioButton_toggled(bool checked)
{
    emit(selectRadioChecked(checked, absolutePath));
}

