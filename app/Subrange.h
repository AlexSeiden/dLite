#ifndef SUBRANGE_H
#define SUBRANGE_H
#include <QRectF>
#include <QMetaType>

// -----------------------------------------------------------------------------
// Subrange

// Defines a rectangle in the spectrum display:
// a minimum and maximum frequency and amplitude.
class Subrange
{
public:
    Subrange();
    ~Subrange();

    // If we make this take the current min/max freq, it could
    // calculate this wrt the correct range.
    QRectF  getWindow() {return _subrangeWindow;}  // XXX This should handle scaling to freqMin/Max
    void    setMinMax(float fmin, float fmax, float amin, float amax) {
        _freqMin = fmin;
        _freqMax = fmax;
        _ampMin = amin;
        _ampMax = amax;
    }
    void    setWin(const QRectF &win) {_subrangeWindow = win;}
//    void    computeWinFromRange(double minfreq, double maxfreq);

    double  amplitudeWithinWindow(double amp);
    bool    isFrequencyWithinWindow(double freq);

protected:
    float _freqMin, _freqMax;
    float _ampMin, _ampMax;

    // XXX this will be wrong if the spectrograph min & max frequencies
    // are changed during execution.
    QRectF _subrangeWindow;

    friend class SublevelNode;
};

// Allows Subrange to be passed via signals & slots
Q_DECLARE_METATYPE(Subrange)

#endif // SUBRANGE_H
