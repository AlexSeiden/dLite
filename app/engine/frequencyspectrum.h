// Copyright (C) 2014-2016 Alex Seiden 
//
#ifndef FREQUENCYSPECTRUM_H
#define FREQUENCYSPECTRUM_H

#include <QtCore/QVector>
#include <QMetaType>

/*
 * Represents a frequency spectrum as a series of elements, each of which
 * consists of a frequency, an amplitude and a phase.
 * This is what is returned by the FFT.
 */
class FrequencySpectrum {
public:
    FrequencySpectrum(int numPoints = 0);

    struct Element {
        Element()
        :   m_frequency(0.0), m_amplitude(0.0), m_phase(0.0), m_clipped(false)
        { }

        qreal m_frequency;  // Frequency in Hertz
        qreal m_amplitude;  // Amplitude in range [0.0, 1.0]
        qreal m_phase;      // Phase in range [0.0, 2*PI]
        bool  m_clipped;    // set when m_amplitude is clipped during fft
    };

    typedef QVector<Element>::iterator          iterator;
    typedef QVector<Element>::const_iterator    const_iterator;

    void reset();

    int count()     const;
    Element&        operator[](int index);
    const Element&  operator[](int index) const;
    iterator        begin();
    iterator        end();
    const_iterator  begin() const;
    const_iterator  end() const;

    void printSpectrum();

private:
    QVector<Element> m_elements;
};

Q_DECLARE_METATYPE(FrequencySpectrum)

#endif // FREQUENCYSPECTRUM_H
