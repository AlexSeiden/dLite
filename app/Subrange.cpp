#include "Subrange.h"
#include <math.h>
#include <QDebug>

Subrange::Subrange() :
    _freqMin(100.0),
    _freqMax(200.0),
    _ampMin(0.0),
    _ampMax(1.0)
{
    // Register this type so we can emit to spectrograph.
    // ??? is there any problem with this happening more than once?
    // I don't think so, because I see it in Qt example code.
    qRegisterMetaType<Subrange>("Subrange");
}

Subrange::~Subrange() { }

/*
 * Returns 0.0 if the given amplitude is below the minimum
 * threshold for the subrange, 1.0 if it's above it, and
 * a proportional value in between.
 */
double Subrange::amplitudeWithinWindow(double amp)
{
    if (amp < _ampMin)
        return 0.0;
    if (amp > _ampMax)
        return 1.0;
    return ((amp-_ampMin)/(_ampMax-_ampMin));
}

/*
 * Returns true if a given frequency is within the window
 * set for this range.
 */
bool Subrange::isFrequencyWithinWindow(double freq)
{
    if (freq < _freqMin)
        return false;
    if (freq > _freqMax)
        return false;
    return true;
}

double freq2frac(double freq, double minOverallFreq, double maxOverallFreq)
{
    const double log_min = log10(minOverallFreq);
    const double log_max = log10(maxOverallFreq);
    const double log_freq = log10(freq);
    const double delta_log = log_max - log_min;

    return ((log_freq - log_min)/delta_log);
}

void Subrange::computeWinFromRange(double minWindowFreq, double maxWindowFreq)
{
    // Invert amplitudes, since drawing is y increases downwards.
    double ampMin = 1.0 - _ampMin;
    double ampMax = 1.0 - _ampMax;

    double minSubwindowFreq = freq2frac(_freqMin, minWindowFreq, maxWindowFreq);
    double maxSubwindowFreq = freq2frac(_freqMax, minWindowFreq, maxWindowFreq);

    QRectF calcWindow;
    calcWindow.setCoords(minSubwindowFreq, ampMax, maxSubwindowFreq, ampMin);
#if 0
    _subrangeWindow = calcWindow;
#else
    qDebug() << "orig window" << _subrangeWindow;
    qDebug() << "calc window" << calcWindow;
#endif

}
