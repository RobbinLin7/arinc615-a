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

private:
    Ui::ProgressWidget *ui;

public slots:
    void updateProgressBar(int);
};

#endif // PROGRESSWIDGET_H
