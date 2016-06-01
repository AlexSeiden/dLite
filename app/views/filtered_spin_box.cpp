// Copyright (c) 2014-2016 Alex Seiden
//
// A standard Qt double spin box, with wheel events filtered.

#include "views/filtered_spin_box.h"
#include "engine/utils.h"
#include <QEvent>

FilteredSpinBox::FilteredSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    installEventFilter(this);
    CHECKED_CONNECT(this, SIGNAL(editingFinished()), this, SLOT(clearFocusSlot()));
}

bool FilteredSpinBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}


void FilteredSpinBox::clearFocusSlot()
{
    clearFocus();
}
