#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QNetworkAddressEntry>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr, QList<QNetworkAddressEntry>* entryList = nullptr);
    bool updateEntryList(QList<QNetworkAddressEntry>* entryList = nullptr);
    ~FindDialog();
signals:
    void on_accpetBtn_clicked(int);
private:
    Ui::FindDialog *ui;

};

#endif // FINDDIALOG_H
