#ifndef XPTCONTROL_H
#define XPTCONTROL_H

#include <QWidget>

namespace Ui {
class XptControl;
}

class XptControl : public QWidget
{
    Q_OBJECT

public:
    explicit XptControl(QWidget *parent = nullptr);
    ~XptControl();

private:
    Ui::XptControl *ui;
};

#endif // XPTCONTROL_H
