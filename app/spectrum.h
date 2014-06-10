#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <qglobal.h>
#include "utils.h"
#include "fftreal_wrapper.h" // For FFTLengthPowerOfTwo

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of audio samples used to calculate the frequency spectrum
const int    SpectrumLengthSamples  = PowerOfTwo<FFTLengthPowerOfTwo>::Result;

// Number of bands in the frequency spectrum
const int    SpectrumNumBands       = 20;

// Lower bound of first band in the spectrum
const qreal  SpectrumLowFreq        = 0.0; // Hz

// Upper band of last band in the spectrum
const qreal  SpectrumHighFreq       = 1000.0; // Hz

// Waveform window size in microseconds
const qint64 WaveformWindowDuration = 100 * 1000;

// Length of waveform tiles in bytes
// Ideally, these would match the QAudio*::bufferSize(), but that isn't
// available until some time after QAudio*::start() has been called, and we
// need this value in order to initialize the waveform display.
// We therefore just choose a sensible value.
// const int   WaveformTileLength      = 4096;

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

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

// Macro which connects a signal to a slot, and which causes application to
// abort if the connection fails.  This is intended to catch programming errors
// such as mis-typing a signal or slot name.  It is necessary to write our own
// macro to do this - the following idiom
//     Q_ASSERT(connect(source, signal, receiver, slot));
// will not work because Q_ASSERT compiles to a no-op in release builds.

#define CHECKED_CONNECT(source, signal, receiver, slot) \
    if (!connect(source, signal, receiver, slot)) \
        qt_assert_x(Q_FUNC_INFO, "CHECKED_CONNECT failed", __FILE__, __LINE__);

// Handle some dependencies between macros defined in the .pro file

#undef SUPERIMPOSE_PROGRESS_ON_WAVEFORM

#endif // SPECTRUM_H

