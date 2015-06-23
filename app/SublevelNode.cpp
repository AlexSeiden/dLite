#include "SublevelNode.h"
#include "NodeFactory.h"
#include "utils.h"
#include "Cupid.h"
#include "engine.h"

// Sublevel node is unusual, since its Parameters are really the subrange it
// operates on.  As such, it doesn't have "Params" like most other nodes do.
SublevelNode::SublevelNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Spectral Range%1").arg(nodeCount()));
    m_type = FLOAT;

    addParam<float>("out", 0.0, true);

    // connect level to rangemeter widget for display
    CHECKED_CONNECT(Cupid::Singleton()->getEngine(),
                    SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
                    this,
                    SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    // Update the spectrograph with selections, so that
    // it can display selected window, and send changes
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

SublevelNode::~SublevelNode()
{
    // Deselect when deleted, otherwise
    emit sublevelDeselected(this);
}

void SublevelNode::spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position)
    Q_UNUSED(length)

    m_spectrum = spectrum;
    if (isActive()) {
        calculateLevel();
        // levelChanged signal is connected to the appropriate slot
        // in any subrangemeter widgets in SublevelNodeItem ctor
        float out;
        getValue("out", out);
        emit levelChanged(out);
    }
}

void SublevelNode::calculateLevel()
{
    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    int index = 0;

    float value = 0;
    int nsamples = 0;
    bool clipped = false;

    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;

        // TODO could optimize by skipping straight to start frequency
        if (m_range.isFrequencyWithinWindow(e.m_frequency)) {
            // amplitude window
            value += m_range.amplitudeWithinWindow(e.m_amplitude);
            nsamples++;
            clipped |= e.m_clipped;
        }
    }

    // Compute subrange amplitude
    if (nsamples > 0)
        value /= nsamples;
    if (value > 1.0) {
        value = 1.0;
        clipped = true;
    }

    setValue("out", value);
    emit levelChanged(value);
}

void SublevelNode::operator()()
{
    if (evaluatedThisFrame())
        return;
    // Don't need to evalAllInputs() here because we know there are none!

    // ??? could evaluate here, "pull" style:
    // calculateLevel();
    // rather than running whenever the spectrum is updated?

    float out;
    getValue("out",out);
    // Need to do this to set qvOutput
    setValue("out",out);
}

SublevelNode* SublevelNode::clone()
{
    SublevelNode* lhs = new SublevelNode;
    cloneHelper(*lhs);
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

// Serialization
void SublevelNode::writeToJSONObj(QJsonObject &json) const
{
    // Override for serialization
    Node::writeToJSONObj(json);
    json["freqMin"] = m_range.m_freqMin;
    json["freqMax"] = m_range.m_freqMax;
    json["ampMin"] = m_range.m_ampMin;
    json["ampMax"] = m_range.m_ampMax;
}

void SublevelNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    m_range.m_freqMin = json["freqMin"].toDouble();
    m_range.m_freqMax = json["freqMax"].toDouble();
    m_range.m_ampMin = json["ampMin"].toDouble();
    m_range.m_ampMax = json["ampMax"].toDouble();
}


static Registrar<SublevelNode>   registrar("Spectral Range", Node::FLOAT);
