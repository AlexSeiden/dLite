#include "sublevel.h"

#include <math.h>

#include <QPainter>
#include <QTimer>
#include <QDebug>


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


#if 0
void
Subrange::setWindow(QRect geo, QRect winrect)
{
    freqMin = float(geo.left())/winrect.width();
    subrangeWindow.setLeft(freqMin);  // For drawing
    freqMin = frac2freq(freqMin);     // Actual frequency

    freqMax = float(geo.right())/winrect.width();
    subrangeWindow.setRight(freqMax);
    freqMax = frac2freq(freqMax);

    ampMin = float(geo.bottom())/winrect.height();
    ampMax = float(geo.top())/winrect.height();
    subrangeWindow.setBottom(ampMin);
    subrangeWindow.setTop(ampMax);

    // Invert range, since window y increases from top to bottom.
    ampMin = 1.0 - ampMin;
    ampMax = 1.0 - ampMax;

}
#endif



SublevelMeter::SublevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   m_rmsLevel(0.0)
    ,   m_rmsColor(Qt::red)
    ,   m_squareColor(Qt::blue)
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

// TODO remove unused
void SublevelMeter::levelChanged(qreal rmsLevel, qreal peakLevel, int numSamples)
{
    Q_UNUSED(numSamples);
    Q_UNUSED(peakLevel);
    m_rmsLevel = rmsLevel;
    update();
}

void SublevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    // Draw bar
    QRect bar = rect();

    bar.setTop(rect().top() + (1.0 - m_rmsLevel) * rect().height());
    painter.fillRect(bar, m_rmsColor);


    QRect squareRect = rect();
    squareRect.setBottom(rect().top()+ rect().width());
    QColor pulseColor;
    pulseColor.setHsvF(0.0, 0.0, m_rmsLevel);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);

}
