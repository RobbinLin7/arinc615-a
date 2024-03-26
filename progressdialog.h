#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include "protocal/file_define.h"

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QStringList fileList, QWidget *parent = nullptr);
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();
    void setProgress(Hfile_info_LUS* hfiles, int num);
    void setProgress(int progress, char* stat);

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
