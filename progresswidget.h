#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>

namespace Ui {
class ProgressWidget;
}

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QString fileName, QWidget *parent = nullptr);
    void setStatusLabel(QString status);
    ~ProgressWidget();

    const QString &getFileName() const;
    void updateStatus(const unsigned short);

private:
    Ui::ProgressWidget *ui;
    QString fileName;

public slots:
    void updateProgressBar(int);
};

#endif // PROGRESSWIDGET_H
