#include "frequencyspectrum.h"
#include "utils.h"

FrequencySpectrum::FrequencySpectrum(int numPoints)
    :   m_elements(numPoints)
{

}

void FrequencySpectrum::reset()
{
    iterator i = begin();
    for ( ; i != end(); ++i)
        *i = Element();
}

int FrequencySpectrum::count() const
{
    return m_elements.count();
}

FrequencySpectrum::Element &FrequencySpectrum::operator[](int index)
{
    return m_elements[index];
}

const FrequencySpectrum::Element &FrequencySpectrum::operator[](int index) const
{
    return m_elements[index];
}

FrequencySpectrum::iterator FrequencySpectrum::begin()
{
    return m_elements.begin();
}

FrequencySpectrum::iterator FrequencySpectrum::end()
{
    return m_elements.end();
}

FrequencySpectrum::const_iterator FrequencySpectrum::begin() const
{
    return m_elements.begin();
}

FrequencySpectrum::const_iterator FrequencySpectrum::end() const
{
    return m_elements.end();
}

void FrequencySpectrum::printSpectrum()
{
    qDebug() << "FrequencySpectrum::print\n num elements:"
                           << m_elements.count();

    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = this->begin();
    const FrequencySpectrum::const_iterator end = this->end();
    for ( int index=0; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;
        qDebug() << "index"<<index << "freq" <<e.m_frequency << "amp" <<e.m_amplitude
                 << "phase" <<e.m_phase << "clipped" << e.m_clipped;

    }
}
