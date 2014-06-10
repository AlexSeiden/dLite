#ifndef FFTREAL_WRAPPER_H
#define FFTREAL_WRAPPER_H

#include <QtCore/QtGlobal>

#if defined(FFTREAL_LIBRARY)
#  define FFTREAL_EXPORT Q_DECL_EXPORT
#else
#  define FFTREAL_EXPORT Q_DECL_IMPORT
#endif

class FFTRealWrapperPrivate;

// Each pass of the FFT processes 2^X samples, where X is the
// number below.
static const int FFTLengthPowerOfTwo = 12;

/**
 * Wrapper around the FFTRealFixLen template provided by the FFTReal
 * library
 *
 * This class instantiates a single instance of FFTRealFixLen, using
 * FFTLengthPowerOfTwo as the template parameter.  It then exposes
 * FFTRealFixLen<FFTLengthPowerOfTwo>::do_fft via the calculateFFT
 * function, thereby allowing an application to dynamically link
 * against the FFTReal implementation.
 *
 * See http://ldesoras.free.fr/prod.html
 */
class FFTREAL_EXPORT FFTRealWrapper
{
public:
    FFTRealWrapper();
    ~FFTRealWrapper();

    typedef float DataType;
    void calculateFFT(DataType in[], const DataType out[]);

private:
    FFTRealWrapperPrivate*  m_private;
};

#endif // FFTREAL_WRAPPER_H

