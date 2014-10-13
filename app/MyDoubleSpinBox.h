#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class MyDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit MyDoubleSpinBox(QWidget *parent = 0);

signals:

public slots:
    void clearFocusSlot();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MYDOUBLESPINBOX_H
