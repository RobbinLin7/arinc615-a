#ifndef AUTOCONFIGPROGRESSDIALOG_H
#define AUTOCONFIGPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include "thread/autoconfigthread.h"

namespace Ui {
class AutoConfigProgressDialog;
}

class AutoConfigProgressDialog : public QDialog
{
    Q_OBJECT
    friend class AutoConfigWidget;
public:
    explicit AutoConfigProgressDialog(QWidget *parent = nullptr);
    ~AutoConfigProgressDialog();

private:
    Ui::AutoConfigProgressDialog *ui;
    void addProgressBar(AutoConfigThread*);
};

#endif // AUTOCONFIGPROGRESSDIALOG_H
