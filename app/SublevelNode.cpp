#include "SublevelNode.h"
#include "utils.h"
#include "Cupid.h"
#include "engine.h"

// Sublevel node is unusual, since its Parameters are really the subrange
// it operates on.  As such, it doesn't have "Params" like most other nodes
// do.
SublevelNode::SublevelNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Spectral Range%1").arg(nodeCount()));
    _type = FLOAT;

    _output.setName("out");
    _output.setConnectable(true);
    _output.setOutput(true);

    _paramList << &_output;
    setParamParent();

    // connect level to rangemeter widget for display
    CHECKED_CONNECT(Cupid::Singleton()->getEngine(),
                    SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
                    this,
                    SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    // Update the spectrograph with selections, so that
    // it can display selected window, as well as send changes
    // back to this node.
    CHECKED_CONNECT(this,
                    SIGNAL(sublevelSelected(SublevelNode*)),
                    Cupid::Singleton()->getSpectrograph(),
                    SLOT(submeterSelected(SublevelNode *)));
    CHECKED_CONNECT(this,
                    SIGNAL(sublevelDeselected(SublevelNode*)),
                    Cupid::Singleton()->getSpectrograph(),
                    SLOT(submeterDeselected(SublevelNode *)));
}

void SublevelNode::spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position)
    Q_UNUSED(length)

    _spectrum = spectrum;
    if (isActive()) {
        calculateLevel();
        // levelChanged signal is connected to the appropriate slot
        // in any subrangemeter widgets in SublevelNodeItem ctor
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
    if (evaluatedThisFrame())
        return;
    // Don't need to evalAllInputs() here because we know there are none!

    // ??? could evaluate here, "pull" style:
    // calculateLevel();
    // rather than running whenever the spectrum is updated?

    _output._qvOutput = _output._value;
}

SublevelNode* SublevelNode::clone()
{
    SublevelNode* lhs = new SublevelNode;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

void SublevelNode::beenSelected()
{
    emit sublevelSelected(this);
}

void SublevelNode::beenDeselected()
{
    emit sublevelDeselected(this);
}

void SublevelNode::writeToJSONObj(QJsonObject &json) const
{
    // Override for serialization
    Node::writeToJSONObj(json);
    json["freqMin"] = _range._freqMin;
    json["freqMax"] = _range._freqMax;
    json["ampMin"] = _range._ampMin;
    json["ampMax"] = _range._ampMax;
}

void SublevelNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    _range._freqMin = json["freqMin"].toDouble();
    _range._freqMax = json["freqMax"].toDouble();
    _range._ampMin = json["ampMin"].toDouble();
    _range._ampMax = json["ampMax"].toDouble();
}


static Registrar<SublevelNode>   registrar("Spectral Range", Node::FLOAT);
