#ifndef ODOWNLOADDIALOG_H
#define ODOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class ODownloadDialog;
}

class ODownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ODownloadDialog(QWidget *parent = nullptr);
    ~ODownloadDialog();

private:
    Ui::ODownloadDialog *ui;
};

#endif // ODOWNLOADDIALOG_H
