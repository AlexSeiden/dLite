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

    void    setMinMax(float fmin, float fmax, float amin, float amax) {
        _freqMin = fmin;
        _freqMax = fmax;
        _ampMin = amin;
        _ampMax = amax;
    }

    double  amplitudeWithinWindow(double amp);
    bool    isFrequencyWithinWindow(double freq);

protected:
    float _freqMin, _freqMax;
    float _ampMin, _ampMax;

    friend class SublevelNode;
    friend class Spectrograph;
};

// Allows Subrange to be passed via signals & slots
Q_DECLARE_METATYPE(Subrange)

#endif // SUBRANGE_H
