#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QFile>

namespace Ui {
class FileInfoWidget;
}

class FileInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoWidget(QFile* file, QWidget *parent = nullptr);
    FileInfoWidget(QString fileName, QString fileDes, QWidget *parent = nullptr);
    ~FileInfoWidget();
    void setSelectRadio(bool);
    bool getChecked()const;
    QString getFileName()const;
    QString getFileAbsolutePath()const;
    Ui::FileInfoWidget *ui;
private:
    QFile* file = nullptr;
    QString absolutePath;
    bool flag = 0;
signals:
    void selectRadioChecked(bool, QString);
private slots:
    void on_radioButton_toggled(bool checked);
};

#endif // FILEINFOWIDGET_H
