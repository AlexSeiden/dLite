// Copyright (c) 2014-2016 Alex Seiden
//
// Custom editable color button. Wow!

#include "color_chip.h"
#include <QColorDialog>
#include "views/gui_settings.h"

ColorChip::ColorChip(QWidget *parent) :
    QToolButton(parent)
{
    this->connect(this, SIGNAL(clicked()), this, SLOT(launchColorDialog()));
}


void ColorChip::launchColorDialog() {
    QColor outcol = QColorDialog::getColor(m_color, NULL);
    if (! outcol.isValid())
        return;

    Lightcolor val = Lightcolor(outcol);
    setButtonColor(outcol);
    emit valueChanged(val);
}


void ColorChip::setButtonColor(const QColor col){
    if(col.isValid()) {
        m_color = col;
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1; width:10; height:10").arg(col.name());
        this->setStyleSheet(qss);
    }
}
