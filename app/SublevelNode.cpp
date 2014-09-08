#include "SublevelNode.h"
#include "utils.h"

// Sublevel node is unusual, since its Parameters are really the subrange
// it operates on.  As such, it doesn't have "Params" like most other nodes
// do.
SublevelNode::SublevelNode(QObject *parent) :
    QObject(parent),
    Node()
{
    setName(QString("SublevelNode%1").arg(_nodeCount));
    _type = FLOAT;

    _output.setName("out");
    _output.setConnectable(true);
    _output.setOutput(true);

    _paramList << &_output;
    setParamParent();

    // TODO must connect
//    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(const FrequencySpectrum &)),
//            this, SLOT(spectrumChanged(const FrequencySpectrum &)));
}



//void SublevelNode::spectrumChanged(const FrequencySpectrum &spectrum)
void SublevelNode::spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position)
    Q_UNUSED(length)

    _spectrum = spectrum;
    if (_active) {
        calculateLevel();
        // TODO make sure levelChanged signal is connected to appropriate slot
        // in any subrangemeter widgets.
        emit levelChanged(_output._value);
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

    _output._value = value;
    emit levelChanged(_output._value);
}

void SublevelNode::operator()()
{
    // XXX TODO:  ALL OPERATOR()s need to only exec once per 'frame'!
    // ??? could evaluate here, "pull" style:
    // calculateLevel();
    // rather than running whenever the spectrum is updated??

    _output._qvOutput = _output._value;
}

void SublevelNode::beenSelected()
{
    // Tells the spectrograph to display the current subrange.
    emit displayThisSubrange(_range);
    emit iveBeenSelected(this);
}

static Registrar<SublevelNode>   registrar("SublevelNode", Node::FLOAT);

#ifdef NUKEME
// TODO
std::function<void()> SublevelNode::createProviderClosure()
{
    return [this] (void) { ;};
}
#endif

