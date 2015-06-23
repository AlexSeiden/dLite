#include "MyDoubleSpinBox.h"
#include "utils.h"

#include <QEvent>

MyDoubleSpinBox::MyDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    installEventFilter(this);
    CHECKED_CONNECT(this, SIGNAL(editingFinished()), this, SLOT(clearFocusSlot()));
}

bool MyDoubleSpinBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}


void MyDoubleSpinBox::clearFocusSlot()
{
    clearFocus();
}
