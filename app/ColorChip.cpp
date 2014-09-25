#include "ColorChip.h"
#include <QColorDialog>

ColorChip::ColorChip(QWidget *parent) :
    QToolButton(parent)
{
    this->connect(this, SIGNAL(clicked()), this, SLOT(launchColorDialog()));
}


void ColorChip::launchColorDialog() {
    QColor outcol = QColorDialog::getColor(_color, NULL);
    if (! outcol.isValid())
        return;

    Lightcolor val = Lightcolor(outcol);
    setButtonColor(outcol);
    emit valueChanged(val);
}


void ColorChip::setButtonColor(const QColor col){
    if(col.isValid()) {
        _color = col;
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1").arg(col.name());
        this->setStyleSheet(qss);
    }
}
