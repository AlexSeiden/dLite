#include "Subrange.h"
#include <math.h>
#include <QDebug>

Subrange::Subrange() :
    _freqMin(100.0),
    _freqMax(400.0),
    _ampMin(0.2),
    _ampMax(0.6)
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
