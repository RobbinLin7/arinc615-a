#ifndef PARACONFIGDIALOG_H
#define PARACONFIGDIALOG_H

#include <QDialog>
#include <QValidator>
#include "dir.h"
namespace Ui {
class ParaConfigDialog;
}

class ParaConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParaConfigDialog(QWidget *parent = nullptr);
    ~ParaConfigDialog();

private:
    Ui::ParaConfigDialog *ui;
    QIntValidator* validator1;
    QIntValidator* validator2;
signals:
    void sendParas(size_t, size_t, size_t, size_t);
    void sendLog(QString);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // PARACONFIGDIALOG_H
