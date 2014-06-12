#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <qglobal.h>
#include "utils.h"
#include "fftreal_wrapper.h" // For FFTLengthPowerOfTwo

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of audio samples used to calculate the frequency spectrum
// Bizarre hacky way to find 2^FFTLength at compile time.
const int    SpectrumLengthSamples  = PowerOfTwo<FFTLengthPowerOfTwo>::Result;

// Waveform window size in microseconds
const qint64 WaveformWindowDuration = 100 * 1000;

// Fudge factor used to calculate the spectrum bar heights
const qreal SpectrumAnalyserMultiplier = 0.15;

// Disable message timeout
const int   NullMessageTimeout      = -1;


//-----------------------------------------------------------------------------
// Types and data structures
//-----------------------------------------------------------------------------

enum WindowFunction {
    NoWindow,
    HannWindow
};

const WindowFunction DefaultWindowFunction = HannWindow;

#endif // SPECTRUM_H

