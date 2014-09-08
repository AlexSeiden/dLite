#include "SublevelNode.h"
#include "utils.h"

// Sublevel node is unusual, since its Parameters are really the subrange
// it operates on.  As such, it doesn't have "Params" like most other nodes
// do.
SublevelNode::SublevelNode(QObject *parent) :
    QObject(parent),
    Node()
{
    _type = INT;
    // TODO must connect
//    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(const FrequencySpectrum &)),
//            this, SLOT(spectrumChanged(const FrequencySpectrum &)));
}


void SublevelNode::spectrumChanged(const FrequencySpectrum &spectrum)
{
    _spectrum = spectrum;
    if (_active) {
        calculateLevel();
        // TODO make sure levelChanged signal is connected to appropriate slot
        // in any subrangemeter widgets.
        emit levelChanged(_level);
    }
}

void SublevelNode::calculateLevel()
{
    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = _spectrum.begin();
    const FrequencySpectrum::const_iterator end = _spectrum.end();
    int index = 0;

    float value = 0;
    int nsamples = 0;
    bool clipped = false;

    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;

        // TODO could optimize by skipping straight to start frequency
        if (_range.isFrequencyWithinWindow(e.frequency)) {
            // amplitude window
            value += _range.amplitudeWithinWindow(e.amplitude);
            nsamples++;
            clipped |= e.clipped;
        }
    }

    // Compute subrange amplitude
    if (nsamples > 0)
        value /= nsamples;
    if (value > 1.0) {
        value = 1.0;
        clipped = true;
    }

    _level = value;
    emit levelChanged(_level);
}

#ifdef NUKEME
// TODO
std::function<void()> SublevelNode::createProviderClosure()
{
    return [this] (void) { ;};
}
#endif

