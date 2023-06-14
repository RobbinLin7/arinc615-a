#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    connect(ui->selectUploadDirBtn, &QPushButton::clicked, this, &ConfigWidget::selectUploadDir);
    connect(ui->saveBtn, &QPushButton::clicked, this, [=](){
        emit(saveButton_clicked(ui->uploadDir->text(), ui->localIp->text()));
    });
    //connect(this, SIGNAL(on_saveButton_clicked(QString, QString)), parent, SLOT(saveConfig(QString, QString)));
//    connect(ui->returnBtn, &QPushButton::clicked, parent, SLOT(exitConfig()));
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}


void ConfigWidget::selectUploadDir(){
    //QString dirName = QFileDialog::getOpenFileUrl(this, tr("文件对话框"), "D:", tr("图片文件（*png *jpg）"));
    QString dir = QFileDialog::getExistingDirectory(this, QStringLiteral("选择上传文件夹"), QCoreApplication::applicationDirPath());
    ui->uploadDir->setText(dir);
}



Ui::ConfigWidget* ConfigWidget::getUi(){
    return ui;
}

