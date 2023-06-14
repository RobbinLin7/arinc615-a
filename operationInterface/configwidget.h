#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QFileDialog>

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = nullptr);
    ~ConfigWidget();
    Ui::ConfigWidget* getUi();

signals:
    void saveButton_clicked(QString dir, QString localIp);

private slots:
    void selectUploadDir();

private:
    Ui::ConfigWidget *ui;
};

#endif // CONFIGWIDGET_H
