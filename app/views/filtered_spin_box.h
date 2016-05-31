#ifndef FILTERED_SPIN_BOX_H
#define FILTERED_SPIN_BOX_H

#include <QDoubleSpinBox>

class FilteredSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit FilteredSpinBox(QWidget *parent = 0);

signals:

public slots:
    void clearFocusSlot();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif  // FILTERED_SPIN_BOX_H
