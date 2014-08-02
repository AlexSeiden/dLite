#include "sublevel.h"

#include <math.h>

#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>


Subrange::Subrange() :
    freqMin(100.0),
    freqMax(200.0),
    ampMin(0.0),
    ampMax(1.0)
{
}

Subrange::~Subrange() { }

double
Subrange::amplitudeWithinWindow(double amp)
{
    if (amp < ampMin)
        return 0.0;
    if (amp > ampMax)
        return 1.0;
    return ((amp-ampMin)/(ampMax-ampMin));
}

bool
Subrange::frequencyWithinWindow(double freq)
{
    if (freq < freqMin)
        return false;
    if (freq > freqMax)
        return false;
    return true;
}


SublevelMeter::SublevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   m_rmsLevel(0.0)
    ,   m_rmsColor(Qt::red)
    ,   m_squareColor(Qt::blue)
    ,   isSelectable(false)
    ,   isSelected(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(30);
}

SublevelMeter::~SublevelMeter() {  }

void SublevelMeter::reset()
{
    m_rmsLevel = 0.0;
    update();
}

void SublevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    // TODO make colors configurable
    painter.fillRect(rect(), Qt::black);

    // Draw bar
    QRect bar = rect();
    bar.setTop(rect().top() + (1.0 - m_rmsLevel) * rect().height());
    painter.fillRect(bar, m_rmsColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setBottom(rect().top()+ rect().width());
    squareRect.setRight(rect().right()-1);
    QColor pulseColor;
    pulseColor.setHsvF(0.0, 0.0, m_rmsLevel);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);

    if (isSelected) {
        QRect surroundRect = rect();
        surroundRect.setSize(QSize(rect().width()-1, rect().height()-1));
        QPen pen(Qt::blue);
        pen.setWidth(6);

        painter.setPen(pen);
        painter.drawRect(rect());
    }
}

void SublevelMeter::setSelectable(bool status)
{
    isSelectable = status;
    if (!isSelectable)
        isSelected = false;
}

bool SublevelMeter::setSelection(bool status)
{
    if (!isSelectable) {
        return false;
    }
    isSelected = status;
    update();
    return true;
}

void SublevelMeter::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isSelectable) {
        event->ignore();
        return;
    }

    if (isSelected)
        setSelection(false);
    else {
        setSelection(true);
        emit(iveBeenSelected(this));
    }
}

// TODO remove unused variables
void SublevelMeter::levelChanged(qreal rmsLevel, qreal peakLevel, int numSamples)
{
    Q_UNUSED(numSamples);
    Q_UNUSED(peakLevel);
    m_rmsLevel = rmsLevel;
    update();
}
