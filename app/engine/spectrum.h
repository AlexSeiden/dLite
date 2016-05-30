#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <qglobal.h>
#include "engine/utils.h"
#include "fftreal_wrapper.h" // For FFTLengthPowerOfTwo

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of audio samples used to calculate the frequency spectrum
static const int        SPECTRUM_LENGTH_SAMPLES  = 4096; //2^FFTLengthPowerOfTwo;

// Waveform window size in microseconds
const qint64     WAVEFORM_WINDOW_DURATION = 100 * 1000;

// Fudge factor used to calculate the spectrum bar heights
const qreal     SPECTRUM_ANALYSER_MULTIPLIER = 0.15;

// Disable message timeout
const int       NULL_MESSAGE_TIMEOUT      = -1;


//-----------------------------------------------------------------------------
// Types and data structures
//-----------------------------------------------------------------------------

enum WindowFunction {
    NoWindow,
    HannWindow
};

const WindowFunction DefaultWindowFunction = HannWindow;

#endif // SPECTRUM_H

