// Copyright (C) 2014-2016 Alex Seiden 
//
#include "engine/subrange.h"
#include <math.h>
#include <QDebug>

Subrange::Subrange() :
    m_freqMin(100.0),
    m_freqMax(400.0),
    m_ampMin(0.2),
    m_ampMax(0.6)
{
    // Register this type so we can emit to spectrograph.
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
    if (amp < m_ampMin)
        return 0.0;
    if (amp > m_ampMax)
        return 1.0;
    return ((amp-m_ampMin)/(m_ampMax-m_ampMin));
}

/*
 * Returns true if a given frequency is within the window
 * set for this range.
 */
bool Subrange::isFrequencyWithinWindow(double freq)
{
    if (freq < m_freqMin)
        return false;
    if (freq > m_freqMax)
        return false;
    return true;
}
