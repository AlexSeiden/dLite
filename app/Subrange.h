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
        m_freqMin = fmin;
        m_freqMax = fmax;
        m_ampMin  = amin;
        m_ampMax  = amax;
    }

    double  amplitudeWithinWindow(double amp);
    bool    isFrequencyWithinWindow(double freq);

protected:
    float m_freqMin, m_freqMax;
    float m_ampMin,  m_ampMax;

    friend class SublevelNode;
    friend class Spectrograph;
};

// Allows Subrange to be passed via signals & slots
Q_DECLARE_METATYPE(Subrange)

#endif // SUBRANGE_H
